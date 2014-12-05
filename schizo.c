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
#define INITIAL_CELL_COUNT	2	/* this should be at least 2 (index 0 is reserved for NIL) */

cell_id_t
cell_alloc(state_t* s) {
	/*
	** TODO: handle reallocation, calling garbage collector and the rest...
	** Unit testing is crucial: ensure all branches are visited.
	*/
	if( s->gc_block.cells == NULL ) {
		s->gc_block.count	= INITIAL_CELL_COUNT;
		s->gc_block.free_count	= INITIAL_CELL_COUNT - 1;
		s->gc_block.free_list	= cell_id(1);	/* 0 is reserved for the nil cell */
		s->gc_block.cells	= (cell_t*)malloc(sizeof(cell_t) * s->gc_block.count);
		memset(s->gc_block.cells, 0, sizeof(cell_t) * s->gc_block.free_count);

		for( uint32 i = s->gc_block.free_list.index; i < s->gc_block.count; ++i ) {
			s->gc_block.cells[i].type	= CELL_PAIR;
			s->gc_block.cells[i].object.pair.head	= cell_nil();
			s->gc_block.cells[i].object.pair.tail	= cell_id(i + 1);
		}

		s->gc_block.cells[INITIAL_CELL_COUNT - 1].object.pair.tail	= cell_nil();

		/* first cell, is the nil cell */
		s->gc_block.cells[0].type	= CELL_PAIR;

	} else if( s->gc_block.free_list.index == NIL_CELL ) {

		/* TODO: call garbage collector before trigger a reallocation */

		s->gc_block.free_count	= s->gc_block.count;
		s->gc_block.free_list	= cell_id(s->gc_block.count);
		s->gc_block.count	*= 2;
		s->gc_block.cells	= (cell_t*)realloc(s->gc_block.cells, sizeof(cell_t) * s->gc_block.count);

		if( s->gc_block.cells == NULL ) {
			fprintf(stderr, "Fatal error: Not enough memory...\n");
			exit(1);
		}

		for( uint32 i = s->gc_block.free_list.index; i < s->gc_block.count; ++i ) {
			s->gc_block.cells[i].type	= CELL_PAIR;
			s->gc_block.cells[i].object.pair.head	= cell_nil();
			s->gc_block.cells[i].object.pair.tail	= cell_id(i + 1);
		}

		s->gc_block.cells[s->gc_block.count - 1].object.pair.tail	= cell_nil();
	}

	cell_id_t	c = s->gc_block.free_list;
	s->gc_block.free_list	= list_tail(s, c);
	--(s->gc_block.free_count);
	return c;
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
	   cell_id_t cid,
	   uint32 level)
{
	cell_t*	c	= &s->gc_block.cells[cid.index];

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
			cell_id_t id	= c->object.pair.tail;
			while( !is_nil(id) ) {
				cell_t* n = &s->gc_block.cells[id.index];
				if( !is_nil(n->object.pair.head) ) {
					print_cell(s, n->object.pair.head, 1);
				} else {
					print_level(level + 1);
					fprintf(stderr, "nil");
				}

				id	= list_tail(s, id);
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

#define IMPLEMENT_TYPE_CELL(TYPE, FIELD, ENUM)	cell_id_t \
						atom_new_ ## TYPE(state_t* s, TYPE v) { \
							cell_id_t id	= cell_alloc(s); \
							cell_t* ret	= &s->gc_block.cells[id.index]; \
							ret->type	= ENUM; \
							ret->object.FIELD	= v; \
							return id; \
						}

cell_id_t
atom_new_symbol(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];

	ret->type	= ATOM_SYMBOL;
	ret->object.symbol	= (char*)(malloc(len + 1));
	memcpy(ret->object.symbol, b, len + 1);
	return id;
}

cell_id_t
atom_new_boolean(state_t* s,
		 bool b)
{
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];
	ret->type	= ATOM_BOOL;
	ret->object.boolean	= b;
	return id;
}

cell_id_t
atom_new_char(state_t* s,
	      char c)
{
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];
	ret->type	= ATOM_CHAR;
	ret->object.ch		= c;
	return id;
}

IMPLEMENT_TYPE_CELL(sint64, s64, ATOM_SINT64)
IMPLEMENT_TYPE_CELL(real64, r64, ATOM_REAL64)

cell_id_t
atom_new_string(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];

	ret->type	= ATOM_STRING;
	ret->object.string	= (char*)(malloc(len + 1));
	memcpy(ret->object.symbol, b, len + 1);
	return id;
}
/*
cell_id_t
atom_new_unary_op(state_t* s,
		  const char* op)
{
	size_t len	= strlen(op);
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];

	ret->type	= ATOM_UNARY_OP;
	ret->object.op	= (char*)(malloc(len + 1));
	memcpy(ret->object.symbol, op, len + 1);
	return id;

}

cell_id_t
atom_new_binary_op(state_t* s,
		   const char* op)
{
	size_t len	= strlen(op);
	cell_id_t id	= cell_alloc(s);
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

cell_id_t
list_new(state_t* s,
	 cell_id_t head)
{
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];
	ret->type	= CELL_PAIR;
	ret->object.pair.head	= head;
	ret->object.pair.tail	= cell_nil();
	return id;
}

cell_id_t
list_cons(state_t* s,
	  cell_id_t head,
	  cell_id_t tail)
{
	cell_id_t id	= list_new(s, head);
	cell_t* ret	= &s->gc_block.cells[id.index];

	assert( cell_from_index(s, tail)->type == CELL_PAIR );

	ret->object.pair.tail	= tail;
	return id;
}

uint32
list_length(state_t *s, cell_id_t list)
{
	cell_id_t	curr	= list;
	uint32		len	= 0;

	while( !is_nil(curr) ) {
		assert( cell_from_index(s, list)->type == CELL_PAIR );
		++len;
		curr	= list_tail(s, curr);
	}
	return len;
}

cell_id_t
list_reverse_in_place(state_t *s,
		      cell_id_t list)
{
	if( !is_nil(list) ) {
		assert( cell_from_index(s, list)->type == CELL_PAIR );

		cell_t*		c	= cell_from_index(s, list);
		cell_id_t	current	= list;
		cell_id_t	next	= c->object.pair.tail;
		while( !is_nil(next) ) {
			cell_t*		n	= cell_from_index(s, next);
			assert( cell_from_index(s, list)->type == CELL_PAIR );

			cell_id_t	tmp	= n->object.pair.tail;

			n->object.pair.tail	= current;
			c->object.pair.tail	= tmp;
			current		= next;
			next		= tmp;
		}
		return current;
	} else {
		return list;
	}
}

/*******************************************************************************
** schizo state
*******************************************************************************/
state_t*
state_new()
{
	state_t*	state	= (state_t*)malloc(sizeof(state_t));
	memset(state, 0, sizeof(state_t));
	return state;
}

void
state_release(state_t *s)
{
	free(s);
}

