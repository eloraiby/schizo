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
#define INITIAL_CELL_COUNT	128 * 1024	/* this should be at least 2 */

static void
free_cell(state_t* s,
	  cell_ptr_t ptr)
{
	switch( ptr->type ) {
	case ATOM_BOOL:
	case ATOM_CHAR:
	case ATOM_SINT64:
	case ATOM_REAL64:
	case CELL_FFI:
		break;
	case ATOM_STRING:
		free(ptr->object.string);
		break;
	case ATOM_SYMBOL:
		free(ptr->object.symbol);
		break;

	case ATOM_ERROR:	/* ERRORS should point to another cell symbol/string ? */
		free(ptr->object.string);
		break;

	case CELL_FREE:
		break;
		
	case CELL_PAIR:
		break;

	case CELL_CLOSURE:
		break;

	case CELL_LAMBDA:
		break;

	case CELL_QUOTE:
		break;
	}

	/* append the free cell list */
	ptr->type		= CELL_FREE;
	ptr->ref_count		= 0;
	ptr->flags		= 0;
	ptr->object.pair.head	= NIL_CELL;
	ptr->object.pair.tail	= s->gc_block.free_list;
	s->gc_block.free_list	= ptr;
}

static INLINE cell_ptr_t
remap_cell_ptr(cell_ptr_t old_base,
	       uint32 old_size,
	       cell_ptr_t new_base,
	       cell_ptr_t ptr)
{
	if( ptr >= old_base && ptr < old_base + old_size ) {
		return (ptr - old_base) + new_base;
	} else {
		return ptr;
	}
}

static void
remap_cell(cell_ptr_t old_base,
	   uint32 old_size,
	   cell_ptr_t new_base,
	   cell_ptr_t ptr)
{
#define REMAP(a)	new_ptr->object.a = remap_cell_ptr(old_base, old_size, new_base, ptr->object.a)

	cell_ptr_t new_ptr	= remap_cell_ptr(old_base, old_size, new_base, ptr);
	*new_ptr	= *ptr;

	switch( ptr->type ) {
	case ATOM_SYMBOL:
	case ATOM_BOOL:
	case ATOM_CHAR:
	case ATOM_SINT64:
	case ATOM_REAL64:
	case ATOM_STRING:
	case CELL_FFI:
	case ATOM_ERROR:	/* ERRORS should point to another cell symbol/string ? */
		break;

	case CELL_FREE:
	case CELL_PAIR:
		REMAP(pair.head);
		REMAP(pair.tail);
		break;

	case CELL_CLOSURE:
		REMAP(closure.env);
		REMAP(closure.lambda);
		break;

	case CELL_LAMBDA:
		REMAP(lambda.syms);
		REMAP(lambda.body);
		break;

	case CELL_QUOTE:
		REMAP(quote.list);
		break;
	}

#undef REMAP
}

/**
 * @brief remap remap old allocation block to the new one
 * @param old_block old block address
 * @param old_size old block size (in cells)
 * @param new_block new block address
 * @param new_size new bloack size (in cells)
 * @return
 */
static INLINE void
remap(cell_ptr_t old_block,
      uint32 old_size,
      cell_ptr_t new_block)
{
	for( uint32 i = 0; i < old_size; ++i ) {
		remap_cell(old_block, old_size, new_block, &old_block[i]);
	}
}

cell_ptr_t
cell_alloc(state_t* s) {
	/*
	** TODO: handle reallocation, calling garbage collector and the rest...
	** Unit testing is crucial: ensure all branches are visited.
	*/
	if( s->gc_block.cells == NULL ) {
		s->gc_block.count	= INITIAL_CELL_COUNT;
		s->gc_block.free_count	= INITIAL_CELL_COUNT - 1;
		s->gc_block.cells	= (cell_ptr_t)malloc(sizeof(cell_t) * s->gc_block.count);
		memset(s->gc_block.cells, 0, sizeof(cell_t) * s->gc_block.free_count);

		for( uint32 i = 0; i < s->gc_block.count; ++i ) {
			s->gc_block.cells[i].type		= CELL_FREE;
			s->gc_block.cells[i].object.pair.head	= NIL_CELL;
			s->gc_block.cells[i].object.pair.tail	= &(s->gc_block.cells[i + 1]);
		}

		s->gc_block.cells[INITIAL_CELL_COUNT - 1].object.pair.tail	= NIL_CELL;

		/* first cell, is the nil cell */
		s->gc_block.cells[0].type	= CELL_PAIR;
		s->gc_block.free_list	= s->gc_block.cells;	/* free_list is the allocated lists */

	} else if( s->gc_block.free_list == NIL_CELL ) {

		/* call garbage collector before trigger a reallocation */
		uint32	old_count	= s->gc_block.count;
		cell_ptr_t old_block	= s->gc_block.cells;

		/* only garbage collect if we finished parsing */
		uint32 freed_count	= s->root ? gc(s) : 0;

		if( freed_count == 0 ) {
			s->gc_block.free_count	= s->gc_block.count;
			s->gc_block.count	*= 2;
			s->gc_block.cells	= (cell_ptr_t)realloc(s->gc_block.cells, sizeof(cell_t) * s->gc_block.count);

			if( s->gc_block.cells == NULL ) {
				fprintf(stderr, "Fatal error: Not enough memory...\n");
				exit(1);
			}

			remap(old_block, old_count, s->gc_block.cells);

			s->gc_block.free_list	= &(s->gc_block.cells[old_count]);

			for( uint32 i = old_count; i < s->gc_block.count; ++i ) {
				s->gc_block.cells[i].type		= CELL_FREE;
				s->gc_block.cells[i].object.pair.head	= NIL_CELL;
				s->gc_block.cells[i].object.pair.tail	= &(s->gc_block.cells[i + 1]);
			}

			s->gc_block.cells[s->gc_block.count - 1].object.pair.tail	= NIL_CELL;
		} else {
			fprintf(stderr, "freed %u\n", freed_count);
		}
	}

	cell_ptr_t	c = s->gc_block.free_list;
	s->gc_block.free_list	= list_tail(c);
	--(s->gc_block.free_count);
	return c;
}

static INLINE void
mark_cell(cell_ptr_t cell)
{
	if( cell != NIL_CELL && !gc_is_reachable(cell) ) {
		switch( cell->type ) {
		case ATOM_SYMBOL:
		case ATOM_BOOL:
		case ATOM_CHAR:
		case ATOM_SINT64:
		case ATOM_REAL64:
		case ATOM_STRING:
		case CELL_FFI:
		case ATOM_ERROR:	/* ERRORS should point to another cell symbol/string ? */
			break;

		case CELL_FREE:
		case CELL_PAIR: {
			mark_cell(cell->object.pair.head);
			cell_ptr_t cell2	= list_tail(cell);
			while( cell2 != NIL_CELL && !gc_is_reachable(cell2) ) {
				mark_cell(cell2->object.pair.head);
				cell2	= list_tail(cell2);
			}

			break;
		}

		case CELL_CLOSURE:
			mark_cell(cell->object.closure.env);
			mark_cell(cell->object.closure.lambda);
			break;

		case CELL_LAMBDA:
			mark_cell(cell->object.lambda.syms);
			mark_cell(cell->object.lambda.body);
			break;

		case CELL_QUOTE:
			mark_cell(cell->object.quote.list);
			break;
		}
		gc_mark_reachable(cell);
	}
}

uint32
gc(state_t* s)
{
	uint32	nc	= 0;

	for( uint32 i = 0; i < s->gc_block.count; ++i ) {
		if( s->gc_block.cells[i].type != CELL_FREE ) {
			gc_mark_unreachable(&(s->gc_block.cells[i]));
		}
	}

	mark_cell(s->root);

	for( uint32 i = 0; i < s->gc_block.count; ++i ) {
		if( !gc_is_reachable(&(s->gc_block.cells[i])) && s->gc_block.cells[i].type != CELL_FREE ) {
			free_cell(s, &(s->gc_block.cells[i]));
			++nc;
		}
	}

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
	   cell_ptr_t c,
	   uint32 level)
{
	print_level(level);
	switch(c->type) {
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
				if( !is_nil(n->object.pair.head) ) {
					print_cell(s, n->object.pair.head, 1);
				} else {
					print_level(level + 1);
					fprintf(stderr, "nil");
				}

				n	= list_tail(n);
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
	cell_ptr_t ret	= cell_alloc(s); \
	ret->type	= ENUM; \
	ret->object.FIELD	= v; \
	return ret; \
	}

cell_ptr_t
atom_new_symbol(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_ptr_t ret	= cell_alloc(s);

	ret->type	= ATOM_SYMBOL;
	ret->object.symbol	= (char*)(malloc(len + 1));
	memcpy(ret->object.symbol, b, len + 1);
	return ret;
}

cell_ptr_t
atom_new_boolean(state_t* s,
		 bool b)
{
	cell_ptr_t ret	= cell_alloc(s);
	ret->type	= ATOM_BOOL;
	ret->object.boolean	= b;
	return ret;
}

cell_ptr_t
atom_new_char(state_t* s,
	      char c)
{
	cell_ptr_t ret	= cell_alloc(s);
	ret->type	= ATOM_CHAR;
	ret->object.ch		= c;
	return ret;
}

IMPLEMENT_TYPE_CELL(sint64, s64, ATOM_SINT64)
IMPLEMENT_TYPE_CELL(real64, r64, ATOM_REAL64)

cell_ptr_t
atom_new_string(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_ptr_t ret	= cell_alloc(s);

	ret->type	= ATOM_STRING;
	ret->object.string	= (char*)(malloc(len + 1));
	memcpy(ret->object.string, b, len + 1);
	return ret;
}

cell_ptr_t
schizo_error(state_t* s,
	     const char* error)
{
	size_t len	= strlen(error);
	cell_ptr_t ret	= cell_alloc(s);

	ret->type	= ATOM_ERROR;
	ret->object.string	= (char*)(malloc(len + 1));
	memcpy(ret->object.string, error, len + 1);
	return ret;
}

cell_ptr_t
cell_new_bind_list(state_t* s)
{
	cell_ptr_t ret	= cell_alloc(s);
	ret->type	= CELL_BIND;
	ret->object.bindings	= NIL_CELL;
	return ret;
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
	cell_ptr_t ret	= cell_alloc(s);
	ret->type	= CELL_PAIR;
	ret->object.pair.head	= head;
	ret->object.pair.tail	= NIL_CELL;
	return ret;
}

cell_ptr_t
list_cons(state_t* s,
	  cell_ptr_t head,
	  cell_ptr_t tail)
{
	cell_ptr_t ret	= list_new(s, head);

	assert( tail == NIL_CELL || tail->type == CELL_PAIR );
	ret->object.pair.tail	= tail;

	return ret;
}

uint32
list_length(cell_ptr_t list)
{
	cell_ptr_t	curr	= list;
	uint32		len	= 0;

	while( !is_nil(curr) ) {
		assert( list->type == CELL_PAIR );
		++len;
		curr	= list_tail(curr);
	}
	return len;
}

cell_ptr_t
list_reverse_in_place(cell_ptr_t list)
{
	if( !is_nil(list) ) {
		assert( list->type == CELL_PAIR );

		cell_ptr_t	curr	= list;
		cell_ptr_t	next	= list_tail(curr);
		while( !is_nil(next) ) {
			assert( curr->type == CELL_PAIR );

			cell_ptr_t	tmp	= list_tail(next);

			next->object.pair.tail	= curr;

			curr		= next;
			next		= tmp;
		}

		list->object.pair.tail	= NIL_CELL;
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
	while( !is_nil(list_head(l0)) && !is_nil(list_head(l1)) ) {
		res	= list_cons(s, list_make_pair(s, list_head(l0), list_head(l1)), res);
		l0	= list_tail(l0);
		l1	= list_tail(l1);
	}

	if( is_nil(l0) != is_nil(l1) ) {
		return schizo_error(s, "ERROR: couldn't zip the lists, one is longer than the other");
	} else {
		return list_reverse_in_place(res);
	}
}

/*******************************************************************************
** eval
*******************************************************************************/
static cell_ptr_t
symbol_lookup(cell_ptr_t env,
	      const char* sym)
{
	cell_ptr_t	pair	= list_head(env);

	while( pair != NIL_CELL && strcmp(sym, list_head(pair)->object.symbol) != 0 ) {
		env	= list_tail(env);
		pair	= list_head(env);
	}

	if( pair == NIL_CELL ) {
		return NIL_CELL;
	} else {
		if( list_tail(pair) != NIL_CELL ) {
			return list_head(list_tail(pair));
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
	s->registers.env_stack = list_tail(s->registers.env_stack);
}


static cell_ptr_t
eval_list(state_t* s,
	  cell_ptr_t expr)
{
	cell_ptr_t	res	= NIL_CELL;
	while( !is_nil(expr) ) {
		res	= list_cons(s, eval(s, list_head(expr)), res);
		expr	= list_tail(expr);
	}
	return list_reverse_in_place(res);
}

#define RETURN(EXP)	retval = EXP; pop_env(s); return retval

cell_ptr_t
eval(state_t *s,
     cell_ptr_t exp)
{
	cell_ptr_t	retval	= NIL_CELL;

	push_env(s);
	while( exp != NIL_CELL ) {	/* not a NIL_CELL */
		switch( exp->type ) {
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
			RETURN(exp);

		/* symbols */
		case ATOM_SYMBOL:
			RETURN(symbol_lookup(s->registers.current_env, exp->object.symbol));

		/* symbol binding */
		case CELL_BIND:

		/* applications */
		case CELL_PAIR:	{
			cell_ptr_t	head	= eval(s, list_head(exp));

			if( head == NIL_CELL ) {
				RETURN(NIL_CELL);
			}

			cell_ptr_t	tail	= list_tail(exp);

			switch( head->type ) {
			case CELL_FFI:
				if( head->flags & EVAL_ARGS ) {
					uint32	l	= list_length(tail);
					if( head->object.ffi.arg_count > 0 && l != head->object.ffi.arg_count ) {
						fprintf(stderr, "ERROR: function requires %d arguments, only %d were given\n", l, head->object.ffi.arg_count);
						RETURN(NIL_CELL);
					} else {
						RETURN(head->object.ffi.proc(s, eval_list(s, tail)));
					}
				} else {	/* lambda, define, if */
					exp	= head->object.ffi.proc(s, tail);
				}
				break;
			case CELL_CLOSURE: {
				cell_ptr_t lambda	= head->object.closure.lambda;

				//environment_push(s);

				s->registers.current_env	= head->object.closure.env;

				/* evaluate the arguments and zip them */
				if( list_length(tail) != list_length(lambda->object.lambda.syms) ) {
					RETURN(schizo_error(s, "ERROR: closure arguments do not match given arguments"));
				}
				cell_ptr_t	args	= eval_list(s, tail);
				cell_ptr_t	syms	= lambda->object.lambda.syms;

				while( !is_nil(list_head(syms)) && !is_nil(list_head(args)) ) {
					s->registers.current_env	= list_cons(s, list_make_pair(s, list_head(syms), list_head(args)), s->registers.current_env);
					args	= list_tail(args);
					syms	= list_tail(syms);
				}

				if( is_nil(syms) != is_nil(args) ) {
					RETURN(schizo_error(s, "ERROR: couldn't zip the lists, one is longer than the other"));
				} else {
					/* all good, evaluate the body(*) */
					cell_ptr_t	body	= lambda->object.lambda.body;
					cell_ptr_t	next	= list_tail(body);

					exp	= list_head(body);

					while( !is_nil(next) ) {
						exp	= eval(s, exp);	/* never return a tail call */
						exp	= list_head(next);
						next	= list_tail(next);
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
	cell_ptr_t sym	= list_head(args);
	cell_ptr_t body	= list_head(list_tail(args));

	cell_ptr_t pair	= list_make_pair(s, sym, body);
	s->registers.current_env	= list_cons(s, pair, s->registers.current_env);
	return NIL_CELL;
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
	cell_ptr_t	syms	= list_head(args);
	cell_ptr_t	body	= list_tail(args);

	cell_ptr_t	closure	= cell_alloc(s);
	cell_ptr_t	lambda	= cell_alloc(s);

	lambda->type	= CELL_LAMBDA;
	lambda->object.lambda.syms	= syms;
	lambda->object.lambda.body	= body;

	closure->type	= CELL_CLOSURE;
	closure->object.closure.lambda	= lambda;
	closure->object.closure.env	= s->registers.current_env;
	return closure;
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
	cell_ptr_t	c	= atom_new_symbol(s, sym);
	cell_ptr_t	f	= cell_alloc(s);
	f->type	= CELL_FFI;
	f->flags	|= eval_args ? EVAL_ARGS : 0;
	f->object.ffi.arg_count	= arg_count;
	f->object.ffi.proc	= call;

	cell_ptr_t	p		= list_make_pair(s, c, f);
	s->registers.current_env	= list_cons(s, p, s->registers.current_env);
}

state_t*
state_new()
{
	state_t*	state	= (state_t*)malloc(sizeof(state_t));
	memset(state, 0, sizeof(state_t));

	state_add_ffi(state, false, "lambda", make_closure, -1);
	state_add_ffi(state, false, "define", symbol_define, 2);
	state_add_ffi(state, true, "display", display, 1);

	return state;
}

void
state_release(state_t *s)
{
	free(s);
}
