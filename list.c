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

cell_id_t
list_new_pair(state_t* s,
	      cell_id_t car)
{
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];
	ret->type	= CELL_PAIR;
	ret->object.pair.head	= car;
	ret->object.pair.tail	= cell_nil();
	return id;
}

cell_id_t
list_cons(state_t* s,
	  cell_id_t car,
	  cell_id_t list)
{
	cell_id_t id	= list_new_pair(s, car);
	cell_t* ret	= &s->gc_block.cells[id.index];
	ret->object.pair.tail	= list;
	return id;
}

cell_id_t
list_reverse_in_place(state_t *s,
		      cell_id_t list)
{
	cell_t*		c	= cell_from_index(s, list);
	cell_id_t	current	= list;
	cell_id_t	next	= c->object.pair.tail;
	while( !is_nil(next) ) {
		cell_t*		n	= cell_from_index(s, next);
		cell_id_t	tmp	= n->object.pair.tail;

		n->object.pair.tail	= current;
		c->object.pair.tail	= tmp;
		current		= next;
		next		= tmp;
	}
	return current;
}
