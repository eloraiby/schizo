#ifndef FTL_FUNCTION_HPP
#	include "function.hpp"
#endif

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

#ifndef FTL_LIST_HPP
#define FTL_LIST_HPP

namespace ftl {
template<class T>
struct list {
	explicit list() : __head(nullptr)	{}
	explicit list(const list<T>& other) : __head(other.__head)	{}
	list<T>			operator = (const list<T>& other)	{ __head = other.__head; return *this; }


	list<T>			push_front(const T& v) const		{ return list(new node(v, __head)); }

	list<T>			pop_front() const {
					assert( __head && "poping an empty list");
					return list(__head->tail());
				}

	const T&		head() const {
					assert( __head && "poping an empty list");
					return __head->value();
				}

	list<T>			tail() const {
					assert( __head && "poping an empty list");
					return list(__head->tail());
				}

	/// return true if the list is empty
	bool			empty() const	{ return __head == NULL; }


	void			iter(rfunction<void(const T&)> f) const {
					typename node::const_optr	n(__head);
					while( n ) {
						f(n->value());
						n	= n->tail();
					}
				}

	template<typename R>
	list<R>			map(rfunction<R(const T&)> f) const {
					typename node::const_optr	n(__head);
					typename list<R>::node::const_optr	r(nullptr);
					while( n ) {
						auto res	= f(n->value());
						r	= new typename list<R>::node(res, r);
						n	= n->tail();
					}
					return list<R>(r);
				}

	template<typename R>
	R			fold(rfunction<R(const R&, const T&)> f, const R& acc) const {
					typename node::const_optr	n(__head);
					R			r	= acc;
					while( n ) {
						r	= f(r, n->value());
						n	= n->tail();
					}
					return r;
				}

	list<T>			rev() const {
					typename node::const_optr	n = __head;
					typename node::const_optr	r(nullptr);
					while( n ) {
						r	= new node(n->value(), r);
						n	= n->tail();
					}
					return list<T>(r);
				}


protected:

	struct node final : public noncopyable
	{
		typedef intrusive_ptr<const node> const_iptr;

		node(const T& data, const_iptr tail) : __data(data), __tail(tail)	{}
		const T&		value() const	{ return __data; }
		const_iptr		tail() const	{ return __tail; }

		friend inline void	intrusive_ptr_add_ref(const node* p) { ++p->count__; }
		friend inline void	intrusive_ptr_release(const node* p) { --p->count__; if( p->count__ == 0 ) delete p; }

	protected:
		mutable size_t		count__;
		T			__data;
		const_iptr		__tail;
	};

	list(typename node::const_iptr n) :	__head(n)	{}

	typename node::const_iptr		__head;

	template<typename U>
	friend struct		list;
};
}	// namespace ftl

#endif // RAJA_LIST_HPP
