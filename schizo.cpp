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

using namespace schizo;

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
schizo::print_cell(cell::iptr c,
	   uint32 level)
{
	print_level(level);
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
		/*
	case ATOM_UNARY_OP:
		fprintf(stderr, "%s", c->object.op);
		break;

	case ATOM_BINARY_OP:
		fprintf(stderr, "%s", c->object.op);
		break;
*/
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
		if( !list::head(c) ) {
			fprintf(stderr, "(");
			print_cell(list::head(c), uint32(0));
		} else {
			fprintf(stderr, "(nil");
		}

		if( !list::tail(c) ) {
			cell::iptr n	= list::tail(c);
			while( !n ) {
				if( !list::head(n) ) {
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
}


/*******************************************************************************
** lists
*******************************************************************************/
uint32
list::length(cell::iptr l)
{
	cell::iptr	curr	= l;
	uint32		len	= 0;

	while( !curr ) {
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

	while( !l ) {
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
static cell_ptr_t
symbol_lookup(state_t* s,
	      const char* sym)
{
	cell_ptr_t	env	= list_head(s, s->registers.env_stack);
	cell_ptr_t	pair	= list_head(s, env);

	while( !is_nil(pair) && strcmp(sym, ITC(list_head(s, pair))->object.symbol) != 0 ) {
		env	= list_tail(s, env);
		if( !is_nil(env) ) {
			pair	= list_head(s, env);
		} else {
			pair	= NIL_CELL;
		}
	}

	if( is_nil(pair) ) {
		return NIL_CELL;
	} else {
		if( !is_nil(list_tail(s, pair)) ) {
			return list_head(s, list_tail(s, pair));
		} else {
			return NIL_CELL;
		}
	}
}

static INLINE void
push_env(state_t* s,
	 cell_ptr_t env)
{
	set_cell(s->registers.env_stack, list_cons(s, env, s->registers.env_stack));
}

static INLINE void
pop_env(state_t* s)
{
	set_cell(s->registers.env_stack, list_tail(s, s->registers.env_stack));
}

static INLINE void
push_exp(state_t* s,
	 cell_ptr_t exp)
{
	set_cell(s->registers.exp_stack, list_cons(s, exp, s->registers.exp_stack));
}

static INLINE void
pop_exp(state_t* s)
{
	set_cell(s->registers.exp_stack, list_tail(s, s->registers.exp_stack));
}

static cell_ptr_t
eval_list(state_t* s,
	  cell_ptr_t expr)
{
	cell_ptr_t	res	= NIL_CELL;
	while( !is_nil(expr) ) {
		res	= list_cons(s, eval(s, list_head(s, expr)), res);
		expr	= list_tail(s, expr);
	}
	return list_reverse(s, res);
}

static INLINE void
op_bind(state_t* s,
	cell_ptr_t b)
{
	cell_ptr_t	env	= list_head(s, s->registers.env_stack);

	if( cell_type(s, b) == OP_BIND ) {
		cell_ptr_t	sympair = ITC(b)->object.bindings;
		while( !is_nil(sympair) ) {
			set_cell(env, list_cons(s, list_head(s, sympair), env));
			sympair	= list_tail(s, sympair);
		}
	}

	ITC(s->registers.env_stack)->object.pair.head = env;
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

#define RETURN(EXP)	set_cell(s->registers.ret_val, EXP); set_cell(exp, NIL_CELL);

cell_ptr_t
eval(state_t* s)
{

	while( !is_nil(exp) ) {	/* not a NIL_CELL */
		/* print_env(s); */
		switch( cell_type(s, exp) ) {
		/* constants */
		case ATOM_BOOL:
		case ATOM_CHAR:
		case ATOM_SINT64:
		case ATOM_REAL64:
		case ATOM_STRING:
		case ATOM_ERROR:
		case OP_APPLY_CLOSURE:
		case OP_APPLY_FFI:
		case CELL_QUOTE:
		case OP_BIND:
			RETURN(exp);

		/* symbols */
		case ATOM_SYMBOL:
			fprintf(stderr, "SYMBOL LOOKUP: %s\n", ITC(exp)->object.symbol);
			RETURN(symbol_lookup(s, s->registers.current_env, ITC(exp)->object.symbol));

		/* applications */
		case CELL_PAIR:	{
			cell_ptr_t	head	= NIL_CELL;
			cell_ptr_t	tail	= NIL_CELL;

			set_cell(head, list_head(s, exp));	/* NOT NEEDED ? */
			grab(head);
			set_cell(head, eval(s, head));
			release(head);

			if( is_nil(head) ) {
				set_cell(head, NIL_CELL);
				RETURN(NIL_CELL);
			}

			set_cell(tail, list_tail(s, exp));	/* NOT NEEDED ? */

			switch( cell_type(s, head) ) {
			case OP_FFI:
				if( ITC(head)->object.ffi.flags & EVAL_ARGS ) {
					/* TODO: this is limited to 32768 elements! need to increase the range */
					uint32	l	= list_length(s, tail);
					assert( l < 0x7FFF );
					if( ITC(head)->object.ffi.arg_count > 0 && (sint16)l != ITC(head)->object.ffi.arg_count ) {
						fprintf(stderr, "ERROR: function requires %d arguments, only %d were given\n", l, ITC(head)->object.ffi.arg_count);
						set_cell(tail, NIL_CELL);
						set_cell(head, NIL_CELL);
						RETURN(NIL_CELL);
					} else {
						cell_ptr_t	r	= NIL_CELL;

						set_cell(s->registers.current_exp, eval_list(s, tail));

						push_exp(s);
						ITC(head)->object.ffi.proc(s);
						set_cell(r, s->registers.ret_val);
						pop_exp(s);

						set_cell(tail, NIL_CELL);
						set_cell(head, NIL_CELL);
						RETURN(r);
					}
				} else {	/* lambda, define, if */
					set_cell(exp, ITC(head)->object.ffi.proc(s, tail));
					set_cell(tail, NIL_CELL);
					set_cell(head, NIL_CELL);
				}
				break;
			case OP_CLOSURE: {
				cell_ptr_t	args	= NIL_CELL;
				cell_ptr_t	syms	= NIL_CELL;
				cell_ptr_t	lambda	= NIL_CELL;

				/*print_cell(s, tail, 0);*/

				set_cell(lambda, ITC(head)->object.closure.lambda);
				if( s->registers.current_env.index != ITC(head)->object.closure.env.index ) {
					set_cell(s->registers.current_env, ITC(head)->object.closure.env);
				}

				/* evaluate the arguments and zip them */
				if( list_length(s, tail) != list_length(s, ITC(lambda)->object.lambda.syms) ) {
					RETURN(schizo_error(s, "ERROR: closure arguments do not match given arguments"));
				}

				set_cell(args, eval_list(s, tail));
				set_cell(syms, ITC(lambda)->object.lambda.syms);

				while( !is_nil(list_head(s, syms)) && !is_nil(list_head(s, args)) ) {
					set_cell(s->registers.current_env, list_cons(s, list_make_pair(s, list_head(s, syms), list_head(s, args)), s->registers.current_env));
					set_cell(args, list_tail(s, args));
					set_cell(syms, list_tail(s, syms));
				}

				if( is_nil(syms) != is_nil(args) ) {
					set_cell(syms, NIL_CELL);
					set_cell(args, NIL_CELL);

					set_cell(tail, NIL_CELL);
					set_cell(head, NIL_CELL);

					RETURN(schizo_error(s, "ERROR: couldn't zip the lists, one is longer than the other"));
				} else {
					/* all good, evaluate the body(*) */
					cell_ptr_t	body	= NIL_CELL;
					cell_ptr_t	next	= NIL_CELL;

					set_cell(body, ITC(lambda)->object.lambda.body);
					set_cell(next, list_tail(s, body));

					set_cell(exp, list_head(s, body));

					while( !is_nil(next) ) {
						/* print_env(s); */
						bind(s, eval(s, exp));	/* eval and bind */

						set_cell(exp, list_head(s, next));
						set_cell(next, list_tail(s, next));
					}

					/* now the tail call (env and exp are set) */
					set_cell(body, NIL_CELL);
					set_cell(next, NIL_CELL);

					set_cell(syms, NIL_CELL);
					set_cell(args, NIL_CELL);

					set_cell(tail, NIL_CELL);
					set_cell(head, NIL_CELL);
				}
				break;
			}

			default:
				/* more error handling */
				fprintf(stderr, "Unexpected type: %u\n", cell_type(s, head));
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
	RETURN(NIL_CELL);
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
static cell_ptr_t
symbol_define(state_t* s,
	      cell_ptr_t args)
{
	cell_ptr_t sym	= list_head(s, args);
	cell_ptr_t body	= list_head(s, list_tail(s, args));

	cell_ptr_t pair	= list_make_pair(s, sym, body);

	cell_ptr_t ret	= cell_new_bind_list(s);
	set_cell(ITC(ret)->object.bindings, list_cons(s, pair, ITC(ret)->object.bindings));

	return ret;
}

/**
 * @brief make_closure
 * @param s
 * @param env
 * @param args
 * @return
 */
static cell_ptr_t
make_closure(state_t* s,
	     cell_ptr_t args)
{
	cell_ptr_t	syms	= list_head(s, args);
	cell_ptr_t	body	= list_tail(s, args);

	cell_ptr_t	closure	= cell_alloc(s);

	cell_ptr_t	lambda	= cell_alloc(s);


	ITC(lambda)->type	= CELL_LAMBDA;
	set_cell(ITC(lambda)->object.lambda.syms, syms);
	set_cell(ITC(lambda)->object.lambda.body, body);

	ITC(closure)->type	= OP_CLOSURE;
	set_cell(ITC(closure)->object.closure.lambda, lambda);
	set_cell(ITC(closure)->object.closure.env, s->registers.current_env);
	return closure;
}

static cell_ptr_t
if_else(state_t* s,
	cell_ptr_t args)
{
	cell_ptr_t	cond	= NIL_CELL;
	cell_ptr_t	exp0	= NIL_CELL;
	cell_ptr_t	elsym	= NIL_CELL;
	cell_ptr_t	exp1	= NIL_CELL;

	if( list_length(s, args) != 4 ) {
		return schizo_error(s, "ERROR in \"if\" usage: if cond exp0 else exp1");
	}

	cond	= list_head(s, args);
	exp0	= list_head(s, list_tail(s, args));
	elsym	= list_head(s, list_tail(s, list_tail(s, args)));
	exp1	= list_head(s, list_tail(s, list_tail(s, list_tail(s, args))));

	if( cell_type(s, elsym) == ATOM_SYMBOL && strcmp(ITC(elsym)->object.symbol, "else") == 0 ) {
		cell_ptr_t b	= eval(s, cond);
		if( cell_type(s, b) != ATOM_BOOL ) {
			return schizo_error(s, "ERROR: if requires condition to be boolean");
		}

		if( ITC(b)->object.boolean ) {
			return exp0;
		} else {
			return exp1;
		}
	} else {
		return schizo_error(s, "ERROR: if requires \"else\" keyword: if cond exp0 else exp1");
	}
}

static cell_ptr_t
display(state_t* s,
	cell_ptr_t args)
{
	print_cell(s, args, 0);
	return NIL_CELL;
}

/*******************************************************************************
** schizo state
*******************************************************************************/
void
state_add_ffi(state_t *s,
	      bool eval_args,
	      const char* sym,
	      ffi_call_t call,
	      sint32 arg_count)
{
	cell_ptr_t	p	= NIL_CELL;
	cell_ptr_t	_s	= atom_new_symbol(s, sym);
	cell_ptr_t	f	= cell_alloc(s);

	ITC(f)->type	= OP_FFI;
	ITC(f)->object.ffi.flags	|= eval_args ? EVAL_ARGS : 0;
	ITC(f)->object.ffi.arg_count	= arg_count;
	ITC(f)->object.ffi.proc	= call;

	p		= list_make_pair(s, _s, f);
	set_cell(s->registers.current_env, list_cons(s, p, s->registers.current_env));
}

state_t*
state_new()
{
	state_t*	state	= (state_t*)malloc(sizeof(state_t));
	memset(state, 0, sizeof(state_t));

	state_add_ffi(state, false, "lambda",  make_closure, -1);
	state_add_ffi(state, false, "define",  symbol_define, 2);
	state_add_ffi(state, true,  "display", display,       1);
	state_add_ffi(state, false, "if",      if_else,       4);

	return state;
}

void
state_release(state_t *s)
{
	uint32	i = 0;
	if( s->gc_block.cells ) {
		/* release all cells individually */
		set_cell(s->parser.root, NIL_CELL);
		set_cell(s->registers.current_env, NIL_CELL);
		set_cell(s->registers.current_exp, NIL_CELL);
		set_cell(s->registers.env_stack, NIL_CELL);

		assert( s->gc_block.free_count = s->gc_block.count );

		free(s->gc_block.cells);
	}

	free(s);
}
