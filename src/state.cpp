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
state::lookup(exp::iptr env,
	      const char* sym)
{
	if( env ) {
		iptr	pair	= list::head(env);

		while( pair && strcmp(sym, static_cast<exp::symbol*>(list::head(pair).get())->value()) != 0 ) {
			env	= list::tail(env);
			if( env ) {
				pair	= list::head(env);
			} else {
				pair	= nullptr;
			}
		}

		if( !pair ) {
			return nullptr;
		} else {
			if( list::tail(pair) ) {
				return list::head(list::tail(pair));
			} else {
				return nullptr;
			}
		}
	} else {
		return nullptr;
	}
}

exp::iptr
state::eval_list(exp::iptr env,
		 exp::iptr expr)
{
	exp::iptr	res	= nullptr;
	while( expr ) {
		res	= new list(eval(env, list::head(expr)).value(), res);
		expr	= list::tail(expr);
	}
	return list::reverse(res);
}

schizo::exp::special::ret
state::eval(exp::iptr env,
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
		case EXP_BOOLEAN:
		case EXP_CHARACTER:
		case EXP_SINT64:
		case EXP_REAL64:
		case EXP_STRING:
		case EXP_ERROR:
		case EXP_CLOSURE:
		case EXP_FFI:
		//case EXP_QUOTE:
			return special::ret(env, e);

		// symbols
		case EXP_SYMBOL:
#ifdef DEBUG_LOOKUP
			fprintf(stderr, "SYMBOL %s -> eval to: ", static_cast<symbol*>(e.get())->value());
#endif	// DEBUG_LOOKUP
			e	= lookup(env, static_cast<exp::symbol*>(e.get())->value());
#ifdef DEBUG_LOOKUP
			print_cell(exp, 0);
			fprintf(stderr, "\n");
#endif	// DEBUG_LOOKUP
			return special::ret(env, e);

			/* applications */
		case EXP_LIST:	{
			iptr	head	= list::head(e);
			iptr	tail	= nullptr;

			if( !head ) {
				return special::ret(env, nullptr);
			}

			tail	= list::tail(e);	/* NOT NEEDED ? */
			head	= eval(env, head).value();

			switch( head->type() ) {
			case EXP_FFI: {
				uint32	l	= list::length(tail);
				assert( l < 0x7FFFFFFF );
				if( static_cast<ffi*>(head.get())->arg_count() > 0 && (sint32)l != static_cast<ffi*>(head.get())->arg_count() ) {
					fprintf(stderr, "ERROR: function requires %d arguments, only %d were given\n", l, static_cast<ffi*>(head.get())->arg_count());
					return special::ret(env, nullptr);
				} else {
					iptr args	= eval_list(env, tail);

					return special::ret(env, (*static_cast<ffi*>(head.get()))(args));
				}
				break;
			}

			case EXP_SPECIAL_FORM: {
				/* lambda, define, if */
				exp::special::ret r	= (*static_cast<special*>(head.get()))(env, tail);

				env	= r.env();
				e	= r.value();

				break;
			}

			case EXP_CLOSURE: {
				iptr	args	= nullptr;
				iptr	syms	= nullptr;
				iptr	lambda_	= nullptr;

				lambda_	= static_cast<closure*>(head.get())->lambda();
				env	= static_cast<closure*>(head.get())->env();

				syms	= static_cast<lambda*>(lambda_.get())->syms();

				// evaluate the arguments and zip them
				if( (list::length(tail) != list::length(syms)) ) {
					return special::ret(env, new error("ERROR: closure arguments do not match given arguments"));
				}

				args	= eval_list(env, tail);

				while( list::head(syms) && list::head(args) ) {
					env	= new list(pair(list::head(syms), list::head(args)), env);
					args	= list::tail(args);
					syms	= list::tail(syms);
				}

				if( list::length(syms) != list::length(args) ) {
					return special::ret(env, new error("ERROR: couldn't zip the lists, one is longer than the other"));
				} else {
					/* all good, evaluate the body(*) */
					iptr	body	= static_cast<lambda*>(lambda_.get())->body();
					iptr	next	= list::tail(body);

					e	= list::head(body);

					while( next ) {
						special::ret r	= eval(env, e);	/* eval and bind */
						env	= r.env();

						e	= list::head(next);
						next	= list::tail(next);
					}

					/* now the tail call (env and exp are set) */
				}
				break;
			}

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
	return special::ret(env, nullptr);
}


/*******************************************************************************
** schizo state
*******************************************************************************/
exp::iptr
state::add_ffi(iptr env,
	       const char* sym,
	       sint32 arg_count,
	       ffi::call proc)
{
	exp::iptr	p	= nullptr;
	exp::iptr	_s	= new symbol(sym);

	exp::iptr	f	= new ffi(arg_count, proc);

	return new list(pair(_s, f), env);
}

exp::iptr
state::add_special(iptr env,
		   const char* sym,
		   sint32 arg_count,
		   special::call proc)
{
	exp::iptr	p	= nullptr;
	exp::iptr	_s	= new symbol(sym);

	exp::iptr	f	= new special(arg_count, proc);

	return new list(pair(_s, f), env);
}

state::state() : exp(EXP_STATE) {
	parser_.token_start	= nullptr;
	parser_.token_end	= nullptr;
	parser_.token_line	= 0;
}

state::~state() {
}

exp::iptr
state::default_env() {
	iptr env	= nullptr;
	extern exp::iptr __get_special_forms(exp::iptr env);
	extern exp::iptr __get_built_ins(exp::iptr env);

	env	= __get_special_forms(env);
	env	= __get_built_ins(env);

	return env;
}

}	// namespace schizo
