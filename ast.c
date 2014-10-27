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

#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define IMPLEMENT_TYPE_CELL(TYPE, FIELD, ENUM)	cell_t* \
						cell_new_ ## TYPE(TYPE v) { \
							cell_t*	ret	= (cell_t*)malloc(sizeof(cell_t)); \
							ret->type	= ENUM; \
							ret->FIELD	= v; \
							return ret; \
						}
cell_t*
cell_new_symbol(const char* b) {
	size_t len	= strlen(b);
	cell_t*	ret	= (cell_t*)malloc(sizeof(cell_t));

	ret->type	= CELL_SYMBOL;
	ret->symbol	= (char*)(malloc(len + 1));
	memcpy(ret->symbol, b, len + 1);
	return ret;
}

cell_t*
cell_new_boolean(boolean b) {
	cell_t* ret	= (cell_t*)malloc(sizeof(cell_t));
	ret->type	= CELL_BOOL;
	ret->boolean	= b;
	return ret;
}

cell_t*
cell_new_char(char c) {
	cell_t* ret	= (cell_t*)malloc(sizeof(cell_t));
	ret->type	= CELL_CHAR;
	ret->ch		= c;
	return ret;
}


IMPLEMENT_TYPE_CELL(sint64, s64, CELL_SINT64)
IMPLEMENT_TYPE_CELL(real64, r64, CELL_REAL64)

cell_t*
cell_new_string(const char* b) {
	size_t len	= strlen(b);
	cell_t*	ret	= (cell_t*)malloc(sizeof(cell_t));

	ret->type	= CELL_STRING;
	ret->string	= (char*)(malloc(len + 1));
	memcpy(ret->symbol, b, len + 1);
	return ret;
}

cell_t*
cell_new_cons(cell_t* car) {
	cell_t* ret	= (cell_t*)malloc(sizeof(cell_t));
	ret->type	= CELL_CONS;
	ret->cons.car	= car;
	ret->cons.cdr	= NULL;
	return ret;
}

cell_t*
cell_cons(cell_t* car,
	  cell_t* list)
{
	cell_t* ret	= cell_new_cons(car);
	ret->cons.cdr	= list;
	return ret;
}

cell_t*
cell_car(cell_t* list)
{
	return list->cons.car;
}

cell_t*
cell_cdr(cell_t* list)
{
	return list->cons.cdr;
}


static void
print_level(uint32 level) {
	for( uint32 i = 0; i < level; ++i ) {
		fprintf(stderr, "  ");
	}
}

void
print_cell(cell_t *c,
	   uint32 level)
{
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
		if( c->cons.car ) {
			fprintf(stderr, "cons:\n");
			print_cell(c->cons.car, level + 1);
			print_level(level);
		} else {
			fprintf(stderr, "cons: nil\n");
		}

		if( c->cons.cdr ) {
			cell_t* n = c->cons.cdr;
			while(n) {
				if( n->cons.car ) {
					fprintf(stderr, "cons:\n");
					print_cell(n->cons.car, level + 1);
					print_level(level);
				} else {
					fprintf(stderr, "cons: nil\n");
				}

				n	= n->cons.cdr;
			}
		}
		break;
	default:
		break;
	}

	fprintf(stderr, "\n");
}
