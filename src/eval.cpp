/*
  Schizo programming language
  Copyright (C) 2014-2015  Wael El Oraiby

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Also add information on how to contact you by electronic and paper mail.
*/

#include <schizo/schizo.hpp>
#include <stdio.h>

extern "C" void __cxa_pure_virtual() { fprintf(stderr, "attempt to call a pure virtual method!!!"); exit(1); }

namespace schizo {

/*******************************************************************************
** eval
*******************************************************************************/
exp::iptr
lookup(exp::iptr env,
       const char* sym)
{
	if( env ) {
		exp::iptr	pair	= exp::list::head(env);

		while( pair && strcmp(sym, static_cast<exp::atom_symbol*>(exp::list::head(pair).get())->value()) != 0 ) {
			env	= exp::list::tail(env);
			if( env ) {
				pair	= exp::list::head(env);
			} else {
				pair	= nullptr;
			}
		}

		if( !pair ) {
			return new exp::error("symbol not found");
		} else {
			if( exp::list::tail(pair) ) {
				return exp::list::head(exp::list::tail(pair));
			} else {
				return new exp::error("symbol not found");
			}
		}
	} else {
		return new exp::error("symbol not found");
	}
}

exp::iptr
eval_list(exp::iptr env,
	  exp::iptr expr)
{
	exp::iptr	res	= nullptr;
	while( expr ) {
		exp::special::env_ret	r = eval(env, exp::list::head(expr)).eval();

		if( r.ret() && r.ret()->type() == exp::EXP_ERROR ) {
			return res;	// if error, bail early
		}

		// TODO: what happens in case of error ?
		res	= new exp::list(r.ret(), res);

		expr	= exp::list::tail(expr);
	}

	return exp::list::reverse(res);
}

exp::special::val
eval(exp::iptr env,
     exp::iptr e)
{
#ifdef DEBUG_EVAL
	fprintf(stderr, "EVAL: ");
	exp::print(e, 0);
	fprintf(stderr, "\n");
#endif	// DEBUG_EVAL

	while( e ) {	/* not a NIL_CELL */
		switch( e->type() ) {
		/* constants */
		case exp::EXP_BOOLEAN:
		case exp::EXP_CHARACTER:
		case exp::EXP_SINT64:
		case exp::EXP_REAL64:
		case exp::EXP_STRING:
		case exp::EXP_ERROR:
		case exp::EXP_LAMBDA:
		case exp::EXP_FFI:
		//case EXP_QUOTE:
			SCHIZO_RETURN(env, e);

		// symbols
		case exp::EXP_SYMBOL:
#ifdef DEBUG_LOOKUP
			fprintf(stderr, "SYMBOL %s -> eval to: ", static_cast<exp::symbol*>(e.get())->value());
#endif	// DEBUG_LOOKUP
			e	= lookup(env, static_cast<exp::atom_symbol*>(e.get())->value());
#ifdef DEBUG_LOOKUP
			print_cell(exp, 0);
			fprintf(stderr, "\n");
#endif	// DEBUG_LOOKUP
			SCHIZO_RETURN(env, e);

			/* applications */
		case exp::EXP_LIST:	{
			exp::iptr	head	= exp::list::head(e);
			exp::iptr	tail	= nullptr;

			if( !head ) {
				SCHIZO_RETURN(env, nullptr);
			}

			tail	= exp::list::tail(e);	/* NOT NEEDED ? */
			head	= eval(env, head).eval().ret();

			switch( head->type() ) {
			case exp::EXP_FFI: {
				uint32	l	= exp::list::length(tail);
				assert( l < 0x7FFFFFFF );
				if( static_cast<exp::ffi*>(head.get())->arg_count() > 0 && (sint32)l != static_cast<exp::ffi*>(head.get())->arg_count() ) {
					fprintf(stderr, "ERROR: function requires %d arguments, only %d were given\n", l, static_cast<exp::ffi*>(head.get())->arg_count());
					SCHIZO_RETURN(env, nullptr);
				} else {
					exp::iptr args	= eval_list(env, tail);

					SCHIZO_RETURN(env, (*static_cast<exp::ffi*>(head.get()))(args));
				}
				break;
			}

			case exp::EXP_SPECIAL_FORM: {
				/* lambda, bind, if */
				exp::special::env_ret r	= (*static_cast<exp::special*>(head.get()))(env, tail).eval();

				env	= r.env();
				e	= r.ret();

				break;
			}

			case exp::EXP_LAMBDA: {
				exp::iptr	args	= nullptr;
				exp::iptr	syms	= nullptr;
				exp::lambda*	lambda_	= nullptr;

				lambda_	= static_cast<exp::lambda*>(head.get());

				syms	= lambda_->syms();

				// evaluate the arguments and zip them
				if( (exp::list::length(tail) != exp::list::length(syms)) ) {
					SCHIZO_RETURN(env, new exp::error("ERROR: closure arguments do not match given arguments"));
				}

				args	= eval_list(env, tail);

				if( args && args->type() == exp::EXP_ERROR ) {
					SCHIZO_RETURN(env, args);	// if error, bail early
				}

				while( exp::list::head(syms) && exp::list::head(args) ) {
					env	= new exp::list(pair(exp::list::head(syms), exp::list::head(args)), env);
					args	= exp::list::tail(args);
					syms	= exp::list::tail(syms);
				}

				if( exp::list::length(syms) != exp::list::length(args) ) {
					SCHIZO_RETURN(env, new exp::error("ERROR: couldn't zip the lists, one is longer than the other"));
				} else {
					/* all good, evaluate the body(*) */
					exp::iptr	body	= static_cast<exp::lambda*>(lambda_)->body();
					exp::iptr	next	= exp::list::tail(body);

					e	= exp::list::head(body);

					while( next ) {
						exp::special::env_ret r	= eval(env, e).eval();	/* eval and bind */

						if( r.ret() && r.ret()->type() == exp::EXP_ERROR ) {
							return exp::special::val(r);	// if error, bail early
						}

						env	= r.env();

						e	= exp::list::head(next);
						next	= exp::list::tail(next);
					}

					/* now the tail call (env and exp are set) */
					return exp::special::val(tailcall(eval(env, e)));
				}
			}

			case exp::EXP_ERROR:
				SCHIZO_RETURN(env, head);

			default:
				/* more error handling */
				fprintf(stderr, "Unexpected type: %u\n", head->type());
				assert(0);
				break;
			}
			break;
		}

		default:
			assert( 0 );
			break;
		}
	}

	/* if expression is NIL_CELL */
	SCHIZO_RETURN(env, nullptr);
}


/*******************************************************************************
** schizo state
*******************************************************************************/
exp::iptr
add_ffi(exp::iptr env,
	const char* sym,
	sint32 arg_count,
	exp::ffi::call proc)
{
	exp::iptr	p	= nullptr;
	exp::iptr	_s	= new exp::atom_symbol(sym);

	exp::iptr	f	= new exp::ffi(arg_count, proc);

	return new exp::list(pair(_s, f), env);
}

exp::iptr
add_special(exp::iptr env,
	    const char* sym,
	    sint32 arg_count,
	    exp::special::call proc)
{
	exp::iptr	p	= nullptr;
	exp::iptr	_s	= new exp::atom_symbol(sym);

	exp::iptr	f	= new exp::special(arg_count, proc);

	return new exp::list(pair(_s, f), env);
}

exp::iptr
default_env() {
	exp::iptr env	= nullptr;
	extern exp::iptr __get_special_forms(exp::iptr env);
	extern exp::iptr __get_built_ins(exp::iptr env);

	env	= __get_special_forms(env);
	env	= __get_built_ins(env);

	return env;
}

}	// namespace schizo
