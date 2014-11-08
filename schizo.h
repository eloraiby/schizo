#ifndef CELL_H
#define CELL_H

#include "parser.h"
#include <stdint.h>

#define INLINE __inline

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


typedef uint16		CELL_TYPE;


struct cons_t;
struct closure_t;
struct cell_t;

typedef struct state_t	state_t;

typedef struct cell_id_t {
	uint32		index;
} cell_id_t;

static INLINE cell_id_t
cell_id(uint32 id) {
	cell_id_t	ret = { id };
	return ret;
}

#define NIL_CELL	0x0
#define is_nil(c)	(c.index == NIL_CELL)

static INLINE cell_id_t cell_nil() {
	cell_id_t	id = { 0 };
	return id;
}

typedef struct cons_t {
	cell_id_t	cdr;		/* the tail */
	cell_id_t	car;		/* the head */
} cons_t;

typedef struct closure_t {
	uint32		arg_count;	/* total argument count */
	cell_id_t	expression;
} closure_t;

#define GC_REACHABLE	0x8000
#define FOREIGN		0x0800
#define REDIRECTOR	0x0400

typedef struct cell_t {
	uint16		flags;
	CELL_TYPE	type;

	union {
		char*		symbol;

		bool		boolean;

		char		ch;

		sint64		s64;
		real64		r64;

		char*		string;
		cons_t		cons;
		closure_t	closure;
	} object;

} cell_t;

struct state_t {
	struct gc_block_t {
		cell_t*		cells;
		uint32		count;
		uint32		free_count;
		cell_id_t	free_list;
	} gc_block;

	cell_id_t	root;
	cell_id_t	current_cell;
	const char*	token_start;
	const char*	token_end;
	uint32		token_line;
};

static INLINE cell_t*
cell_from_index(state_t* s,
		cell_id_t idx)
{
	return &s->gc_block.cells[idx.index];
}

/* atoms */
cell_id_t	cell_new_symbol(state_t* s, const char* sym);
cell_id_t	cell_new_boolean(state_t* s, bool b);
cell_id_t	cell_new_char(state_t* s, char c);
cell_id_t	cell_new_sint64(state_t* s, sint64 i);
cell_id_t	cell_new_real64(state_t* s, real64 i);
cell_id_t	cell_new_string(state_t* s, const char* b);

cell_id_t	index_from_cell(state_t* s, cell_t* c);

cell_id_t	cell_new_cons(state_t* s, cell_id_t car);
cell_id_t	cell_new_application(state_t* s, cell_t* app, cell_t* args);

cell_id_t	cell_cons(state_t* s, cell_id_t car, cell_id_t list);
cell_id_t	cell_reverse_in_place(state_t* s, cell_id_t list);
cell_id_t	cell_get_head(state_t*s, cell_id_t tail);

cell_id_t	cell_car(state_t* s, cell_id_t list);
cell_id_t	cell_cdr(state_t* s, cell_id_t list);
cell_id_t	cell_array(state_t* s, uint32 count);


state_t*	parse(const char* str);

void		print_cell(state_t* s, cell_id_t c, uint32 level);
#ifdef __cplusplus
}
#endif

#endif /* CELL_H */
