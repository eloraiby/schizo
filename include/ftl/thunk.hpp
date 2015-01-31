#ifndef FTL_FUNCTION_HPP
#	include "function.hpp"
#endif

#ifndef FTL_THUNK_HPP
#define FTL_THUNK_HPP

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

#define tailcall(a)	decltype(a)([=]()->decltype(a) { return a; })

#define caseof(a) const_lambda(a)

namespace ftl {

template<typename R>
struct thunk {

	enum TYPE {
		FINAL,
		DELAYED,
	};

	TYPE		get_type() const	{ return type__; }

	thunk(R r) : type__(FINAL), v__(r) {}
	thunk(rfunction<thunk<R>()> f) : type__(DELAYED), f__(f) {}

	thunk<R>	eval_once() {
		switch(type__) {
		case DELAYED:
			return f__();

		case FINAL:
			return *this;
		}
	}

	template<typename RM>
	RM	map(const rfunction<RM(R)>& lambda_res) {
		switch(type__) {
		case FINAL:
			return lambda_res(v__);

		case DELAYED:
			thunk<R>	r = f__();

			while( r.type__ == DELAYED ) {
				r	= r.f__();
			}

			v__	= r.v__;
			return lambda_res(r.v__);
		}
	}

	R		eval() {
		switch(type__) {
		case FINAL:
			return v__;

		case DELAYED:
			thunk<R>	r = f__();

			while( r.type__ == DELAYED ) {
				r	= r.f__();
			}

			v__	= r.v__;
			return v__;
		}
	}


private:
	TYPE		type__;

	R		v__;
	rfunction<thunk<R>()>	f__;
};


}	// namespace ftl


#endif // FTL_THUNK_HPP
