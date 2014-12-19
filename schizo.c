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

/* little helpers */
#define ITC(A)			index_to_cell(s, A)

/*******************************************************************************
** memory management
*******************************************************************************/
#define INITIAL_CELL_COUNT	2/* 128 * 1024 */	/* this should be at least 2 */

void
free_cell(state_t* s,
	  cell_ptr_t ptr)
{
	cell_t*	p	= ITC(ptr);

	assert( p->ref_count == 0 );

	switch( p->type ) {
	case ATOM_BOOL:
	case ATOM_CHAR:
	case ATOM_SINT64:
	case ATOM_REAL64:
	case OP_FFI:
		break;

	case CELL_FREE:
		assert( 0 && "freeing a FREE_CELL!!!");
		break;

	case CELL_PAIR:
		release(p->object.pair.head);

		/* TODO: prevent recursive call and stack overflow from successive list calls */
		release(p->object.pair.tail);
		break;

	case OP_CLOSURE:
		release(p->object.closure.env);
		release(p->object.closure.lambda);
		break;

	case CELL_LAMBDA:
		release(p->object.lambda.syms);
		release(p->object.lambda.body);
		break;

	case CELL_QUOTE:
		release(p->object.quote.list);
		break;

	case OP_BIND:
		release(p->object.bindings);
		break;

	case ATOM_STRING:
		free(p->object.string);
		break;

	case ATOM_SYMBOL:
		free(p->object.symbol);
		break;

	case ATOM_ERROR:	/* ERRORS should point to another cell symbol/string ? */
		free(p->object.string);
		break;
	default:
		assert(false);
	}

	/* append the free cell list */
	p->type			= CELL_FREE;
	p->ref_count		= 0;
	p->object.pair.head	= NIL_CELL;
	p->object.pair.tail	= s->gc_block.free_list;
	s->gc_block.free_list	= ptr;
}



cell_ptr_t
cell_alloc(state_t* s) {
	uint32		i = 0;
	cell_ptr_t	c = NIL_CELL;

	/*
	** TODO: handle reallocation, calling garbage collector and the rest...
	** Unit testing is crucial: ensure all branches are visited.
	*/
	if( s->gc_block.cells == NULL ) {
		s->gc_block.count	= INITIAL_CELL_COUNT;
		s->gc_block.free_count	= INITIAL_CELL_COUNT - 2;
		s->gc_block.cells	= (cell_t*)malloc(sizeof(cell_t) * s->gc_block.count);
		memset(s->gc_block.cells, 0, sizeof(cell_t) * s->gc_block.count);

		for( i = 0; i < s->gc_block.count; ++i ) {
			s->gc_block.cells[i].type		= CELL_FREE;
			s->gc_block.cells[i].object.pair.head	= NIL_CELL;
			s->gc_block.cells[i].object.pair.tail	= cell_ptr(i + 1);
		}

		s->gc_block.cells[INITIAL_CELL_COUNT - 1].object.pair.tail	= NIL_CELL;

		/* first cell, is the nil cell */
		s->gc_block.cells[0].type			= CELL_PAIR;
		s->gc_block.cells[0].object.pair.tail		= NIL_CELL;

		s->gc_block.free_list	= cell_ptr(1);	/* free_list is the allocated lists */

	} else if( is_nil(s->gc_block.free_list) ) {
		uint32	old_count	= s->gc_block.count;

		cell_t* new_block	= (cell_t*)malloc(sizeof(cell_t) * old_count * 2);
		if( new_block == NULL ) {
			fprintf(stderr, "Fatal error: Not enough memory...\n");
			exit(1);
		}

		memset(new_block, 0, sizeof(cell_t) * old_count * 2);
		memcpy(new_block, s->gc_block.cells, sizeof(cell_t) * old_count);
		free(s->gc_block.cells);

		s->gc_block.free_count	= s->gc_block.count;
		s->gc_block.count	*= 2;
		s->gc_block.cells	= new_block;

		s->gc_block.free_list	= cell_ptr(old_count);

		for( i = old_count; i < s->gc_block.count; ++i ) {
			s->gc_block.cells[i].type		= CELL_FREE;
			s->gc_block.cells[i].object.pair.head	= NIL_CELL;
			s->gc_block.cells[i].object.pair.tail	= cell_ptr(i + 1);
		}

		s->gc_block.cells[s->gc_block.count - 1].object.pair.tail	= NIL_CELL;
	}

	c	= s->gc_block.free_list;

	assert( ITC(c)->ref_count == 0 );

	s->gc_block.free_list	= list_tail(s, c);
	--(s->gc_block.free_count);
	/* set both to NIL_CELL so that set_cell usage is consistent */
	ITC(c)->object.pair.head	= NIL_CELL;
	ITC(c)->object.pair.tail	= NIL_CELL;

	return c;
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
print_cell(state_t* s,
	   cell_ptr_t cell,
	   uint32 level)
{
	cell_t* c	= ITC(cell);
	print_level(level);
	switch( c->type ) {
	case ATOM_BOOL:
		if( c->object.boolean ) {
			fprintf(stderr, "#t");
		} else {
			fprintf(stderr, "#f");
		}
		break;

	case ATOM_SYMBOL:
		fprintf(stderr, "%s", c->object.symbol);
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
		fprintf(stderr, "'%c'", c->object.ch);
		break;

	case ATOM_SINT64:
		fprintf(stderr, "%ld", (sint64)c->object.s64);
		break;

	case ATOM_REAL64:
		fprintf(stderr, "%lf", (real64)c->object.r64);
		break;

	case ATOM_STRING:
		fprintf(stderr, "\"%s\"", c->object.string);
		break;

	case CELL_PAIR:
		if( !is_nil(c->object.pair.head) ) {
			fprintf(stderr, "(");
			print_cell(s, c->object.pair.head, 0);
		} else {
			fprintf(stderr, "(nil");
		}

		if( !is_nil(c->object.pair.tail) ) {
			cell_ptr_t n	= c->object.pair.tail;
			while( !is_nil(n) ) {
				if( !is_nil(list_head(s, n)) ) {
					print_cell(s, list_head(s, n), 1);
				} else {
					print_level(level + 1);
					fprintf(stderr, "nil");
				}

				n	= list_tail(s, n);
			}
		}
		fprintf(stderr, ")");
		break;
	default:
		break;
	}
}

/*******************************************************************************
** atoms
*******************************************************************************/

#define IMPLEMENT_TYPE_CELL(TYPE, FIELD, ENUM)	cell_ptr_t \
	atom_new_ ## TYPE(state_t* s, TYPE v) { \
	cell_ptr_t r	= cell_alloc(s); \
	ITC(r)->type	= ENUM; \
	ITC(r)->object.FIELD	= v; \
	return r; \
	}

cell_ptr_t
atom_new_symbol(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_ptr_t r	= cell_alloc(s);

	ITC(r)->type	= ATOM_SYMBOL;
	ITC(r)->object.symbol	= (char*)(malloc(len + 1));
	memcpy(ITC(r)->object.symbol, b, len + 1);
	return r;
}

cell_ptr_t
atom_new_boolean(state_t* s,
		 bool b)
{
	cell_ptr_t r	= cell_alloc(s);

	ITC(r)->type	= ATOM_BOOL;
	ITC(r)->object.boolean	= b;
	return r;
}

cell_ptr_t
atom_new_char(state_t* s,
	      char c)
{
	cell_ptr_t r	= cell_alloc(s);

	ITC(r)->type	= ATOM_CHAR;
	ITC(r)->object.ch	= c;
	return r;
}

IMPLEMENT_TYPE_CELL(sint64, s64, ATOM_SINT64)
IMPLEMENT_TYPE_CELL(real64, r64, ATOM_REAL64)

cell_ptr_t
atom_new_string(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_ptr_t r	= cell_alloc(s);

	ITC(r)->type	= ATOM_STRING;
	ITC(r)->object.string	= (char*)(malloc(len + 1));
	memcpy(ITC(r)->object.string, b, len + 1);
	return r;
}

cell_ptr_t
schizo_error(state_t* s,
	     const char* error)
{
	size_t len	= strlen(error);
	cell_ptr_t r	= cell_alloc(s);

	ITC(r)->type	= ATOM_ERROR;
	ITC(r)->object.string	= (char*)(malloc(len + 1));
	memcpy(ITC(r)->object.string, error, len + 1);
	return r;
}

cell_ptr_t
cell_new_bind_list(state_t* s)
{
	cell_ptr_t r	= cell_alloc(s);

	ITC(r)->type	= OP_BIND;
	ITC(r)->object.bindings	= NIL_CELL;
	return r;
}

/*
cell_ptr_t
atom_new_unary_op(state_t* s,
		  const char* op)
{
	size_t len	= strlen(op);
	cell_ptr_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];

	ret->type	= ATOM_UNARY_OP;
	ret->object.op	= (char*)(malloc(len + 1));
	memcpy(ret->object.symbol, op, len + 1);
	return id;

}

cell_ptr_t
atom_new_binary_op(state_t* s,
		   const char* op)
{
	size_t len	= strlen(op);
	cell_ptr_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];

	ret->type	= ATOM_BINARY_OP;
	ret->object.op	= (char*)(malloc(len + 1));
	memcpy(ret->object.symbol, op, len + 1);
	return id;

}
*/

/*******************************************************************************
** lists
*******************************************************************************/

cell_ptr_t
list_cons(state_t* s,
	  cell_ptr_t head,
	  cell_ptr_t tail)
{
	cell_ptr_t r	= cell_alloc(s);

	grab(head);
	grab(tail);

	ITC(r)->type	= CELL_PAIR;
	ITC(r)->object.pair.head	= head;
	ITC(r)->object.pair.tail	= NIL_CELL;

	assert( is_nil(tail) || cell_type(s, tail) == CELL_PAIR );

	ITC(r)->object.pair.tail	= tail;

	return r;
}

uint32
list_length(state_t* s, cell_ptr_t list)
{
	cell_ptr_t	curr	= list;
	uint32		len	= 0;

	while( !is_nil(curr) ) {
		assert( cell_type(s, list) == CELL_PAIR );
		++len;
		curr	= list_tail(s, curr);
	}
	return len;
}

cell_ptr_t
list_reverse(state_t* s,
	     cell_ptr_t list)
{
	cell_ptr_t	nl	= NIL_CELL;

	while( !is_nil(list) ) {
		nl	= list_cons(s, list_head(s, list), nl);
		list	= list_tail(s, list);
	}

	return nl;
}

/*
cell_ptr_t
list_reverse_in_place(state_t* s, cell_ptr_t list)
{
	if( !is_nil(list) ) {
		cell_ptr_t	curr	= NIL_CELL;
		cell_ptr_t	next	= NIL_CELL;

		assert( cell_type(s, list) == CELL_PAIR );

		curr	= list;
		next	= list_tail(s, curr);
		while( !is_nil(next) ) {
			cell_ptr_t	tmp	= NIL_CELL;

			assert( cell_type(s, curr) == CELL_PAIR );

			tmp		= list_tail(s, next);

			ITC(next)->object.pair.tail	= curr;

			curr		= next;
			next		= tmp;
		}

		ITC(list)->object.pair.tail	= NIL_CELL;

		assert( ITC(curr)->ref_count >= 1 );
		assert( ITC(list)->ref_count >= 1 );
		return curr;
	} else {
		return list;
	}
}

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
	      cell_ptr_t env,
	      const char* sym)
{
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
push_env(state_t* s)
{
	set_cell(s->registers.env_stack, list_cons(s, s->registers.current_env, s->registers.env_stack));
}

static INLINE void
pop_env(state_t* s)
{
	set_cell(s->registers.env_stack, list_tail(s, s->registers.env_stack));
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
bind(state_t* s,
     cell_ptr_t b)
{
	if( cell_type(s, b) == OP_BIND ) {
		cell_ptr_t	sympair = ITC(b)->object.bindings;
		while( !is_nil(sympair) ) {
			set_cell(s->registers.current_env, list_cons(s, list_head(s, sympair), s->registers.current_env));
			sympair	= list_tail(s, sympair);
		}
	}
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

#define RETURN(EXP)	retval = EXP; pop_env(s); set_cell(exp, NIL_CELL); return retval

cell_ptr_t
eval(state_t *s,
     cell_ptr_t exp)
{
	cell_ptr_t	retval	= NIL_CELL;

	push_env(s);	/* save the environment before doing anything */

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
		case OP_CLOSURE:
		case OP_FFI:
		case CELL_QUOTE:
		case OP_BIND:
			RETURN(exp);

		/* symbols */
		case ATOM_SYMBOL:
			/*fprintf(stderr, "SYMBOL LOOKUP: %s\n", ITC(exp)->object.symbol);*/
			RETURN(symbol_lookup(s, s->registers.current_env, ITC(exp)->object.symbol));

		/* applications */
		case CELL_PAIR:	{
			cell_ptr_t	head	= NIL_CELL;
			cell_ptr_t	tail	= NIL_CELL;

			__inc_ref_count(s, list_head(s, exp));	/* NOT NEEDED ? */
			set_cell(head, eval(s, list_head(s, exp)));
			__dec_ref_count(s, list_head(s, exp));

			if( is_nil(head) ) {
				set_cell(head, NIL_CELL);
				RETURN(NIL_CELL);
			}

			__inc_ref_count(s, list_tail(s, exp));	/* NOT NEEDED ? */
			set_cell(tail, list_tail(s, exp));
			__dec_ref_count(s, list_tail(s, exp));

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
						cell_ptr_t	t	= NIL_CELL;
						cell_ptr_t	r	= NIL_CELL;
						set_cell(t, eval_list(s, tail));
						r	= ITC(head)->object.ffi.proc(s, t);

						set_cell(t, NIL_CELL);
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
		set_cell(s->root, NIL_CELL);
		set_cell(s->registers.current_env, NIL_CELL);
		set_cell(s->registers.current_exp, NIL_CELL);
		set_cell(s->registers.env_stack, NIL_CELL);

		assert( s->gc_block.free_count = s->gc_block.count );

		free(s->gc_block.cells);
	}

	free(s);
}
