#include "ast.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define IMPLEMENT_TYPE_CELL(TYPE, FIELD, ENUM)	cell_t* \
						cell_new_ ## TYPE(TYPE v) { \
							cell_t*	ret	= (cell_t*)malloc(sizeof(cell_t)); \
							ret->type	= ENUM; \
							ret->FIELD	= v; \
							ret->next	= NULL; \
							return ret; \
						}
cell_t*
cell_new_symbol(const char* b) {
	size_t len	= strlen(b);
	cell_t*	ret	= (cell_t*)malloc(sizeof(cell_t));

	ret->type	= CELL_SYMBOL;
	ret->symbol	= (char*)(malloc(len + 1));
	memcpy(ret->symbol, b, len + 1);
	ret->next	= NULL;
	return ret;
}

cell_t*
cell_new_boolean(boolean b) {
	cell_t* ret	= (cell_t*)malloc(sizeof(cell_t));
	ret->type	= CELL_BOOL;
	ret->boolean	= b;
	ret->next	= NULL;
	return ret;
}

cell_t*
cell_new_char(char c) {
	cell_t* ret	= (cell_t*)malloc(sizeof(cell_t));
	ret->type	= CELL_CHAR;
	ret->ch		= c;
	ret->next	= NULL;
	return ret;
}

IMPLEMENT_TYPE_CELL(sint8,  s8,  CELL_SINT8)
IMPLEMENT_TYPE_CELL(sint16, s16, CELL_SINT16)
IMPLEMENT_TYPE_CELL(sint32, s32, CELL_SINT32)
IMPLEMENT_TYPE_CELL(sint64, s64, CELL_SINT64)
IMPLEMENT_TYPE_CELL(uint8,  u8,  CELL_UINT8)
IMPLEMENT_TYPE_CELL(uint16, u16, CELL_UINT16)
IMPLEMENT_TYPE_CELL(uint32, u32, CELL_UINT32)
IMPLEMENT_TYPE_CELL(uint64, u64, CELL_UINT64)
IMPLEMENT_TYPE_CELL(real32, r32, CELL_REAL32)
IMPLEMENT_TYPE_CELL(real64, r64, CELL_REAL64)

cell_t*
cell_new_string(const char* b) {
	size_t len	= strlen(b);
	cell_t*	ret	= (cell_t*)malloc(sizeof(cell_t));

	ret->type	= CELL_STRING;
	ret->string	= (char*)(malloc(len + 1));
	memcpy(ret->symbol, b, len + 1);
	ret->next	= NULL;
	return ret;
}

cell_t*
cell_new_cons(cell_t* to) {
	cell_t* ret	= (cell_t*)malloc(sizeof(cell_t));
	ret->type	= CELL_CONS;
	ret->cons	= to;
	ret->next	= NULL;
	return ret;
}

cell_t*
cell_cons(cell_t* c,
	  cell_t* list)
{
	c->next	= list;
	return c;
}

static void
print_level(uint32 level)
{
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

	case CELL_SINT8:
		fprintf(stderr, "sint8: %ld", (sint64)c->s8);
		break;

	case CELL_SINT16:
		fprintf(stderr, "sint16: %ld", (sint64)c->s16);
		break;

	case CELL_SINT32:
		fprintf(stderr, "sint32: %ld", (sint64)c->s32);
		break;

	case CELL_SINT64:
		fprintf(stderr, "sint64: %ld", (sint64)c->s64);
		break;

	case CELL_UINT8:
		fprintf(stderr, "uint8: %lu", (uint64)c->u8);
		break;

	case CELL_UINT16:
		fprintf(stderr, "uint16: %lu", (uint64)c->u16);
		break;

	case CELL_UINT32:
		fprintf(stderr, "uint32: %lu", (uint64)c->u32);
		break;

	case CELL_UINT64:
		fprintf(stderr, "uint64: %lu", (uint64)c->u64);
		break;

	case CELL_REAL32:
		fprintf(stderr, "real32: %lf", (real64)c->r32);
		break;

	case CELL_REAL64:
		fprintf(stderr, "real64: %lf", (real64)c->r64);
		break;

	case CELL_STRING:
		fprintf(stderr, "string: %s", c->string);
		break;

	case CELL_CONS:
		if( c->cons ) {
			fprintf(stderr, "cons:\n");
			print_cell(c->cons, level + 1);
			print_level(level);
		} else {
			fprintf(stderr, "cons: nil\n");
		}

		if( c->next ) {
			cell_t* n = c->next;
			while(n) {
				if( n->cons ) {
					fprintf(stderr, "cons:\n");
					print_cell(n->cons, level + 1);
					print_level(level);
				} else {
					fprintf(stderr, "cons: nil\n");
				}

				n	= n->next;
			}
		}
		break;
	default:
		break;
	}

	fprintf(stderr, "\n");
}
