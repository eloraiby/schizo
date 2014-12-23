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
#	define INLINE __inline__ __attribute__ ((__unused__))
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

/* strong typing int with unit of measures */
typedef struct cell_ptr_t {
	uint32		index;
} cell_ptr_t;

static INLINE cell_ptr_t	cell_ptr(uint32 idx)	{ cell_ptr_t ret; ret.index = idx; return ret; }

typedef struct state_t	state_t;

/* quote */
typedef struct quote_t {
	cell_ptr_t	list;
} quote_t;

/* lists */
typedef struct pair_t {
	cell_ptr_t	tail;		/* the tail */
	cell_ptr_t	head;		/* the head */
} pair_t;

/* lambda */
typedef struct lambda_t {
	cell_ptr_t	syms;		/* arguments */
	cell_ptr_t	body;		/* closure body */
} lambda_t;

/* foreign function */
typedef void (*ffi_call_t)(state_t* s);

typedef struct ffi_t {
	uint16		flags;
	sint16		arg_count;	/* total argument count, -1 = any */
	ffi_call_t	proc;		/* the procedure */
} ffi_t;

/* closure */
typedef struct closure_t {
	cell_ptr_t	env;		/* captured environment */
	cell_ptr_t	lambda;		/* original args and body pair */
} closure_t;


#define EVAL_ARGS	0x8000		/* arguments are evaluated before call */
#define ARGS_ANY	-1		/* any number of argument */

typedef struct cell_t {
	CELL_TYPE	type;
	uint32		ref_count;

	union {
		char*		symbol;
		char*		op;

		bool		boolean;

		char		ch;

		sint64		s64;
		real64		r64;

		char*		string;
		pair_t		pair;
		quote_t		quote;
		lambda_t	lambda;
		cell_ptr_t	bindings;	/* list of bindings */
		closure_t	closure;
		ffi_t		ffi;
	} object;

} cell_t;

struct state_t {
	struct gc_block_t {
		cell_t*		cells;
		uint32		count;
		uint32		free_count;
		cell_ptr_t	free_list;
	} gc_block;

	struct {
		cell_ptr_t	exp_stack;	/* expression/operator to execute */
		cell_ptr_t	env_stack;	/* environment stack */

		/* volatile registers */
		cell_ptr_t	current_env;	/* current environment */
		cell_ptr_t	current_exp;	/* current exp + args */
		cell_ptr_t	ret_val;	/* return value */
	} registers;

	struct {
		cell_ptr_t	root;		/* root cell */
		cell_ptr_t	current_cell;
		const char*	token_start;
		const char*	token_end;
		uint32		token_line;
	} parser;
};

/*
 * Inline helpers
 */

#define NIL_CELL	cell_ptr(0x0)

#define is_nil(c)	((c).index == NIL_CELL.index)

#define cell_type(s, c)				(index_to_cell(s, c)->type)

static INLINE cell_t*		index_to_cell(state_t* s, cell_ptr_t c)	{ return &(s->gc_block.cells[c.index]);		}
static INLINE cell_ptr_t	cell_to_index(state_t* s, cell_t* c)	{ cell_ptr_t ret; ret.index = (uint32)(c - s->gc_block.cells); return ret; 	}

static INLINE cell_ptr_t	list_head(state_t* s, cell_ptr_t l) { assert(cell_type(s, l) == CELL_PAIR || cell_type(s, l) == CELL_FREE); return index_to_cell(s, l)->object.pair.head; }
static INLINE cell_ptr_t	list_tail(state_t* s, cell_ptr_t l) { assert(cell_type(s, l) == CELL_PAIR || cell_type(s, l) == CELL_FREE); return index_to_cell(s, l)->object.pair.tail; }


/* printing */
void		print_cell(state_t* s, cell_ptr_t c, uint32 level);

/* atoms */
cell_ptr_t	atom_new_symbol(state_t* s, const char* sym);
cell_ptr_t	atom_new_boolean(state_t* s, bool b);
cell_ptr_t	atom_new_char(state_t* s, char c);
cell_ptr_t	atom_new_sint64(state_t* s, sint64 i);
cell_ptr_t	atom_new_real64(state_t* s, real64 i);
cell_ptr_t	atom_new_string(state_t* s, const char* b);
cell_ptr_t	atom_new_unary_op(state_t* s, const char* op);
cell_ptr_t	atom_new_binary_op(state_t* s, const char* op);

cell_ptr_t	schizo_error(state_t* s, const char* error);
void		free_cell(state_t* s, cell_ptr_t ptr);

static INLINE cell_ptr_t
__inc_ref_count(state_t* s,
	      cell_ptr_t c)
{
	if( !is_nil(c) ) {
		assert(index_to_cell(s, c)->ref_count <= 0xFFFFFFFF);
		++(index_to_cell(s, c)->ref_count);
	}
	return c;
}

static INLINE cell_ptr_t
__dec_ref_count(state_t* s,
	      cell_ptr_t c)
{
	if( !is_nil(c) ) {
		assert(index_to_cell(s, c)->ref_count > 0);
		--(index_to_cell(s, c)->ref_count);
		if( index_to_cell(s, c)->ref_count == 0 ) {
			free_cell(s, c);
		}
	}
	return c;
}

/*
** set a cell: FIRST increase THEN decrease the counters.
**	The order must be respected, especially for cases like set_cell(s, a, list_tail(s, a))
**	where decreasing the ref count first, would cause the destruction of the tail!
*/
#define set_cell(c, v)		{ \
					cell_ptr_t ev = v; \
					assert( is_nil(c) || c.index != ev.index ); \
					__inc_ref_count(s, ev); \
					__dec_ref_count(s, c); \
					c	= ev; \
				}

#define grab(c)			__inc_ref_count(s, c)
#define release(c)		__dec_ref_count(s, c)

#define __get_fst(_1, _2)		_1
#define __get_snd(_1, _2)		_2

#define __get_fst_snd_name(NAME)	NAME

#define get_fst(...)			__get_fst_snd_name(__get_fst) __VA_ARGS__
#define get_snd(...)			__get_fst_snd_name(__get_snd) __VA_ARGS__

#define bind(A, B)			(A, B)

#define let_in(P, E)			{ cell_ptr_t	get_fst(P)	= get_snd(P); grab(get_fst(P)); E; set_cell(get_fst(P), NIL_CELL); }

#define let_multi_0(P, E)		{ cell_ptr_t	get_fst(P)	= get_snd(P); grab(get_fst(P)); E; set_cell(get_fst(P), NIL_CELL); }
#define let_multi_1(P, ...)		{ cell_ptr_t	get_fst(P)	= get_snd(P); grab(get_fst(P)); let_multi_0(__VA_ARGS__) set_cell(get_fst(P), NIL_CELL); }
#define let_multi_2(P, ...)		{ cell_ptr_t	get_fst(P)	= get_snd(P); grab(get_fst(P)); let_multi_1(__VA_ARGS__) set_cell(get_fst(P), NIL_CELL); }
#define let_multi_3(P, ...)		{ cell_ptr_t	get_fst(P)	= get_snd(P); grab(get_fst(P)); let_multi_2(__VA_ARGS__) set_cell(get_fst(P), NIL_CELL); }
#define let_multi_4(P, ...)		{ cell_ptr_t	get_fst(P)	= get_snd(P); grab(get_fst(P)); let_multi_3(__VA_ARGS__) set_cell(get_fst(P), NIL_CELL); }
#define let_multi_5(P, ...)		{ cell_ptr_t	get_fst(P)	= get_snd(P); grab(get_fst(P)); let_multi_4(__VA_ARGS__) set_cell(get_fst(P), NIL_CELL); }
#define let_multi_6(P, ...)		{ cell_ptr_t	get_fst(P)	= get_snd(P); grab(get_fst(P)); let_multi_5(__VA_ARGS__) set_cell(get_fst(P), NIL_CELL); }
#define let_multi_7(P, ...)		{ cell_ptr_t	get_fst(P)	= get_snd(P); grab(get_fst(P)); let_multi_6(__VA_ARGS__) set_cell(get_fst(P), NIL_CELL); }

#define let_multi_NAME(_0, _1, _2, _3, _4, _5, _6, _7, MSSING_SCOPE, NAME, ...)	NAME
#define let_multi(...)			let_multi_NAME(__VA_ARGS__, let_multi_7, let_multi_6, let_multi_5, let_multi_4, let_multi_3, let_multi_2, let_multi_1, let_multi_0)(__VA_ARGS__)

/* lists */
cell_ptr_t	list_cons(state_t* s, cell_ptr_t head, cell_ptr_t tail);
#define		list_make_pair(s, fst, snd)	(list_cons((s), (fst), list_cons((s), (snd), NIL_CELL)))

cell_ptr_t	list_reverse(state_t* s, cell_ptr_t list);
uint32		list_length(state_t* s, cell_ptr_t list);

/* vectors */
cell_ptr_t	cell_vector(state_t* s, uint32 count);

/* parser.y / lexer.rl */
void		parse(state_t* state, const char* str);

/* states */
state_t*	state_new();
void		state_release(state_t* s);
void		state_add_ffi(state_t* s, bool eval_args, const char* sym, ffi_call_t call, sint32 arg_count);

/* eval */
cell_ptr_t	eval(state_t* s);

#ifdef __cplusplus
}
#endif

#endif /* SCHIZO_H */
