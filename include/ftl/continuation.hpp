#ifndef FTL_FUNCTION_HPP
#	include "function.hpp"
#endif

#ifndef FTL_CONTINUATION_HPP
#define FTL_CONTINUATION_HPP

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
struct result {

	enum TYPE {
		FINAL,
		CONTINUATION,
	};

	TYPE		get_type() const	{ return type__; }

	result(const R& r) : type__(FINAL), v__(r) {}
	result(rfunction<result<R>()> f) : type__(CONTINUATION), f__(f) {}

	result<R>	step() {
		switch(type__) {
		case CONTINUATION:
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

		case CONTINUATION:
			result<R>	r = f__();

			while( r.type__ == CONTINUATION ) {
				r	= r.f__();
			}

			v__	= r.v__;
			return lambda_res(r.v__);
		}
	}

	R		value() {
		switch(type__) {
		case FINAL:
			return v__;

		case CONTINUATION:
			result<R>	r = f__();

			while( r.type__ == CONTINUATION ) {
				r	= r.f__();
			}

			v__	= r.v__;
			return v__;
		}
	}


private:
	TYPE		type__;

	R		v__;
	rfunction<result<R>()>	f__;
};


}	// namespace ftl


#endif // FTL_CONTINUATION_HPP
