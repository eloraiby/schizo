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
