#ifndef SCHIZO_H
#define SCHIZO_H
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
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* platform related */
#ifdef __GNUC__
#	define INLINE inline __attribute__ ((__unused__))
#	define UNUSED __attribute__ ((__unused__))
#else
#	define INLINE __inline
#	define UNUSED
#endif

#ifdef __cplusplus
extern "C" {
#else
typedef	unsigned char	bool;
#	define false	(0)
#	define true	(!false)
#endif

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

#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) < (b) ? (a) : (b))

/* schizo related */

typedef uint16		CELL_TYPE;


struct cons_t;
struct closure_t;
struct cell_t;

typedef struct state_t	state_t;

/* C doesn't have units of measure, fake them */
typedef struct cell_id_t {
	uint32		index;
} cell_id_t;

typedef struct ret_code_t {
	sint32		code;
} ret_code_t;

/* lists */
typedef struct pair_t {
	cell_id_t	tail;		/* the tail */
	cell_id_t	head;		/* the head */
} pair_t;

/* lambda */
typedef struct lambda_t {
	cell_id_t	syms;		/* arguments */
	cell_id_t	body;		/* closure body */
} lambda_t;

/* foreign function */
typedef cell_id_t (*ffi_call_t)(state_t* s, cell_id_t args);

typedef struct ffi_t {
	uint32		arg_count;	/* total argument count, -1 = any */
	ffi_call_t	proc;		/* the procedure */
} ffi_t;

/* closure */
typedef struct closure_t {
	cell_id_t	env;		/* captured environment */
	cell_id_t	lambda;		/* original args and body pair */
} closure_t;

/* for a foreign procedure, the arguments are evaluated before calling the procedure */
typedef cell_id_t	(*foreign_procedure_t)(state_t* sc, cell_id_t args);

/* for a foreign syntax, the arguments are not evaluated and are left to the syntax to interpret them */
typedef cell_id_t	(*foreign_syntax_t)(state_t* sc, cell_id_t env, cell_id_t args);

#define GC_REACHABLE	0x8000
#define FOREIGN		0x0800
#define SYNTAX		0x0400

typedef struct cell_t {
	uint16		flags;
	CELL_TYPE	type;

	union {
		char*		symbol;
		char*		op;

		bool		boolean;

		char		ch;

		sint64		s64;
		real64		r64;

		char*		string;
		pair_t		pair;
		lambda_t	lambda;
		closure_t	closure;
		ffi_t		ffi;
	} object;

} cell_t;

struct state_t {
	struct gc_block_t {
		cell_t*		cells;
		uint32		count;
		uint32		free_count;
		cell_id_t	free_list;
	} gc_block;

	struct {
		cell_id_t	current_exp;	/* current expression pointer */
		cell_id_t	stack;		/* current environment head */
	} environment;

	cell_id_t	root;			/* root cell */
	cell_id_t	current_cell;
	const char*	token_start;
	const char*	token_end;
	uint32		token_line;
};

/*
 * Inline helpers
 */
static INLINE cell_id_t
cell_id(uint32 id)
{
	cell_id_t	ret = { id };
	return ret;
}

#define NIL_CELL	0x0
#define is_nil(c)	((c).index == NIL_CELL)

static INLINE cell_id_t	cell_nil()		{ cell_id_t	id = { 0 };		return id;	}
static INLINE cell_id_t cell_quote()		{ cell_id_t	id = { (uint32)-1 };	return id;	}
static INLINE cell_id_t cell_quasiquote()	{ cell_id_t	id = { (uint32)-2 };	return id;	}

#define cell_from_index(s, idx)			(&(s)->gc_block.cells[(idx).index])
#define list_head(s, list)			((s)->gc_block.cells[(list).index].object.pair.head)
#define list_tail(s, list)			((s)->gc_block.cells[(list).index].object.pair.tail)
#define cell_type(s, c)				((s)->gc_block.cells[(c).index].type)

static INLINE cell_id_t
index_from_cell(state_t* s,
		cell_t* c)
{
	cell_id_t	ret	= { (uint32)(c - s->gc_block.cells) };
	return ret;
}

#define gc_mark_reachable(sc, cid)	{ \
						cell_t*	c	= cell_from_index((sc), (cid)); \
						c->flags	|= GC_REACHABLE; \
					}

#define gc_mark_unreachable(sc, cid)	{ \
						cell_t* c	= cell_from_index((sc), (cid)); \
						c->flags	&= ~GC_REACHABLE; \
					}

#define gc_is_reachable(sc, cid)	((cell_from_index((sc), (cid))->flags & GC_REACHABLE) ? true : false)


/* printing */
void		print_cell(state_t* s, cell_id_t c, uint32 level);

/* atoms */
cell_id_t	atom_new_symbol(state_t* s, const char* sym);
cell_id_t	atom_new_boolean(state_t* s, bool b);
cell_id_t	atom_new_char(state_t* s, char c);
cell_id_t	atom_new_sint64(state_t* s, sint64 i);
cell_id_t	atom_new_real64(state_t* s, real64 i);
cell_id_t	atom_new_string(state_t* s, const char* b);
cell_id_t	atom_new_unary_op(state_t* s, const char* op);
cell_id_t	atom_new_binary_op(state_t* s, const char* op);

/* lists */
cell_id_t	list_new(state_t* s, cell_id_t head);
cell_id_t	list_cons(state_t* s, cell_id_t head, cell_id_t tail);
#define		like_make_pair(s, fst, snd)	(list_cons((s), (fst), list_new((s), (snd))))

cell_id_t	list_reverse_in_place(state_t* s, cell_id_t list);
uint32		list_length(state_t* s, cell_id_t list);

/* vectors */
cell_id_t	cell_vector(state_t* s, uint32 count);

/* parser.y / lexer.rl */
void		parse(state_t* state, const char* str);

/* states */
state_t*	state_new();
void		state_release(state_t* s);
void		state_call(state_t* s, cell_id_t fn, cell_id_t args);

#ifdef __cplusplus
}
#endif

#endif /* SCHIZO_H */
