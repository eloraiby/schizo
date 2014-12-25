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

template<class T>
class intrusive_ptr
{
private:

	typedef intrusive_ptr this_type;

public:

	typedef T element_type;

	intrusive_ptr(): px( 0 )	{}
	intrusive_ptr( T * p, bool add_ref = true ): px( p )	{	if( px != 0 && add_ref ) intrusive_ptr_add_ref( px );	}

	template<class U>
	intrusive_ptr( intrusive_ptr<U> const & rhs ) : px( rhs.get() )	{	if( px != 0 ) intrusive_ptr_add_ref( px );	}
	intrusive_ptr(intrusive_ptr const & rhs): px( rhs.px )	{	if( px != 0 ) intrusive_ptr_add_ref( px );		}

	~intrusive_ptr()					{	if( px != 0 ) intrusive_ptr_release( px );		}

	template<class U>
	intrusive_ptr&	operator=(intrusive_ptr<U> const & rhs)	{
		this_type(rhs).swap(*this);
		return *this;
	}

	// Move support
	intrusive_ptr(intrusive_ptr && rhs): px( rhs.px )	{	rhs.px = 0;	}
	intrusive_ptr&	operator=(intrusive_ptr && rhs)	{
		this_type( static_cast< intrusive_ptr && >( rhs ) ).swap(*this);
		return *this;
	}

	intrusive_ptr&	operator=(intrusive_ptr const & rhs)	{	this_type(rhs).swap(*this); return *this;		}
	intrusive_ptr&	operator=(T * rhs)			{	this_type(rhs).swap(*this); return *this;		}

	void		reset()					{	this_type().swap( *this );				}
	void		reset( T * rhs )			{	this_type( rhs ).swap( *this );				}

	T*		get() const				{	return px;						}
	T&		operator*() const {
		/* if( px == nullptr )
			throw "pointer is null"; */
		return *px;
	}

	T*		operator->() const {
		/* if( px == nullptr )
			throw "pointer is null"; */
		return px;
	}

	typedef T* this_type::*unspecified_bool_type;

	operator unspecified_bool_type() const { // never throws
		return px == 0? 0: &this_type::px;
	}

	// operator! is redundant, but some compilers need it
	bool		operator! () const { // never throws
		return px == 0;
	}

	void		swap(intrusive_ptr & rhs) {
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

template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b) {	return a.get() == b.get();	}
template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b) {	return a.get() != b.get();	}
template<class T, class U> inline bool operator==(intrusive_ptr<T> const & a, U * b)		{	return a.get() == b;	}
template<class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, U * b)		{	return a.get() != b;	}
template<class T, class U> inline bool operator==(T * a, intrusive_ptr<U> const & b)		{	return a == b.get();	}
template<class T, class U> inline bool operator!=(T * a, intrusive_ptr<U> const & b)		{	return a != b.get();	}

#if __GNUC__ == 2 && __GNUC_MINOR__ <= 96

// Resolve the ambiguity between our op!= and the one in rel_ops

template<class T> inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b) {	return a.get() != b.get();	}

#endif

template<class T> inline bool operator<(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)	{	return static_cast<size_t>(a.get()) <  static_cast<size_t>(b.get());	}
template<class T> void swap(intrusive_ptr<T> & lhs, intrusive_ptr<T> & rhs)			{	lhs.swap(rhs);	}

// mem_fn support
template<class T> T * get_pointer(intrusive_ptr<T> const & p)	{	return p.get(); }
template<class T, class U> intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const & p)	{	return static_cast<T *>(p.get());	}
template<class T, class U> intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const & p)	{	return const_cast<T *>(p.get());	}
template<class T, class U> intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const & p)	{	return dynamic_cast<T *>(p.get());	}


struct state;
struct cell;

struct cell {
	typedef intrusive_ptr<cell>	iptr;

	static bool	clear_to_destroy;

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
		if( --(p->ref_count_) == 0 ) {
			assert(clear_to_destroy == true);
			delete p;
		}
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

// error
struct error : public cell {
	error(const char* str) : cell(ATOM_ERROR), string_(static_cast<char*>(malloc(strlen(str) + 1)))	{ memcpy(string_, str, strlen(str) + 1); }
	virtual		~error() override	{ free(string_); }

	const char*	value() const		{ return string_; }

protected:
	char*		string_;
};

// bool
struct bool_cell : public cell {
	bool_cell(bool b) : cell(ATOM_BOOL), b_(b)	{}
	virtual		~bool_cell() override	{}

	bool		value() const		{ return b_; }

protected:
	bool		b_;
};

// char
struct char_cell : public cell {
	char_cell(char ch) : cell(ATOM_CHAR), ch_(ch)	{}
	virtual		~char_cell() override	{}

	char		value() const		{ return ch_; }

protected:
	char		ch_;
};

// sint64
struct sint64_cell : public cell {
	sint64_cell(sint64 s64) : cell(ATOM_SINT64), s64_(s64)	{}
	virtual		~sint64_cell() override	{}

	sint64		value() const		{ return s64_; }

protected:
	sint64		s64_;
};

// real64
struct real64_cell : public cell {
	real64_cell(real64 r64) : cell(ATOM_REAL64), r64_(r64)	{}
	virtual		~real64_cell() override	{}

	real64		value() const		{ return r64_; }

protected:
	real64		r64_;
};

// symbol
struct symbol : public cell {
	symbol(const char* sym) : cell(ATOM_SYMBOL), sym_(static_cast<char*>(malloc(strlen(sym) + 1)))	{ memcpy(sym_, sym, strlen(sym) + 1); }
	virtual		~symbol() override	{ free(sym_); }

	const char*	value() const		{ return sym_; }

private:
	char*		sym_;
};

// list
struct list : public cell {
	list(iptr head, iptr tail) : cell(CELL_LIST), head_(head), tail_(tail)	{}
	virtual		~list() override	{}

	iptr		head() const		{ return head_; }
	iptr		tail() const		{ return tail_; }

	static iptr	reverse(iptr l);
	static uint32	length(iptr l);

	static INLINE cell::iptr	head(cell::iptr l) { assert(l->type() == CELL_LIST); return static_cast<list*>(l.get())->head(); }
	static INLINE cell::iptr	tail(cell::iptr l) { assert(l->type() == CELL_LIST); return static_cast<list*>(l.get())->tail(); }
private:
	iptr		head_;			///< the head
	iptr		tail_;			///< the tail
};

// lambda
struct lambda : public cell {
	lambda(iptr syms, iptr body) : cell(CELL_LAMBDA), syms_(syms), body_(body)	{}
	virtual		~lambda() override	{}

	iptr		syms() const		{ return syms_; }
	iptr		body() const		{ return body_; }

private:
	iptr		syms_;			///< arguments
	iptr		body_;			///< closure body
};

// foreign function
typedef cell::iptr (*ffi_call_t)(cell::iptr env, cell::iptr args);

struct ffi : public cell {
	inline		ffi(uint16 flags, sint16 arg_count, ffi_call_t proc) : cell(CELL_FFI), flags_(flags), arg_count_(arg_count), proc_(proc)	{}
	virtual		~ffi() override		{}

	inline uint16	flags() const		{ return flags_; }
	inline sint16	arg_count() const	{ return arg_count_; }
	inline iptr	operator()(iptr env, iptr args) const		{ return proc_(env, args); }

private:
	uint16		flags_;
	sint16		arg_count_;		///< total argument count, -1 = any
	ffi_call_t	proc_;			///< the procedure
};

// closure
struct closure : public cell {
	closure(iptr env, iptr lambda) : cell(CELL_CLOSURE), env_(env), lambda_(lambda)	{}
	virtual		~closure() override	{}

	inline iptr	env() const		{ return env_;		}
	inline iptr	lambda() const		{ return lambda_;	}
private:
	iptr		env_;			///< captured environment
	iptr		lambda_;		///< original args and body pair
};

// bind
struct bind : public cell {
	bind(iptr bindings) : cell(CELL_BIND), bindings_(bindings)	{}
	virtual		~bind() override	{}

	inline iptr	bindings() const	{ return bindings_; }

private:
	iptr		bindings_;		///< binding list
};


#define EVAL_ARGS	0x8000		/* arguments are evaluated before call */
#define ARGS_ANY	-1		/* any number of argument */

struct state : public cell {

	state();
	virtual		~state() override;

	inline iptr	root() const		{ return parser_.root; }
	inline void	set_root(iptr root)	{ parser_.root = root; }

	static iptr	eval(iptr env, iptr exp);

	/// parser.y / lexer.rl
	static void	parse(state* state, const char* str);

	static iptr	default_env();
	static iptr	add_ffi(cell::iptr env, const char* sym, uint16 flags, sint32 arg_count, ffi_call_t proc);
	static iptr	lookup(cell::iptr env, const char* sym);

	static inline cell* add_token(state* s, cell* c) {
		s->parser_.token_list	= new list(c, s->parser_.token_list);
		return c;
	}

protected:

	static iptr	eval_list(iptr env, iptr l);

//	inline void	push_env(iptr env)	{ registers_.env_stack = new list(env, registers_.env_stack);	}
//	inline void	pop_env()		{ registers_.env_stack = list::tail(registers_.env_stack);	}
//	inline iptr	top_env()		{ return list::head(registers_.env_stack);			}

	inline void	push_exp(iptr exp)	{ registers_.exp_stack = new list(exp, registers_.exp_stack);	}
	inline void	pop_exp()		{ registers_.exp_stack = list::tail(registers_.exp_stack);	}
	inline iptr	top_exp()		{ return list::head(registers_.exp_stack);			}

	static iptr	apply_bind(iptr env, iptr bexp);

	struct {
		cell::iptr	exp_stack;	///< expression/operator to execute
		//cell::iptr	env_stack;	///< environment stack

		/* volatile registers */
		//cell::iptr	current_env;	///< current environment
	} registers_;

	struct {
		cell::iptr	token_list;
		cell::iptr	root;		///< root cell
		const char*	token_start;
		const char*	token_end;
		uint32		token_line;
	} parser_;
};

/* printing */
void		print_cell(cell::iptr c, uint32 level);

/* lists */
#define		pair(fst, snd)	(new list((fst), new list((snd), nullptr)))



}	// namespace schizo

#endif /* SCHIZO_H */
