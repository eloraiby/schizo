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

	ret->type	= CELL_SYMBOL;
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
	ret->type	= CELL_BOOL;
	ret->object.boolean	= b;
	return id;
}

cell_id_t
atom_new_char(state_t* s,
	      char c)
{
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];
	ret->type	= CELL_CHAR;
	ret->object.ch		= c;
	return id;
}

IMPLEMENT_TYPE_CELL(sint64, s64, CELL_SINT64)
IMPLEMENT_TYPE_CELL(real64, r64, CELL_REAL64)

cell_id_t
atom_new_string(state_t* s,
		const char* b)
{
	size_t len	= strlen(b);
	cell_id_t id	= cell_alloc(s);
	cell_t*	ret	= &s->gc_block.cells[id.index];

	ret->type	= CELL_STRING;
	ret->object.string	= (char*)(malloc(len + 1));
	memcpy(ret->object.symbol, b, len + 1);
	return id;
}
