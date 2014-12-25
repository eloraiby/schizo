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

#include "schizo.h"
#include <stdio.h>

extern "C" void __cxa_pure_virtual() { while(1); }

namespace schizo {

bool cell::clear_to_destroy	= true;

cell::~cell() {

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
print_cell(cell::iptr c,
	   uint32 level)
{
	print_level(level);
	if( c ) {
		switch( c->type() ) {
		case ATOM_BOOL:
			if( static_cast<bool_cell*>(c.get())->value() ) {
				fprintf(stderr, "#t");
			} else {
				fprintf(stderr, "#f");
			}
			break;

		case ATOM_SYMBOL:
			fprintf(stderr, "%s", static_cast<symbol*>(c.get())->value());
			break;

		case ATOM_CHAR:
			fprintf(stderr, "'%c'", static_cast<char_cell*>(c.get())->value());
			break;

		case ATOM_SINT64:
			fprintf(stderr, "%ld", static_cast<sint64_cell*>(c.get())->value());
			break;

		case ATOM_REAL64:
			fprintf(stderr, "%lf", static_cast<real64_cell*>(c.get())->value());
			break;

		case ATOM_STRING:
			fprintf(stderr, "\"%s\"", static_cast<string_cell*>(c.get())->value());
			break;

		case CELL_LIST:
			if( list::head(c) ) {
				fprintf(stderr, "(");
				print_cell(list::head(c), uint32(0));
			} else {
				fprintf(stderr, "(nil");
			}

			if( list::tail(c) ) {
				cell::iptr n	= list::tail(c);
				while( n ) {
					if( list::head(n) ) {
						print_cell(list::head(n), 1);
					} else {
						print_level(level + 1);
						fprintf(stderr, "nil");
					}

					n	= list::tail(n);
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
list::length(cell::iptr l)
{
	cell::iptr	curr	= l;
	uint32		len	= 0;

	while( curr ) {
		assert( l->type() == CELL_LIST );
		++len;
		curr	= list::tail(curr);
	}
	return len;
}

cell::iptr
list::reverse(cell::iptr l)
{
	cell::iptr	nl	= nullptr;

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
cell::iptr
state::lookup(cell::iptr env,
	      const char* sym)
{
	if( env ) {
		iptr	pair	= list::head(env);

		while( pair && strcmp(sym, static_cast<symbol*>(list::head(pair).get())->value()) != 0 ) {
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

cell::iptr
state::eval_list(cell::iptr env,
		 cell::iptr expr)
{
	cell::iptr	res	= nullptr;
	while( expr ) {
		res	= new list(eval(env, list::head(expr)), res);
		expr	= list::tail(expr);
	}
	return list::reverse(res);
}

cell::iptr
state::apply_bind(iptr env,
		  cell::iptr b)
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

/*
static void
print_env(state_t* s)
{
	cell_ptr_t l	= s->registers.current_env;
	while( l != NIL_CELL ) {
		fprintf(stderr, "* %s\n", list_head(list_head(l))->object.symbol);
		l	= list_tail(l);
	}
}
*/

cell::iptr
state::eval(cell::iptr env,
	    cell::iptr exp)
{
	while( exp ) {	/* not a NIL_CELL */
		/* print_env(s); */
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
			fprintf(stderr, "SYMBOL LOOKUP: %s\n", static_cast<symbol*>(exp.get())->value());
			return lookup(env, static_cast<symbol*>(exp.get())->value());

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
			case CELL_FFI:
				if( static_cast<ffi*>(head.get())->flags() & EVAL_ARGS ) {
					/* TODO: this is limited to 32768 elements! need to increase the range */
					uint32	l	= list::length(tail);
					assert( l < 0x7FFF );
					if( static_cast<ffi*>(head.get())->arg_count() > 0 &&
					    (sint16)l != static_cast<ffi*>(head.get())->arg_count() ) {
						fprintf(stderr, "ERROR: function requires %d arguments, only %d were given\n", l, static_cast<ffi*>(head.get())->arg_count());
						return nullptr;
					} else {

						iptr args	= eval_list(env, tail);

						return (*static_cast<ffi*>(head.get()))(env, args);
					}
				} else {	/* lambda, define, if */
					exp	= (*static_cast<ffi*>(head.get()))(env, tail);
					if( exp->type() == CELL_BIND ) {	// bind needs to be returned immediately
						return exp;
					}
				}
				break;
			case CELL_CLOSURE: {
				iptr	args	= nullptr;
				iptr	syms	= nullptr;
				iptr	lambda_	= nullptr;

				/*print_cell(s, tail, 0);*/

				lambda_	= static_cast<closure*>(head.get())->lambda();
				env	= static_cast<closure*>(head.get())->env();

				/* evaluate the arguments and zip them */
				if( list::length(tail) != list::length(static_cast<lambda*>(lambda_.get())->syms()) ) {
					return new error("ERROR: closure arguments do not match given arguments");
				}

				args	= eval_list(env, tail);
				syms	= static_cast<lambda*>(lambda_.get())->syms();

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
					if( exp->type() == CELL_BIND ) {	// bind needs to be treated here
						env = apply_bind(env, exp);
					}

					while( next ) {
						/* print_env(s); */
						ret	= eval(env, exp);	/* eval and bind */
						if( ret->type() == CELL_BIND ) {	// bind needs to be treated here
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

/*******************************************************************************
** core functions
*******************************************************************************/

/**
 * @brief symbol_define
 * @param s
 * @param env
 * @param args
 * @return
 */
static cell::iptr
symbol_define(cell::iptr env,
	      cell::iptr args)
{
	cell::iptr sym	= list::head(args);
	cell::iptr body	= list::head(list::tail(args));

	cell::iptr pair_	= pair(sym, body);

	return new bind(new list(pair_, nullptr));
}

/**
 * @brief make_closure
 * @param s
 * @param env
 * @param args
 * @return
 */
static cell::iptr
make_closure(cell::iptr env,
	     cell::iptr args)
{
	cell::iptr	syms	= list::head(args);
	cell::iptr	body	= list::tail(args);

	cell::iptr	lam	= new lambda(syms, body);

	return new closure(env, lam);
}

static cell::iptr
if_else(cell::iptr env,
	cell::iptr args)
{
	if( list::length(args) != 4 ) {
		return new error("ERROR in \"if\" usage: if cond exp0 else exp1");
	}

	cell::iptr cond		= list::head(args);
	cell::iptr exp0		= list::head(list::tail(args));
	cell::iptr elsym	= list::head(list::tail(list::tail(args)));
	cell::iptr exp1		= list::head(list::tail(list::tail(list::tail(args))));

	if( elsym->type() == ATOM_SYMBOL && strcmp(static_cast<symbol*>(elsym.get())->value(), "else") == 0 ) {
		cell::iptr b	= state::eval(env, cond);
		if( b->type() != ATOM_BOOL ) {
			return new error("ERROR: if requires condition to be boolean");
		}

		if( static_cast<bool_cell*>(b.get())->value() ) {
			return exp0;
		} else {
			return exp1;
		}
	} else {
		return new error("ERROR: if requires \"else\" keyword: if cond exp0 else exp1");
	}
}

static cell::iptr
display(cell::iptr env,
	cell::iptr args)
{
	print_cell(args, 0);
	return nullptr;
}

/*******************************************************************************
** schizo state
*******************************************************************************/
cell::iptr
state::add_ffi(iptr env,
	       const char* sym,
	       uint16 flags,
	       sint32 arg_count,
	       ffi_call_t proc)
{
	cell::iptr	p	= nullptr;
	cell::iptr	_s	= new symbol(sym);

	cell::iptr	f	= new ffi(flags, arg_count, proc);

	return new list(pair(_s, f), env);
}

state::state() : cell(CELL_STATE) {
	parser_.token_start	= nullptr;
	parser_.token_end	= nullptr;
	parser_.token_line	= 0;
}

state::~state() {
}

cell::iptr
state::default_env() {
	iptr env	= nullptr;
	env = state::add_ffi(env, "lambda",  uint16(EVAL_ARGS), sint16(-1), make_closure);
	env = state::add_ffi(env, "define",  EVAL_ARGS,  2, symbol_define);
	env = add_ffi(env, "display", EVAL_ARGS,  1, display);
	env = add_ffi(env, "if",      0,          4, if_else);
	return env;
}

}	// namespace schizo
