#ifndef CELL_H
#define CELL_H
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
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16		CELL_TYPE;


struct cons_t;
struct closure_t;
struct cell_t;

typedef struct state_t	state_t;

/* C doesn't have units of measure, fake one */
typedef struct cell_id_t {
	uint32		index;
} cell_id_t;

/* lists */
typedef struct pair_t {
	cell_id_t	tail;		/* the tail */
	cell_id_t	head;		/* the head */
} pair_t;

/* lambda */
typedef struct closure_t {
	cell_id_t	syms;		/* arguments */
	cell_id_t	body;		/* closure body */
} closure_t;

/* foreign function */
typedef struct ffi_t {
	uint32		arg_count;	/* total argument count, -1 = any */
	cell_id_t	symbol;
} ffi_t;

/* scope or environment */
typedef struct environment_t {
	cell_id_t	parent;		/* parent environment */
	cell_id_t	sym_bindings;	/* list of pairs (symbol * binding) */
} environment_t;

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
		cell_id_t	op;		/* current operation pointer */
		cell_id_t	stack_top;	/* frame stack */
	} exec_environment;

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


/* cell.c */
cell_id_t	cell_alloc(state_t* s);
void		print_cell(state_t* s, cell_id_t c, uint32 level);

/* atom.c */
cell_id_t	atom_new_symbol(state_t* s, const char* sym);
cell_id_t	atom_new_boolean(state_t* s, bool b);
cell_id_t	atom_new_char(state_t* s, char c);
cell_id_t	atom_new_sint64(state_t* s, sint64 i);
cell_id_t	atom_new_real64(state_t* s, real64 i);
cell_id_t	atom_new_string(state_t* s, const char* b);
cell_id_t	atom_new_unary_op(state_t* s, const char* op);
cell_id_t	atom_new_binary_op(state_t* s, const char* op);

/* list.c */
cell_id_t	list_new(state_t* s, cell_id_t head);
cell_id_t	list_cons(state_t* s, cell_id_t head, cell_id_t tail);
#define		like_make_pair(s, fst, snd)	(list_cons((s), (fst), list_new((s), (snd))))

cell_id_t	list_reverse_in_place(state_t* s, cell_id_t list);
uint32		list_length(state_t* s, cell_id_t list);

/* vectors */
cell_id_t	cell_vector(state_t* s, uint32 count);

/* parser.y / lexer.rl */
state_t*	parse(const char* str);

/* environment.c */
cell_id_t	env_new(state_t* s, cell_id_t parent);
cell_id_t	env_add_symbol(state_t* s, cell_id_t env, cell_id_t sym);

#ifdef __cplusplus
}
#endif

#endif /* CELL_H */
