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

#ifndef FTL_ARRAY_HPP
#define FTL_ARRAY_HPP

namespace ftl {

template<typename T>
struct array {
	explicit array() : __arr(nullptr), __fast_array(nullptr), __fast_size(0)	{}

	array(const array<T>& other) : __arr(other.__arr), __fast_array(other.__fast_array), __fast_size(other.__fast_size)	{}

	explicit array(size_t size, const T* arr) : __arr(new node(size, arr)), __fast_array(nullptr), __fast_size(size) { if(size) __fast_array = __arr->value(); }

	explicit array(size_t size, rfunction<T(size_t)> build) {
		T*	arr	= static_cast<T*>(malloc(sizeof(T) * size));
		for( size_t i = 0; i < size; ++i ) {
			arr[i]	= build(i);
		}
		__arr	= new node(size, arr, TAKE);

		if( size ) __fast_array	= __arr->value();
		else	__fast_array	= nullptr;

		__fast_size	= size;
	}

	explicit array(size_t size, const T& t) {
		T*	arr	= static_cast<T*>(malloc(sizeof(T) * size));
		for( size_t i = 0; i < size; ++i ) {
			new(&(arr[i])) T(t);
		}
		__arr	= new node(size, arr, TAKE);

		if( size ) __fast_array	= __arr->value();
		else	__fast_array	= nullptr;

		__fast_size	= size;
	}


	//	array<T>&		operator = (const array<T>& other) {
	//					__arr		= other.__arr;
	//					__fast_array	= other.__fast_array;
	//					__fast_size	= other.__fast_size;
	//					return *this;
	//				}

	const T*		get() const { return __fast_array; }

	const T&		operator[] (size_t i) const	{
		assert( (__fast_array && i < __fast_size) && "index out of range");
		return __fast_array[i];
	}

	size_t			size() const			{ return __fast_size; }

	array<T>		set(size_t idx, const T& val) const {
		assert(( __fast_array && idx < __fast_size ) && "index out of range");
		auto data	= static_cast<T*>(alloc(__fast_size * sizeof(T)));
		for( size_t i = 0; i < idx; ++i )
			new(&data[i]) T(__fast_array[i]);
		new(&data[idx]) T(val);
		for( size_t i = idx + 1; i < __fast_size; ++i )
			new(&data[i]) T(__fast_array[i]);

		return array<T>(new node(__fast_size, data, TAKE));
	}

	void			iter(rfunction<void(const T&)> f) const {
		if( !__fast_array )
			return;

		for( size_t i = 0; i < __fast_size; ++i )
			f(__fast_array[i]);
	}

	template<typename R>
	array<R>		map(rfunction<R(const T&)> f) const {
		if( !__fast_size )
			return array<R>();

		auto data	= static_cast<R*>(raja_alloc(__fast_size * sizeof(R)));
		for( size_t i = 0; i < __fast_size; ++i )
			new(&data[i]) R(f(__fast_array[i]));

		return array<R>(new typename array<R>::node(__fast_size, data, array<R>::TAKE));
	}

	template<typename R>
	R			fold(rfunction<R(const R&, const T&)> f, const R& acc) const {
		if( !__fast_size )
			return acc;

		R	tmp(acc);
		for( size_t i = 0; i < __fast_size; ++i )
			tmp	= f(tmp, __fast_array[i]);
		return tmp;
	}

	array<T>		rev() const {
		if( !__fast_size )
			return array<T>();

		auto data	= static_cast<T*>(raja_alloc(__fast_size * sizeof(T)));
		for( size_t i = 0; i < __fast_size; ++i )
			new(&data[i]) T(__fast_array[__fast_size - 1 - i]);

		return array<T>(new node(__fast_size, data, TAKE));
	}

	array<T>		clone() const {
		if( !__fast_size )
			return array<T>();

		auto data	= static_cast<T*>(raja_alloc(__fast_size * sizeof(T)));
		for( size_t i = 0; i < __fast_size; ++i )
			new(&data[i]) T(__fast_array[i]);

		return array<T>(new node(__fast_size, data, TAKE));
	}

	///
	/// accessor (get/set)
	///
	struct accessor {
		size_t			size() const	{ return __len; }
		const T*		get() const	{ return __buff; }
		T&			operator [] (size_t idx) {
			assert( idx < __len && "array: index out of range");

			return __buff[idx];
		}

		const T&		operator [] (size_t idx) const {
			assert( idx >= __len && "array: index out of range");

			return __buff[idx];
		}

	private:
		accessor(const array<T>& a) {
			const T*	old = a.get();
			__len		= a.size();
			__buff	= static_cast<T*>(malloc(__len * sizeof(T)));
			for( size_t i = 0; i < __len; ++i )
				new(&__buff[i]) T(old[i]);
		}

		T*			__buff;
		size_t			__len;

		friend struct array;
	};

	array<T>		transform(rfunction<void(accessor&)> f) const {
		accessor	ax(*this);
		f(ax);
		return array<T>(new node(ax.size(), ax.get(), TAKE));
	}


protected:
	enum OWNERSHIP {
		COPY,
		TAKE,
	};

	struct node final : public noncopyable {
		typedef intrusive_ptr<const node> const_iptr;
		explicit node(size_t size, const T* data, OWNERSHIP osh = COPY) : count__(0), __size(size), __data(nullptr) {
			switch(osh) {
			case OWNERSHIP::COPY:
				//printf("array created of type: %s\n", typeid(T).name());
				if( size ) {
					__data	= static_cast<T*>(raja_alloc(__size * sizeof(T)));
					for( size_t i = 0; i < __size; ++i )
						new(&__data[i]) T(data[i]);
				}
				break;

			case OWNERSHIP::TAKE:
				__data	= const_cast<T*>(data);
				break;
			}
		}

		~node()			{ for(size_t i = 0; i < __size; ++i ) __data[i].~T(); free(__data); /*printf("array deleted of type: %s\n", typeid(T).name());*/ }
		const T*		value() const	{ return __data; }
		size_t			size() const	{ return __size; }

		friend inline void	intrusive_ptr_add_ref(node* p) { ++p->count__; }
		friend inline void	intrusive_ptr_release(node* p) { --p->count__; if(p->count == 0) delete p; }

	protected:
		mutable size_t		count__;
		size_t			__size;
		T*			__data;
	};

	explicit array(typename node::const_optr n) : __arr(n), __fast_array(n->value()), __fast_size(n->size())	{}

	typename node::const_optr	__arr;
	const T*		__fast_array;	///< fast accessor (optim tweek)
	size_t			__fast_size;	///< fast size copy (optim tweek)

	template<typename U>
	friend struct		array;

	template<typename U, size_t CS>
	friend struct		cluster_array;
};
}	// namespace ftl
#endif // FTL_ARRAY_HPP
