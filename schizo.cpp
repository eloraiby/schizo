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
state::lookup(const char* sym)
{
	iptr	env	= list::head(registers_.env_stack);
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
}

cell::iptr
state::eval_list(cell::iptr expr) {
	cell::iptr	res	= nullptr;
	while( expr ) {
		res	= new list(eval(list::head(expr)), res);
		expr	= list::tail(expr);
	}
	return list::reverse(res);
}

cell::iptr
state::apply_bind(cell::iptr b)
{
	iptr	env	= top_env();

	if( b->type() == CELL_BIND ) {
		iptr	sympair = static_cast<bind*>(b.get())->bindings();
		while( sympair ) {
			env = new list(list::head(sympair), env);
			sympair	= list::tail(sympair);
		}
	}

	registers_.env_stack	= new list(env, list::tail(registers_.env_stack));
	return nullptr;
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
state::eval(cell::iptr exp) {
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
			return apply_bind(exp);

			/* symbols */
		case ATOM_SYMBOL:
			fprintf(stderr, "SYMBOL LOOKUP: %s\n", static_cast<symbol*>(exp.get())->value());
			return lookup(static_cast<symbol*>(exp.get())->value());

			/* applications */
		case CELL_LIST:	{
			iptr	head	= list::head(exp);
			iptr	tail	= nullptr;

			if( !head ) {
				return nullptr;
			}

			tail	= list::tail(exp);	/* NOT NEEDED ? */

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

						iptr args	= eval_list(tail);

						return (*static_cast<ffi*>(head.get()))(this, registers_.current_env, args);
					}
				} else {	/* lambda, define, if */
					exp	= (*static_cast<ffi*>(head.get()))(this, registers_.current_env, tail);
				}
				break;
			case CELL_CLOSURE: {
				iptr	args	= nullptr;
				iptr	syms	= nullptr;
				iptr	lambda_	= nullptr;

				/*print_cell(s, tail, 0);*/

				lambda_	= static_cast<closure*>(head.get())->lambda();
				registers_.current_env	= static_cast<closure*>(head.get())->env();

				/* evaluate the arguments and zip them */
				if( list::length(tail) != list::length(static_cast<lambda*>(lambda_.get())->syms()) ) {
					return new error("ERROR: closure arguments do not match given arguments");
				}

				args	= eval_list(tail);
				syms	= static_cast<lambda*>(lambda_.get())->syms();

				while( list::head(syms) && list::head(args) ) {
					registers_.current_env	= new list(pair(list::head(syms), list::head(args)), registers_.current_env);
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

					while( next ) {
						/* print_env(s); */
						ret	= eval(exp);	/* eval and bind */

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
symbol_define(state* s,
	      cell::iptr env,
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
make_closure(state* s,
	     cell::iptr env,
	     cell::iptr args)
{
	cell::iptr	syms	= list::head(args);
	cell::iptr	body	= list::tail(args);

	cell::iptr	lam	= new lambda(syms, body);

	return new closure(env, lam);
}

static cell::iptr
if_else(state* s,
	cell::iptr env,
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
		cell::iptr b	= s->eval(cond);
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
display(state* s,
	cell::iptr env,
	cell::iptr args)
{
	print_cell(args, 0);
	return nullptr;
}

/*******************************************************************************
** schizo state
*******************************************************************************/
void
state::add_ffi(const char* sym,
	       uint16 flags,
	       sint32 arg_count,
	       ffi_call_t proc)
{
	cell::iptr	p	= nullptr;
	cell::iptr	_s	= new symbol(sym);

	cell::iptr	f	= new ffi(flags, arg_count, proc);

	registers_.current_env	= new list(pair(_s, f), registers_.current_env);
}

state::state() : cell(CELL_STATE) {
	parser_.token_start	= nullptr;
	parser_.token_end	= nullptr;
	parser_.token_line	= 0;

	add_ffi("lambda",  EVAL_ARGS, -1, make_closure);
	add_ffi("define",  EVAL_ARGS,  2, symbol_define);
	add_ffi("display", EVAL_ARGS,  1, display);
	add_ffi("if",      0,          4, if_else);
}

state::~state() {
}
}	// namespace schizo
