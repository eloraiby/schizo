/*
  Schizo programming language
  Copyright (C) 2014  Wael El Oraiby

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

#include "parser.h"
#include <schizo/schizo.hpp>
#include <stdio.h>

extern "C" void __cxa_pure_virtual() { fprintf(stderr, "attempt to call a pure virtual method!!!"); exit(1); }

namespace schizo {

exp::~exp() {

}

/*******************************************************************************
** printing
*******************************************************************************/

static void
print_level(uint32 level) {
	uint32	i	= 0;
	for( i = 0; i < level; ++i ) {
		fprintf(stderr, "  ");
	}
}

void
print_cell(exp::iptr c,
	   uint32 level)
{
	print_level(level);
	if( c ) {
		switch( c->type() ) {
		case ATOM_BOOL:
			if( static_cast<exp::boolean*>(c.get())->value() ) {
				fprintf(stderr, "#t");
			} else {
				fprintf(stderr, "#f");
			}
			break;

		case ATOM_SYMBOL:
			fprintf(stderr, "%s", static_cast<exp::symbol*>(c.get())->value());
			break;

		case ATOM_CHAR:
			fprintf(stderr, "'%c'", static_cast<exp::character*>(c.get())->value());
			break;

		case ATOM_SINT64:
			fprintf(stderr, "%ld", static_cast<exp::sint64*>(c.get())->value());
			break;

		case ATOM_REAL64:
			fprintf(stderr, "%lf", static_cast<exp::real64*>(c.get())->value());
			break;

		case ATOM_STRING:
			fprintf(stderr, "\"%s\"", static_cast<exp::string*>(c.get())->value());
			break;

		case ATOM_ERROR:
			fprintf(stderr, "<ERROR>");
			break;

		case CELL_CLOSURE:
			fprintf(stderr, "<CLOSURE>");
			break;

		case CELL_FFI:
			fprintf(stderr, "<FFI>");
			break;

		case CELL_LIST:
			if( exp::list::head(c) ) {
				fprintf(stderr, "(");
				print_cell(exp::list::head(c), uint32(0));
			} else {
				fprintf(stderr, "(nil");
			}

			if( exp::list::tail(c) ) {
				exp::iptr n	= exp::list::tail(c);
				while( n ) {
					if( exp::list::head(n) ) {
						print_cell(exp::list::head(n), 1);
					} else {
						print_level(level + 1);
						fprintf(stderr, "nil");
					}

					n	= exp::list::tail(n);
				}
			}
			fprintf(stderr, ")");
			break;
		default:
			break;
		}
	} else {
		fprintf(stderr, "nil");
	}
}


/*******************************************************************************
** lists
*******************************************************************************/
uint32
exp::list::length(exp::iptr l)
{
	exp::iptr	curr	= l;
	uint32		len	= 0;

	while( curr ) {
		assert( l->type() == CELL_LIST );
		++len;
		curr	= list::tail(curr);
	}
	return len;
}

exp::iptr
exp::list::reverse(exp::iptr l)
{
	exp::iptr	nl	= nullptr;

	while( l ) {
		nl	= new list(list::head(l), nl);
		l	= list::tail(l);
	}

	return nl;
}

/*
cell_ptr_t
list_zip(state_t* s,
	 cell_ptr_t l0,
	 cell_ptr_t l1)
{
	cell_ptr_t	res	= NIL_CELL;
	cell_ptr_t	pair	= NIL_CELL;

	while( !is_nil(list_head(s, l0)) && !is_nil(list_head(s, l1)) ) {
		set_cell(s, pair, list_make_pair(s, list_head(s, l0), list_head(s, l1)));

		set_cell(s, res, list_cons(s, pair, res));

		set_cell(s, l0, list_tail(s, l0));
		set_cell(s, l1, list_tail(s, l1));
	}

	if( is_nil(l0) != is_nil(l1) ) {
		return schizo_error(s, "ERROR: couldn't zip the lists, one is longer than the other");
	} else {
		return list_reverse_in_place(s, res);
	}
}
*/
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
		res	= new list(eval(env, list::head(expr)), res);
		expr	= list::tail(expr);
	}
	return list::reverse(res);
}

exp::iptr
state::apply_bind(iptr env,
		  exp::iptr b)
{
	if( b->type() == CELL_BIND ) {
		iptr	sympair = static_cast<bind*>(b.get())->bindings();
		while( sympair ) {
			env = new list(list::head(sympair), env);
			sympair	= list::tail(sympair);
		}
	}

	return env;
}

static void
print_env(exp::iptr env)
{
	fprintf(stderr, "------------------------\n");
	while( env ) {
		fprintf(stderr, "* %s :: ", static_cast<exp::symbol*>(exp::list::head(exp::list::head(env)).get())->value());
		print_cell(exp::list::head(exp::list::tail(exp::list::head(env))), 0);
		fprintf(stderr, "\n");
		env	= exp::list::tail(env);
	}
}

exp::iptr
state::eval(exp::iptr env,
	    exp::iptr exp)
{
#ifdef DEBUG_EVAL
	fprintf(stderr, "EVAL: ");
	print_cell(exp, 0);
	fprintf(stderr, "\n");
#endif	// DEBUG_EVAL

	while( exp ) {	/* not a NIL_CELL */
		switch( exp->type() ) {
		/* constants */
		case ATOM_BOOL:
		case ATOM_CHAR:
		case ATOM_SINT64:
		case ATOM_REAL64:
		case ATOM_STRING:
		case ATOM_ERROR:
		case CELL_CLOSURE:
		case CELL_FFI:
		case CELL_QUOTE:
			return exp;
		case CELL_BIND:
			return apply_bind(env, exp);

			/* symbols */
		case ATOM_SYMBOL:
#ifdef DEBUG_LOOKUP
			fprintf(stderr, "SYMBOL %s -> eval to: ", static_cast<symbol*>(exp.get())->value());
#endif	// DEBUG_LOOKUP
			exp	= lookup(env, static_cast<exp::symbol*>(exp.get())->value());
#ifdef DEBUG_LOOKUP
			print_cell(exp, 0);
			fprintf(stderr, "\n");
#endif	// DEBUG_LOOKUP
			return exp;

			/* applications */
		case CELL_LIST:	{
			iptr	head	= list::head(exp);
			iptr	tail	= nullptr;

			if( !head ) {
				return nullptr;
			}

			tail	= list::tail(exp);	/* NOT NEEDED ? */
			head	= eval(env, head);

			switch( head->type() ) {
			case CELL_FFI: {
				uint32	l	= list::length(tail);
				assert( l < 0x7FFFFFFF );
				if( static_cast<ffi*>(head.get())->arg_count() > 0 && (sint32)l != static_cast<ffi*>(head.get())->arg_count() ) {
					fprintf(stderr, "ERROR: function requires %d arguments, only %d were given\n", l, static_cast<ffi*>(head.get())->arg_count());
					return nullptr;
				} else {
					iptr args	= eval_list(env, tail);

					return (*static_cast<ffi*>(head.get()))(args);
				}
				break;
			}

			case CELL_SPECIAL_FORM:
				/* lambda, define, if */
				exp	= (*static_cast<special*>(head.get()))(env, tail);
				if( exp->type() == CELL_BIND ) {	// bind needs to be returned immediately
					return exp;
				}
				break;

			case CELL_CLOSURE: {
				iptr	args	= nullptr;
				iptr	syms	= nullptr;
				iptr	lambda_	= nullptr;

				lambda_	= static_cast<closure*>(head.get())->lambda();
				env	= static_cast<closure*>(head.get())->env();

				syms	= static_cast<lambda*>(lambda_.get())->syms();

				// evaluate the arguments and zip them
				if( (list::length(tail) != list::length(syms)) ) {
					return new error("ERROR: closure arguments do not match given arguments");
				}

				args	= eval_list(env, tail);

				while( list::head(syms) && list::head(args) ) {
					env	= new list(pair(list::head(syms), list::head(args)), env);
					args	= list::tail(args);
					syms	= list::tail(syms);
				}

				if( list::length(syms) != list::length(args) ) {
					return new error("ERROR: couldn't zip the lists, one is longer than the other");
				} else {
					/* all good, evaluate the body(*) */
					iptr	body	= static_cast<lambda*>(lambda_.get())->body();
					iptr	next	= list::tail(body);
					iptr	ret	= nullptr;

					exp	= list::head(body);
					if( exp && exp->type() == CELL_BIND ) {	// bind needs to be treated here
						env = apply_bind(env, exp);
					}

					while( next ) {
						ret	= eval(env, exp);	/* eval and bind */
						if( ret && ret->type() == CELL_BIND ) {	// bind needs to be treated here
							env = apply_bind(env, ret);
						}

						exp	= list::head(next);
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
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
/// special functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief symbol_define
 * @param s
 * @param env
 * @param args
 * @return
 */
static exp::iptr
symbol_define(exp::iptr UNUSED env,
	      exp::iptr args)
{
	exp::iptr sym	= exp::list::head(args);
	exp::iptr body	= exp::list::head(exp::list::tail(args));

	exp::iptr pair_	= pair(sym, body);

	return new exp::bind(new exp::list(pair_, nullptr));
}

/**
 * @brief make_closure
 * @param s
 * @param env
 * @param args
 * @return
 */
static exp::iptr
make_closure(exp::iptr env,
	     exp::iptr args)
{
	// the new nullptr list is needed in case we pass a NIL argument ()
	exp::iptr	syms	= exp::list::head(args) ? args : new exp::list(nullptr, nullptr);
	exp::iptr	body	= exp::list::tail(args);

	exp::iptr	lam	= new exp::lambda(syms, body);

	return new exp::closure(env, lam);
}

static exp::iptr
if_else(exp::iptr env,
	exp::iptr args)
{
	if( exp::list::length(args) != 4 ) {
		return new exp::error("ERROR in \"if\" usage: if cond exp0 else exp1");
	}

	exp::iptr cond		= exp::list::head(args);
	exp::iptr exp0		= exp::list::head(exp::list::tail(args));
	exp::iptr elsym	= exp::list::head(exp::list::tail(exp::list::tail(args)));
	exp::iptr exp1		= exp::list::head(exp::list::tail(exp::list::tail(exp::list::tail(args))));

	if( elsym->type() == ATOM_SYMBOL && strcmp(static_cast<exp::symbol*>(elsym.get())->value(), "else") == 0 ) {
		exp::iptr b	= state::eval(env, cond);
		switch( b->type() ) {
		case ATOM_ERROR:
			return b;

		case ATOM_BOOL:
			if( static_cast<exp::boolean*>(b.get())->value() ) {
				return exp0;
			} else {
				return exp1;
			}

		default:
			return new exp::error("ERROR: \"if\" requires condition to be boolean");
		}
	} else {
		return new exp::error("ERROR: \"if\" requires \"else\" keyword: if cond exp0 else exp1");
	}
}

////////////////////////////////////////////////////////////////////////////////
/// built-in functions
////////////////////////////////////////////////////////////////////////////////
static exp::iptr
less_than(exp::iptr args)
{
	exp::iptr	left	= exp::list::head(args);
	exp::iptr	right	= exp::list::head(exp::list::tail(args));

	if( left->type() != right->type() &&
	    left->type() != ATOM_REAL64 &&
	    left->type() != ATOM_SINT64 ) {
		return new exp::error("lt takes 2 values of the same number type");
	}

	switch(left->type()) {
	case ATOM_REAL64:
		return new exp::boolean(static_cast<exp::real64*>(left.get())->value() < static_cast<exp::real64*>(right.get())->value());
	case ATOM_SINT64:
		return new exp::boolean(static_cast<exp::sint64*>(left.get())->value() < static_cast<exp::sint64*>(right.get())->value());
	}

	return new exp::error("lt should not pass here");
}

static exp::iptr
display(exp::iptr args)
{
	print_cell(args, 0);
	return nullptr;
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

state::state() : exp(CELL_STATE) {
	parser_.token_start	= nullptr;
	parser_.token_end	= nullptr;
	parser_.token_line	= 0;
}

state::~state() {
}

exp::iptr
state::default_env() {
	iptr env	= nullptr;
	env = add_special(env, "lambda",  -1,	make_closure);
	env = add_special(env, "define",  2,	symbol_define);
	env = add_special(env, "if",      4,	if_else);

	env = add_ffi    (env, "display", 1,	display);
	env = add_ffi    (env, "lt",	  2,	less_than);
	return env;
}

}	// namespace schizo
