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
		EXCEPTION,
	};

	struct exception {
		exception() {}
		exception(const rstring& msg) : msg__(msg)	{}
		const rstring&	message() const			{ return msg__; }

	private:
		rstring		msg__;
	};

	TYPE		get_type() const	{ return type__; }

	result(const R& r) : type__(FINAL), v__(r) {}
	result(rfunction<result<R>()> f) : type__(CONTINUATION), f__(f) {}
	result(const exception& except) : type__(EXCEPTION), except__(except) {}

	TYPE	step() {
		switch(type__) {
		case CONTINUATION: {
				result<R>	r = f__();

				type__	= r.type__;

				switch( type__ ) {
				case FINAL:
					v__	= r.v__;
					break;
				case EXCEPTION:
					except__	= r.except__;
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
		return type__;
	}

	template<typename RM>
	RM	match(const rfunction<RM(R)>& lambda_res, const rfunction<RM(const exception&)>& except) {
		switch(type__) {
		case FINAL:
			return lambda_res(v__);

		case EXCEPTION:
			return except(except__);

		case CONTINUATION:
			result<R>	r = f__();

			while( r.type__ == CONTINUATION ) {
				r	= r.f__();
			}

			switch( r.type__ ) {
			case FINAL:
				v__	= r.v__;
				return lambda_res(r.v__);
			case EXCEPTION:
				except__	= r.except__;
				return except(r.except__);
			}
			break;
		}
	}


private:
	TYPE		type__;

	R		v__;
	rfunction<result<R>()>	f__;
	exception	except__;
};


}	// namespace ftl


#endif // FTL_CONTINUATION_HPP
