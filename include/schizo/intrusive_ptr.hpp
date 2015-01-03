#ifndef INTRUSIVE_PTR_HPP
#define INTRUSIVE_PTR_HPP
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
namespace schizo {
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
}	// namespace schizo
#endif // INTRUSIVE_PTR_HPP
