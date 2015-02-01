#ifndef FTL_FUNCTION_HPP
#define FTL_FUNCTION_HPP
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

#include "platform.hpp"
#include "noncopyable.hpp"
#include "intrusive_ptr.hpp"

#define const_lambda [=]

namespace ftl {
template <typename T>
struct rfunction;

template <typename Result, typename... Args>
struct rfunction<Result(Args...)> {
private:
	// this is the bit that will erase the actual type
	struct concept {
		inline concept() : count_(0)				{}
		virtual ~concept()					{}
		virtual Result operator()(Args...) const		= 0;

		friend inline void	intrusive_ptr_add_ref(concept* p)	{ ++p->count_;		}
		friend inline void	intrusive_ptr_release(concept* p)	{ --p->count_; if( p->count_ == 0 ) { delete p; } }

		mutable size_t		count_;

	};

	// this template provides us derived classes from `concept`
	// that can store and invoke op() for any type
	template <typename T>
	struct model : concept {
		model(const T& u) : t(u) {}

		Result operator()(Args... a) const override {
			return t(a...);
		}

		T t;
	};


	// this is the actual storage
	// note how the `model<?>` type is not used here
	intrusive_ptr<concept> fn_;

public:
	// construct a `model<T>`, but store it as a pointer to `concept`
	// this is where the erasure "happens"
	rfunction() {}

	template <typename T>
	rfunction(const T& t) : fn_(new model<T>(t)) {}

	// do the virtual call
	Result operator()(Args... args) const {
		return fn_->operator ()(args...);
	}
};

}	// namespace ftl

#include "array.hpp"
#include "string.hpp"
#include "list.hpp"
#include "thunk.hpp"

#endif // FTL_FUNCTION_HPP
