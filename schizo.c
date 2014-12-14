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

/*******************************************************************************
** memory management
*******************************************************************************/
#define INITIAL_CELL_COUNT	2/* 128 * 1024 */	/* this should be at least 2 */

static void
free_cell(state_t* s,
	  cell_ptr_t ptr)
{
	cell_t*	p	= index_to_cell(s, ptr);
	switch( p->type ) {
	case ATOM_BOOL:
	case ATOM_CHAR:
	case ATOM_SINT64:
	case ATOM_REAL64:
	case CELL_FREE:
	case CELL_PAIR:
	case CELL_CLOSURE:
	case CELL_LAMBDA:
	case CELL_QUOTE:
	case CELL_FFI:
	case CELL_BIND:
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
	p->flags		= 0;
	p->object.pair.head	= NIL_CELL;
	p->object.pair.tail	= s->gc_block.free_list;
	s->gc_block.free_list	= ptr;
}



cell_ptr_t
cell_alloc(state_t* s) {
	/*
	** TODO: handle reallocation, calling garbage collector and the rest...
	** Unit testing is crucial: ensure all branches are visited.
	*/
	if( s->gc_block.cells == NULL ) {
		s->gc_block.count	= INITIAL_CELL_COUNT;
		s->gc_block.free_count	= INITIAL_CELL_COUNT - 2;
		s->gc_block.cells	= (cell_t*)malloc(sizeof(cell_t) * s->gc_block.count);
		memset(s->gc_block.cells, 0, sizeof(cell_t) * s->gc_block.count);

		for( uint32 i = 0; i < s->gc_block.count; ++i ) {
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
		/* call garbage collector before trigger a reallocation */
		uint32	old_count	= s->gc_block.count;

		/* only garbage collect if we finished parsing */
		uint32 freed_count	= s->root.index ? gc(s) : 0;

		if( freed_count == 0 ) {
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

			for( uint32 i = old_count; i < s->gc_block.count; ++i ) {
				s->gc_block.cells[i].type		= CELL_FREE;
				s->gc_block.cells[i].flags		= 0;
				s->gc_block.cells[i].object.pair.head	= NIL_CELL;
				s->gc_block.cells[i].object.pair.tail	= cell_ptr(i + 1);
			}

			s->gc_block.cells[s->gc_block.count - 1].object.pair.tail	= NIL_CELL;
		} else {
			fprintf(stderr, "freed %u\n", freed_count);
		}
	}

	cell_ptr_t	c = s->gc_block.free_list;

	s->gc_block.free_list	= list_tail(s, c);
	--(s->gc_block.free_count);
	return c;
}

static INLINE void
mark_cell(state_t* s, cell_ptr_t cell)
{
	if( !is_nil(cell) && !gc_is_reachable(s, cell) ) {

		cell_t*	c	= index_to_cell(s, cell);

		switch( c->type ) {
		case ATOM_SYMBOL:
		case ATOM_BOOL:
		case ATOM_CHAR:
		case ATOM_SINT64:
		case ATOM_REAL64:
		case ATOM_STRING:
		case CELL_FFI:
		case ATOM_ERROR:	/* ERRORS should point to another cell symbol/string ? */
			break;

		case CELL_BIND:
			mark_cell(s, c->object.bindings);
			break;

		case CELL_FREE:
		case CELL_PAIR: {
			mark_cell(s, c->object.pair.head);
			cell_ptr_t cell2	= list_tail(s, cell);
			while( !is_nil(cell2) && !gc_is_reachable(s, cell2) ) {
				gc_mark_reachable(s, cell2);
				mark_cell(s, index_to_cell(s, cell2)->object.pair.head);
				cell2	= list_tail(s, cell2);
			}

			break;
		}

		case CELL_CLOSURE:
			mark_cell(s, c->object.closure.env);
			mark_cell(s, c->object.closure.lambda);
			break;

		case CELL_LAMBDA:
			mark_cell(s, c->object.lambda.syms);
			mark_cell(s, c->object.lambda.body);
			break;

		case CELL_QUOTE:
			mark_cell(s, c->object.quote.list);
			break;
		default:
			assert(false);
		}
		gc_mark_reachable(s, cell);
	}
}

uint32
gc(state_t* s)
{
	uint32	nc	= 0;

	for( uint32 i = 0; i < s->gc_block.count; ++i ) {
		if( s->gc_block.cells[i].type != CELL_FREE ) {
			gc_mark_unreachable(s, cell_ptr(i));
		}
	}

	mark_cell(s, s->root);

	/* mark all registers/stack pointers as reachable */
	mark_cell(s, s->registers.current_env);
	mark_cell(s, s->registers.current_exp);
	mark_cell(s, s->registers.env_stack);

	for( uint32 i = 1; i < s->gc_block.count; ++i ) {
		if( !gc_is_reachable(s, cell_ptr(i)) && s->gc_block.cells[i].type != CELL_FREE ) {
			free_cell(s, cell_ptr(i));
			++nc;
		}
	}

	/* first cell, is the nil cell */
	s->gc_block.cells[0].type			= CELL_PAIR;
	s->gc_block.cells[0].object.pair.tail		= NIL_CELL;

	return nc;
}

/*******************************************************************************
** printing
*******************************************************************************/

static void
print_level(uint32 level) {
	for( uint32 i = 0; i < level; ++i ) {
		fprintf(stderr, "  ");
	}
}

void
print_cell(state_t* s,
	   cell_ptr_t cell,
	   uint32 level)
{
	cell_t* c	= index_to_cell(s, cell);
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

	//fprintf(stderr, "\n");
}

/*******************************************************************************
** atoms
*******************************************************************************/

#define IMPLEMENT_TYPE_CELL(TYPE, FIELD, ENUM)	cell_ptr_t \
	atom_new_ ## TYPE(state_t* s, TYPE v) { \
	cell_ptr_t r	= cell_alloc(s); \
	cell_t* ret	= index_to_cell(s, r); \
	ret->type	= ENUM; \
	ret->object.FIELD	= v; \
	return r; \
	}

cell_ptr_t
atom_new_symbol(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_ptr_t r	= cell_alloc(s);
	cell_t*	ret	= index_to_cell(s, r);

	ret->type	= ATOM_SYMBOL;
	ret->object.symbol	= (char*)(malloc(len + 1));
	memcpy(ret->object.symbol, b, len + 1);
	return r;
}

cell_ptr_t
atom_new_boolean(state_t* s,
		 bool b)
{
	cell_ptr_t r	= cell_alloc(s);
	cell_t*	ret	= index_to_cell(s, r);

	ret->type	= ATOM_BOOL;
	ret->object.boolean	= b;
	return r;
}

cell_ptr_t
atom_new_char(state_t* s,
	      char c)
{
	cell_ptr_t r	= cell_alloc(s);
	cell_t*	ret	= index_to_cell(s, r);

	ret->type	= ATOM_CHAR;
	ret->object.ch		= c;
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
	cell_t*	ret	= index_to_cell(s, r);

	ret->type	= ATOM_STRING;
	ret->object.string	= (char*)(malloc(len + 1));
	memcpy(ret->object.string, b, len + 1);
	return r;
}

cell_ptr_t
schizo_error(state_t* s,
	     const char* error)
{
	size_t len	= strlen(error);
	cell_ptr_t r	= cell_alloc(s);
	cell_t*	ret	= index_to_cell(s, r);

	ret->type	= ATOM_ERROR;
	ret->object.string	= (char*)(malloc(len + 1));
	memcpy(ret->object.string, error, len + 1);
	return r;
}

cell_ptr_t
cell_new_bind_list(state_t* s)
{
	cell_ptr_t r	= cell_alloc(s);
	cell_t*	ret	= index_to_cell(s, r);

	ret->type	= CELL_BIND;
	ret->object.bindings	= NIL_CELL;
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
list_new(state_t* s,
	 cell_ptr_t head)
{
	cell_ptr_t r	= cell_alloc(s);
	cell_t*	ret	= index_to_cell(s, r);

	ret->type	= CELL_PAIR;
	ret->object.pair.head	= head;
	ret->object.pair.tail	= NIL_CELL;
	return r;
}

cell_ptr_t
list_cons(state_t* s,
	  cell_ptr_t head,
	  cell_ptr_t tail)
{
	cell_ptr_t r	= list_new(s, head);
	cell_t* ret	= index_to_cell(s, r);

	assert( is_nil(tail) || cell_type(s, tail) == CELL_PAIR );

	ret->object.pair.tail	= tail;

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
list_reverse_in_place(state_t* s, cell_ptr_t list)
{
	if( !is_nil(list) ) {
		assert( cell_type(s, list) == CELL_PAIR );

		cell_ptr_t	curr	= list;
		cell_ptr_t	next	= list_tail(s, curr);
		while( !is_nil(next) ) {
			assert( cell_type(s, curr) == CELL_PAIR );

			cell_ptr_t	tmp	= list_tail(s, next);

			index_to_cell(s, next)->object.pair.tail	= curr;

			curr		= next;
			next		= tmp;
		}

		index_to_cell(s, list)->object.pair.tail	= NIL_CELL;
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
	while( !is_nil(list_head(s, l0)) && !is_nil(list_head(s, l1)) ) {
		res	= list_cons(s, list_make_pair(s, list_head(s, l0), list_head(s, l1)), res);
		l0	= list_tail(s, l0);
		l1	= list_tail(s, l1);
	}

	if( is_nil(l0) != is_nil(l1) ) {
		return schizo_error(s, "ERROR: couldn't zip the lists, one is longer than the other");
	} else {
		return list_reverse_in_place(s, res);
	}
}

/*******************************************************************************
** eval
*******************************************************************************/
static cell_ptr_t
symbol_lookup(state_t* s,
	      cell_ptr_t env,
	      const char* sym)
{
	cell_ptr_t	pair	= list_head(s, env);

	while( !is_nil(pair) && strcmp(sym, index_to_cell(s, list_head(s, pair))->object.symbol) != 0 ) {
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
	s->registers.env_stack = list_cons(s, s->registers.current_env, s->registers.env_stack);
}

static INLINE void
pop_env(state_t* s)
{
	s->registers.env_stack = list_tail(s, s->registers.env_stack);
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
	return list_reverse_in_place(s, res);
}

static INLINE void
bind(state_t* s,
     cell_ptr_t b)
{
	if( cell_type(s, b) == CELL_BIND ) {
		cell_ptr_t	sympair = index_to_cell(s, b)->object.bindings;
		while( !is_nil(sympair) ) {
			s->registers.current_env = list_cons(s, list_head(s, sympair), s->registers.current_env);
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

#define RETURN(EXP)	retval = EXP; pop_env(s); return retval

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
		case CELL_CLOSURE:
		case CELL_FFI:
		case CELL_QUOTE:
		case CELL_BIND:
			RETURN(exp);

		/* symbols */
		case ATOM_SYMBOL:
			RETURN(symbol_lookup(s, s->registers.current_env, index_to_cell(s, exp)->object.symbol));

		/* applications */
		case CELL_PAIR:	{
			cell_ptr_t	head	= eval(s, list_head(s, exp));

			if( is_nil(head) ) {
				RETURN(NIL_CELL);
			}

			cell_ptr_t	tail	= list_tail(s, exp);

			switch( cell_type(s, head) ) {
			case CELL_FFI:
				if( index_to_cell(s, head)->flags & EVAL_ARGS ) {
					uint32	l	= list_length(s, tail);
					if( index_to_cell(s, head)->object.ffi.arg_count > 0 && l != index_to_cell(s, head)->object.ffi.arg_count ) {
						fprintf(stderr, "ERROR: function requires %d arguments, only %d were given\n", l, index_to_cell(s, head)->object.ffi.arg_count);
						RETURN(NIL_CELL);
					} else {
						RETURN(index_to_cell(s, head)->object.ffi.proc(s, eval_list(s, tail)));
					}
				} else {	/* lambda, define, if */
					exp	= index_to_cell(s, head)->object.ffi.proc(s, tail);
				}
				break;
			case CELL_CLOSURE: {
				cell_ptr_t lambda	= index_to_cell(s, head)->object.closure.lambda;

				//environment_push(s);

				s->registers.current_env	= index_to_cell(s, head)->object.closure.env;

				/* evaluate the arguments and zip them */
				if( list_length(s, tail) != list_length(s, index_to_cell(s, lambda)->object.lambda.syms) ) {
					RETURN(schizo_error(s, "ERROR: closure arguments do not match given arguments"));
				}
				cell_ptr_t	args	= eval_list(s, tail);
				cell_ptr_t	syms	= index_to_cell(s, lambda)->object.lambda.syms;

				while( !is_nil(list_head(s, syms)) && !is_nil(list_head(s, args)) ) {
					s->registers.current_env	= list_cons(s, list_make_pair(s, list_head(s, syms), list_head(s, args)), s->registers.current_env);
					args	= list_tail(s, args);
					syms	= list_tail(s, syms);
				}

				if( is_nil(syms) != is_nil(args) ) {
					RETURN(schizo_error(s, "ERROR: couldn't zip the lists, one is longer than the other"));
				} else {
					/* all good, evaluate the body(*) */
					cell_ptr_t	body	= index_to_cell(s, lambda)->object.lambda.body;
					cell_ptr_t	next	= list_tail(s, body);

					exp	= list_head(s, body);

					while( !is_nil(next) ) {
						/* print_env(s); */
						bind(s, eval(s, exp));	/* eval and bind */

						exp	= list_head(s, next);
						next	= list_tail(s, next);
					}

					/* now the tail call (env and exp are set) */
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
	index_to_cell(s, ret)->object.bindings	= list_cons(s, pair, index_to_cell(s, ret)->object.bindings);
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

	cell_ptr_t	c	= cell_alloc(s);
	cell_ptr_t	l	= cell_alloc(s);
	cell_t*		closure	= index_to_cell(s, c);
	cell_t*		lambda	= index_to_cell(s, l);

	lambda->type	= CELL_LAMBDA;
	lambda->object.lambda.syms	= syms;
	lambda->object.lambda.body	= body;

	closure->type	= CELL_CLOSURE;
	closure->object.closure.lambda	= l;
	closure->object.closure.env	= s->registers.current_env;
	return c;
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

	if( cell_type(s, elsym) == ATOM_SYMBOL && strcmp(index_to_cell(s, elsym)->object.symbol, "else") == 0 ) {
		cell_ptr_t b	= eval(s, cond);
		if( cell_type(s, b) != ATOM_BOOL ) {
			return schizo_error(s, "ERROR: if requires condition to be boolean");
		}

		if( index_to_cell(s, b)->object.boolean ) {
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
	cell_ptr_t	_s	= atom_new_symbol(s, sym);
	cell_ptr_t	_f	= cell_alloc(s);
	cell_t*		f	= index_to_cell(s, _f);

	f->type		= CELL_FFI;
	f->flags	|= eval_args ? EVAL_ARGS : 0;
	f->object.ffi.arg_count	= arg_count;
	f->object.ffi.proc	= call;

	cell_ptr_t	p		= list_make_pair(s, _s, _f);
	s->registers.current_env	= list_cons(s, p, s->registers.current_env);
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
	if( s->gc_block.cells ) {
		/* release all cells individually */
		for( uint32 i = 1; i < s->gc_block.count; ++i ) {
			free_cell(s, cell_ptr(i));
		}

		free(s->gc_block.cells);
	}

	free(s);
}
