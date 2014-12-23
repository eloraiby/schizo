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

namespace schizo {

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

//
//  intrusive_ptr.hpp
//
//  Copyright (c) 2001, 2002 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/intrusive_ptr.html for documentation.
//
//
//  intrusive_ptr
//
//  A smart pointer that uses intrusive reference counting.
//
//  Relies on unqualified calls to
//
//      void intrusive_ptr_add_ref(T * p);
//      void intrusive_ptr_release(T * p);
//
//          (p != 0)
//
//  The object is responsible for destroying itself.
//

namespace raja
{
template<class T>
class intrusive_ptr
{
private:

	typedef intrusive_ptr this_type;

public:

	typedef T element_type;

	intrusive_ptr(): px( 0 )
	{
	}

	intrusive_ptr( T * p, bool add_ref = true ): px( p )
	{
		if( px != 0 && add_ref ) intrusive_ptr_add_ref( px );
	}

	template<class U>
	intrusive_ptr( intrusive_ptr<U> const & rhs )
		: px( rhs.get() )
	{
		if( px != 0 ) intrusive_ptr_add_ref( px );
	}

	intrusive_ptr(intrusive_ptr const & rhs): px( rhs.px )
	{
		if( px != 0 ) intrusive_ptr_add_ref( px );
	}

	~intrusive_ptr()
	{
		if( px != 0 ) intrusive_ptr_release( px );
	}

	template<class U> intrusive_ptr & operator=(intrusive_ptr<U> const & rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}

	// Move support

//	intrusive_ptr(intrusive_ptr && rhs): px( rhs.px )
//	{
//		rhs.px = 0;
//	}

//	intrusive_ptr & operator=(intrusive_ptr && rhs)
//	{
//		this_type( static_cast< intrusive_ptr && >( rhs ) ).swap(*this);
//		return *this;
//	}

	intrusive_ptr & operator=(intrusive_ptr const & rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}

	intrusive_ptr & operator=(T * rhs)
	{
		this_type(rhs).swap(*this);
		return *this;
	}

	void reset()
	{
		this_type().swap( *this );
	}

	void reset( T * rhs )
	{
		this_type( rhs ).swap( *this );
	}

	T * get() const
	{
		return px;
	}

	T & operator*() const
	{
		if( px == nullptr )
			throw "pointer is null";
		return *px;
	}

	T * operator->() const
	{
		if( px == nullptr )
			throw "pointer is null";
		return px;
	}

	typedef T * this_type::*unspecified_bool_type;

	operator unspecified_bool_type() const // never throws
	{
		return px == 0? 0: &this_type::px;
	}

	// operator! is redundant, but some compilers need it
	bool operator! () const // never throws
	{
		return px == 0;
	}

	void swap(intrusive_ptr & rhs)
	{
		T * tmp = px;
		px = rhs.px;
		rhs.px = tmp;
	}

// will create a pointer ambiguous
//	operator T*()
//	{
//		return px;
//	}

private:

	T * px;
};

template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
	return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
{
	return a.get() != b.get();
}

template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, U * b)
{
	return a.get() == b;
}

template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, U * b)
{
	return a.get() != b;
}

template<class T, class U> inline bool operator==(T * a, intrusive_ptr<U> const & b)
{
	return a == b.get();
}

template<class T, class U> inline bool operator!=(T * a, intrusive_ptr<U> const & b)
{
	return a != b.get();
}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

// Resolve the ambiguity between our op!= and the one in rel_ops

template<class T> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
{
	return a.get() != b.get();
}

#endif

template<class T> inline bool operator<(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
{
	return static_cast<size_t>(a.get()) <  static_cast<size_t>(b.get());
}

template<class T> void swap(intrusive_ptr<T> & lhs, intrusive_ptr<T> & rhs)
{
	lhs.swap(rhs);
}

// mem_fn support

template<class T> T * get_pointer(intrusive_ptr<T> const & p)
{
	return p.get();
}

template<class T, class U> intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const & p)
{
	return static_cast<T *>(p.get());
}

template<class T, class U> intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const & p)
{
	return const_cast<T *>(p.get());
}

template<class T, class U> intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const & p)
{
	return dynamic_cast<T *>(p.get());
}


struct state;
struct cell;

struct cell {
	typedef intrusive_ptr<cell>	iptr;

	uint32		type() const		{ return type_; }

			cell(uint32 type) : ref_count_(0), type_(type)	{}
	virtual		~cell()	= 0;

	void* operator	new(size_t s)		{ return malloc(s); }
	void operator	delete(void* p)		{ free(p); }

	inline size_t	get_ref_count() const	{ return ref_count_; }

	friend inline void	intrusive_ptr_add_ref(cell* p) {
		// increment reference count of object *p
		++(p->ref_count_);
	}

	friend inline void	intrusive_ptr_release(cell* p) {
		// decrement reference count, and delete object when reference count reaches 0
		if( --(p->ref_count_) == 0 )
			delete p;
	}

	friend inline void	intrusive_decrement_ref_count(cell* p) {
		--p->ref_count_;
	}

protected:
	uint32		ref_count_;
	uint32		type_;
};	// cell

// string
struct string_cell : public cell {
	string_cell(const char* str) : cell(ATOM_STRING), string_(static_cast<char*>(malloc(strlen(str))))	{ memcpy(string_, str, strlen(str)); }
	virtual		~string_cell() override	{ free(string_); }

	const char*	value() const		{ return string_; }

protected:
	char*		string_;
};

struct bool_cell : public cell {
	bool_cell(bool b) : cell(ATOM_BOOL), b_(b)	{}
	virtual		~bool_cell() override	{}

	bool		value() const		{ return b_; }

protected:
	bool		b_;
};

struct char_cell : public cell {
	char_cell(char ch) : cell(ATOM_CHAR), ch_(ch)	{}
	virtual		~char_cell() override	{}

	bool		value() const		{ return ch_; }

protected:
	bool		ch_;
};

// symbol
struct symbol : public cell {
	symbol(const char* sym) : cell(ATOM_SYMBOL), sym_(static_cast<char*>(malloc(strlen(sym))))	{ memcpy(sym_, sym, strlen(sym)); }
	virtual		~symbol() override	{ free(sym_); }

	const char*	value() const		{ return sym_; }

private:
	char*		sym_;
};

// lists
struct pair : public cell {
	pair(iptr head, iptr tail) : cell(CELL_PAIR), head_(head), tail_(tail)	{}
	virtual		~pair() override	{}

	iptr		head() const		{ return head_; }
	iptr		tail() const		{ return tail_; }

private:
	iptr		head_;		/* the head */
	iptr		tail_;		/* the tail */
};

// lambda
struct lambda : public cell {
	iptr		syms;		/* arguments */
	iptr		body;		/* closure body */
};

// foreign function
typedef void (*ffi_call_t)(state* s);

struct ffi : public cell {
	uint16		flags;
	sint16		arg_count;	/* total argument count, -1 = any */
	ffi_call_t	proc;		/* the procedure */
};

// closure
struct closure : public cell {
	iptr		env;		/* captured environment */
	iptr		lambda;		/* original args and body pair */
};


cell(sint64 i) : ref_count_(0), type_(ATOM_SINT64), obj_(i)	{}
cell(real64 r) : ref_count_(0), type_(ATOM_REAL64), obj_(r)	{}
cell(lambda l) : ref_count_(0), type_(CELL_LAMBDA), obj_(l)	{}
cell(iptr bindings) : ref_count_(0), type_(OP_BIND), obj_(bindings)	{}
cell(closure cl) : ref_count_(0), type_(OP_APPLY_CLOSURE), obj_(cl)	{}
cell(ffi fn) : ref_count_(0), type_(OP_APPLY_FFI), obj_(fn)	{}


#define EVAL_ARGS	0x8000		/* arguments are evaluated before call */
#define ARGS_ANY	-1		/* any number of argument */

struct state {
	struct {
		cell::iptr	exp_stack;	/* expression/operator to execute */
		cell::iptr	env_stack;	/* environment stack */

		/* volatile registers */
		cell::iptr	current_env;	/* current environment */
		cell::iptr	current_exp;	/* current exp + args */
		cell::iptr	ret_val;	/* return value */
	} registers;

	struct {
		cell::iptr	root;		/* root cell */
		cell::iptr	current_cell;
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

static INLINE cell::iptr	list_head(cell::iptr l) { assert(l->type() == CELL_PAIR || l->type() == CELL_FREE); return l->get_pair().head; }
static INLINE cell::iptr	list_tail(cell::iptr l) { assert(l->type() == CELL_PAIR || l->type() == CELL_FREE); return l->get_pair().tail; }


/* printing */
void		print_cell(cell::iptr c, uint32 level);

/* atoms */
cell::iptr	error(const char* error);

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

}	// namespace schizo

#endif /* SCHIZO_H */
