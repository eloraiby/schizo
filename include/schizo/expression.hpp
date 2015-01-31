#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP
/*
  Schizo programming language
  Copyright (C) 2014-2015  Wael El Oraiby

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

#include "../ftl/function.hpp"

namespace schizo {
////////////////////////////////////////////////////////////////////////////////
/// \brief expression
////////////////////////////////////////////////////////////////////////////////
struct exp {
	typedef ftl::intrusive_ptr<exp>	iptr;

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
		EXP_FFI,
		EXP_SPECIAL_FORM,
	};

	TYPE		type() const		{ return type_; }

			exp(TYPE type) : ref_count_(0), type_(type)	{}
	virtual		~exp()	= 0;

	void* operator	new(size_t s)		{ return malloc(s); }
	void operator	delete(void* p)		{ free(p); }

	virtual uint32	get_ref_count() const;
	virtual void	inc_ref_count();
	virtual void	dec_ref_count();

	friend inline void	intrusive_ptr_add_ref(exp* p) { p->inc_ref_count(); }
	friend inline void	intrusive_ptr_release(exp* p) { p->dec_ref_count(); }

	static void	print(exp::iptr c, uint32 level);
	static void	print_env(exp::iptr env);

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
	struct env_ret {
		env_ret() : env_(nullptr), ret_(nullptr)	{}
		env_ret(exp::iptr env, exp::iptr ret) : env_(env), ret_(ret)	{}

		inline exp::iptr	env() const	{ return env_;	}
		inline exp::iptr	ret() const	{ return ret_;	}

	private:
		exp::iptr	env_;
		exp::iptr	ret_;
	};

	typedef ftl::result<env_ret>	val;
	typedef val	(*call)(exp::iptr env, exp::iptr args);

	inline		special(sint32 arg_count, call proc) : exp(EXP_SPECIAL_FORM), arg_count_(arg_count), proc_(proc)	{}
	virtual		~special() override		{}

	inline sint32	arg_count() const	{ return arg_count_; }
	inline val	operator()(iptr env, iptr args) const		{ return proc_(env, args); }

private:
	sint32		arg_count_;		///< total argument count, -1 = any
	call		proc_;			///< the procedure
};

#define ENV_RET	::schizo::exp::special::env_ret
#define SP_VAL ::schizo::exp::special::val

}	// namespace schizo
#endif // EXPRESSION_HPP
