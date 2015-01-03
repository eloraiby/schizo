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

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct state;
struct exp;

////////////////////////////////////////////////////////////////////////////////
/// \brief expression
////////////////////////////////////////////////////////////////////////////////
struct exp {
	typedef intrusive_ptr<exp>	iptr;

	struct error;
	struct string;
	struct boolean;
	struct character;
	struct sint64;
	struct real64;
	struct symbol;
	struct list;
	struct lambda;
	struct closure;
	struct ffi;
	struct special;
	struct bind;

	enum TYPE {
		EXP_ERROR,
		EXP_SYMBOL,
		EXP_BOOLEAN,
		EXP_CHARACTER,
		EXP_SINT64,
		EXP_REAL64,
		EXP_STRING,
		EXP_LIST,
		EXP_LAMBDA,
		EXP_CLOSURE,
		EXP_FFI,
		EXP_SPECIAL_FORM,
		EXP_BIND,
		EXP_STATE,
	};

	TYPE		type() const		{ return type_; }

			exp(TYPE type) : ref_count_(0), type_(type)	{}
	virtual		~exp()	= 0;

	void* operator	new(size_t s)		{ return malloc(s); }
	void operator	delete(void* p)		{ free(p); }

	inline size_t	get_ref_count() const	{ return ref_count_; }

	friend inline void	intrusive_ptr_add_ref(exp* p) {
		// increment reference count of object *p
		++(p->ref_count_);
	}

	friend inline void	intrusive_ptr_release(exp* p) {
		// decrement reference count, and delete object when reference count reaches 0
		if( --(p->ref_count_) == 0 ) {
			delete p;
		}
	}

	friend inline void	intrusive_decrement_ref_count(exp* p) {
		--p->ref_count_;
	}

protected:
	uint32		ref_count_;
	TYPE		type_;
};	// exp

// string
struct exp::string : public exp {
	string(const char* str) : exp(EXP_STRING), string_(static_cast<char*>(malloc(strlen(str) + 1)))	{ memcpy(string_, str, strlen(str) + 1); }
	virtual		~string() override	{ free(string_); }

	const char*	value() const		{ return string_; }

protected:
	char*		string_;
};

// error
struct exp::error : public exp {
	error(const char* str) : exp(EXP_ERROR), string_(static_cast<char*>(malloc(strlen(str) + 1)))	{ memcpy(string_, str, strlen(str) + 1); }
	virtual		~error() override	{ free(string_); }

	const char*	value() const		{ return string_; }

protected:
	char*		string_;
};

// bool
struct exp::boolean : public exp {
	boolean(bool b) : exp(EXP_BOOLEAN), b_(b)	{}
	virtual		~boolean() override	{}

	bool		value() const		{ return b_; }

protected:
	bool		b_;
};

// character
struct exp::character : public exp {
	character(char ch) : exp(EXP_CHARACTER), ch_(ch)	{}
	virtual		~character() override	{}

	char		value() const		{ return ch_; }

protected:
	char		ch_;
};

// sint64
struct exp::sint64 : public exp {
	sint64(::schizo::sint64 s64) : exp(EXP_SINT64), s64_(s64)	{}
	virtual		~sint64() override	{}

	::schizo::sint64	value() const		{ return s64_; }

protected:
	::schizo::sint64	s64_;
};

// real64
struct exp::real64 : public exp {
	real64(::schizo::real64 r64) : exp(EXP_REAL64), r64_(r64)	{}
	virtual		~real64() override	{}

	::schizo::real64	value() const		{ return r64_; }

protected:
	::schizo::real64	r64_;
};

///
/// @brief The exp::symbol struct
///
struct exp::symbol : public exp {
	symbol(const char* sym) : exp(EXP_SYMBOL), sym_(static_cast<char*>(malloc(strlen(sym) + 1)))	{ memcpy(sym_, sym, strlen(sym) + 1); }
	virtual		~symbol() override	{ free(sym_); }

	const char*	value() const		{ return sym_; }

private:
	char*		sym_;
};

///
/// @brief The exp::list struct
///
struct exp::list : public exp {
	list(iptr head, iptr tail) : exp(EXP_LIST), head_(head), tail_(tail)	{}
	virtual		~list() override	{}

	iptr		head() const		{ return head_; }
	iptr		tail() const		{ return tail_; }

	static iptr	reverse(iptr l);
	static uint32	length(iptr l);

	static INLINE exp::iptr	head(exp::iptr l) { assert(l->type() == EXP_LIST); return static_cast<list*>(l.get())->head(); }
	static INLINE exp::iptr	tail(exp::iptr l) { assert(l->type() == EXP_LIST); return static_cast<list*>(l.get())->tail(); }
private:
	iptr		head_;			///< the head
	iptr		tail_;			///< the tail
};

///
/// @brief The exp::lambda struct
///
struct exp::lambda : public exp {
	lambda(iptr syms, iptr body) : exp(EXP_LAMBDA), syms_(syms), body_(body)	{}
	virtual		~lambda() override	{}

	iptr		syms() const		{ return syms_; }
	iptr		body() const		{ return body_; }

private:
	iptr		syms_;			///< arguments
	iptr		body_;			///< closure body
};

///
/// @brief foreign function interface
///
struct exp::ffi : public exp {
	typedef exp::iptr (*call)(exp::iptr args);

	inline		ffi(sint32 arg_count, call proc) : exp(EXP_FFI), arg_count_(arg_count), proc_(proc)	{}
	virtual		~ffi() override		{}

	inline sint32	arg_count() const	{ return arg_count_; }
	inline iptr	operator()(iptr args) const		{ return proc_(args); }

private:
	sint32		arg_count_;		///< total argument count, -1 = any
	call		proc_;			///< the procedure
};

///
/// @brief special function
///
struct exp::special : public exp {
	typedef exp::iptr (*call)(exp::iptr env, exp::iptr args);

	inline		special(sint32 arg_count, call proc) : exp(EXP_SPECIAL_FORM), arg_count_(arg_count), proc_(proc)	{}
	virtual		~special() override		{}

	inline sint32	arg_count() const	{ return arg_count_; }
	inline iptr	operator()(iptr env, iptr args) const		{ return proc_(env, args); }

private:
	sint32		arg_count_;		///< total argument count, -1 = any
	call		proc_;			///< the procedure
};

///
/// @brief The exp::closure struct
///
struct exp::closure : public exp {
	closure(iptr env, iptr lambda) : exp(EXP_CLOSURE), env_(env), lambda_(lambda)	{}
	virtual		~closure() override	{}

	inline iptr	env() const		{ return env_;		}
	inline iptr	lambda() const		{ return lambda_;	}
private:
	iptr		env_;			///< captured environment
	iptr		lambda_;		///< original args and body pair
};

///
/// @brief The exp::bind struct
///
struct exp::bind : public exp {
	bind(iptr bindings) : exp(EXP_BIND), bindings_(bindings)	{}
	virtual		~bind() override	{}

	inline iptr	bindings() const	{ return bindings_; }

private:
	iptr		bindings_;		///< binding list
};

struct state : public exp {

	state();
	virtual		~state() override;

	inline iptr	root() const		{ return parser_.root; }
	inline void	set_root(iptr root)	{ parser_.root = root; }

	static iptr	eval(iptr env, iptr exp);

	/// parser.y / lexer.rl
	static void	parse(state* state, const char* str);

	static iptr	default_env();
	static iptr	add_ffi(exp::iptr env, const char* sym, sint32 arg_count, ffi::call proc);
	static iptr	add_special(exp::iptr env, const char* sym, sint32 arg_count, special::call proc);
	static iptr	lookup(exp::iptr env, const char* sym);

	static inline exp* add_token(state* s, exp* c) {
		s->parser_.token_list	= new list(c, s->parser_.token_list);
		return c;
	}

protected:

	static iptr	eval_list(iptr env, iptr l);

	static iptr	apply_bind(iptr env, iptr bexp);

	struct {
		exp::iptr	token_list;
		exp::iptr	root;		///< root exp
		const char*	token_start;
		const char*	token_end;
		uint32		token_line;
	} parser_;
};

/* printing */
void		print_cell(exp::iptr c, uint32 level);

/* lists */
static inline exp::iptr	pair(exp::iptr fst, exp::iptr snd) { return new exp::list((fst), new exp::list((snd), nullptr)); }



}	// namespace schizo

#endif /* SCHIZO_H */
