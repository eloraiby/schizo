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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CELL_COUNT	4096

#define IMPLEMENT_TYPE_CELL(TYPE, FIELD, ENUM)	cell_id_t \
						cell_new_ ## TYPE(state_t* s, TYPE v) { \
							cell_id_t id	= cell_alloc(s); \
							cell_t* ret	= &s->gc_block.cells[id.index]; \
							ret->type	= ENUM; \
							ret->FIELD	= v; \
							return id; \
						}
INLINE cell_t*
cell_from_index(state_t* s,
		cell_id_t idx)
{
	return &s->gc_block.cells[idx.index];
}

cell_id_t
cell_alloc(state_t* s) {
	/*
	** TODO: handle reallocation, calling garbage collector and the rest...
	** Unit testing is crucial: ensure all branches are visited.
	*/
	if( s->gc_block.cells == NULL ) {
		s->gc_block.count	= INITIAL_CELL_COUNT;
		s->gc_block.free_count	= INITIAL_CELL_COUNT;
		s->gc_block.free_list	= cell_id(1);	/* 0 is reserved for the nil cell */
		s->gc_block.cells	= (cell_t*)malloc(sizeof(cell_t) * s->gc_block.count);
		memset(s->gc_block.cells, 0, sizeof(cell_t) * s->gc_block.free_count);

		for( uint32 i = s->gc_block.free_list.index; i < s->gc_block.count; ++i ) {
			s->gc_block.cells[i].type	= CELL_CONS;
			s->gc_block.cells[i].cons.car	= nil();
			s->gc_block.cells[i].cons.cdr	= cell_id(i + 1);
		}
	} else if( s->gc_block.free_list.index == NIL_CELL ){
		exit(1);
	}

	cell_id_t	c = s->gc_block.free_list;
	s->gc_block.free_list	= cell_cdr(s, c);
	--(s->gc_block.free_count);
	return c;
}

cell_id_t
cell_new_symbol(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];

	ret->type	= CELL_SYMBOL;
	ret->symbol	= (char*)(malloc(len + 1));
	memcpy(ret->symbol, b, len + 1);
	return id;
}

cell_id_t
cell_new_boolean(state_t* s,
		 bool b)
{
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];
	ret->type	= CELL_BOOL;
	ret->boolean	= b;
	return id;
}

cell_id_t
cell_new_char(state_t* s,
	      char c)
{
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];
	ret->type	= CELL_CHAR;
	ret->ch		= c;
	return id;
}

IMPLEMENT_TYPE_CELL(sint64, s64, CELL_SINT64)
IMPLEMENT_TYPE_CELL(real64, r64, CELL_REAL64)

cell_id_t
cell_new_string(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];

	ret->type	= CELL_STRING;
	ret->string	= (char*)(malloc(len + 1));
	memcpy(ret->symbol, b, len + 1);
	return id;
}

cell_id_t
cell_new_cons(state_t* s,
	      cell_id_t car)
{
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];
	ret->type	= CELL_CONS;
	ret->cons.car	= car;
	ret->cons.cdr	= nil();
	return id;
}

cell_id_t
cell_cons(state_t* s,
	  cell_id_t car,
	  cell_id_t list)
{
	cell_id_t id	= cell_new_cons(s, car);
	cell_t* ret	= &s->gc_block.cells[id.index];
	ret->cons.cdr	= list;
	return id;
}

cell_id_t
cell_reverse_in_place(state_t *s,
		      cell_id_t list)
{
	cell_t*		c	= cell_from_index(s, list);
	cell_id_t	current	= list;
	cell_id_t	next	= c->cons.cdr;
	while( !is_nil(next) ) {
		cell_t*		n	= cell_from_index(s, next);
		cell_id_t	tmp	= n->cons.cdr;

		n->cons.cdr	= current;
		c->cons.cdr	= tmp;
		current		= next;
		next		= tmp;
	}
	return current;
}

cell_id_t
cell_car(state_t* s, cell_id_t list)
{
	cell_t*	c	= &s->gc_block.cells[list.index];
	return c->cons.car;
}

cell_id_t
cell_cdr(state_t* s, cell_id_t list)
{
	cell_t*	c	= &s->gc_block.cells[list.index];
	return c->cons.cdr;
}


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
	case CELL_BOOL:
		if( c->boolean ) {
			fprintf(stderr, "bool: true");
		} else {
			fprintf(stderr, "bool: false");
		}
		break;

	case CELL_SYMBOL:
		fprintf(stderr, "symbol: %s", c->symbol);
		break;

	case CELL_CHAR:
		fprintf(stderr, "char: '%c'", c->ch);
		break;

	case CELL_SINT64:
		fprintf(stderr, "sint64: %ld", (sint64)c->s64);
		break;

	case CELL_REAL64:
		fprintf(stderr, "real64: %lf", (real64)c->r64);
		break;

	case CELL_STRING:
		fprintf(stderr, "string: %s", c->string);
		break;

	case CELL_CONS:
		if( !is_nil(c->cons.car) ) {
			fprintf(stderr, "cons:\n");
			print_cell(s, c->cons.car, level + 1);
			print_level(level);
		} else {
			fprintf(stderr, "cons: nil\n");
		}

		if( !is_nil(c->cons.cdr) ) {
			cell_id_t id	= c->cons.cdr;
			while( !is_nil(id) ) {
				cell_t* n = &s->gc_block.cells[id.index];
				if( !is_nil(n->cons.car) ) {
					fprintf(stderr, "cons:\n");
					print_cell(s, n->cons.car, level + 1);
					print_level(level);
				} else {
					fprintf(stderr, "cons: nil\n");
				}

				id	= cell_cdr(s, id);
			}
		}
		break;
	default:
		break;
	}

	fprintf(stderr, "\n");
}
