#ifndef AST_H
#define AST_H
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
#include "parser.h"
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif
typedef	unsigned char	boolean;

#define FALSE		(0)
#define TRUE		(!FALSE)

typedef int8_t		sint8;
typedef int16_t		sint16;
typedef int32_t		sint32;
typedef int64_t		sint64;

typedef uint8_t		uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;

typedef float		real32;
typedef double		real64;

typedef int CELL_TYPE;

struct cons_t;
struct closure_t;
struct cell_t;

typedef struct cons_t {
	struct cell_t*	cdr;	/* the tail */
	struct cell_t*	car;
} cons_t;

typedef struct closure_t {
	struct cell_t*	env;
	struct cell_t*	expression;
} closure_t;

typedef struct cell_t {
	CELL_TYPE	type;

	union {
		char*		symbol;

		boolean		boolean;

		char		ch;

		sint64		s64;
		real64		r64;

		char*		string;
		cons_t		cons;
		closure_t	closure;
	};

} cell_t;

cell_t*		cell_new_symbol(const char* sym);
cell_t*		cell_new_boolean(boolean b);
cell_t*		cell_new_char(char c);
cell_t*		cell_new_sint64(sint64 i);
cell_t*		cell_new_real64(real64 i);
cell_t*		cell_new_string(const char* b);

cell_t*		cell_new_cons(cell_t *car);
cell_t*		cell_cons(cell_t* car, cell_t* list);
cell_t*		cell_car(cell_t* list);
cell_t*		cell_cdr(cell_t* list);



typedef struct state_t {
	cell_t*		root;
	cell_t*		current_cell;
	const char*	token_start;
	const char*	token_end;
	uint32		token_line;
} state_t;

state_t*	parse(const char* str);

void		print_cell(cell_t* c, uint32 level);
#ifdef __cplusplus
}
#endif

#endif /* AST_H */
