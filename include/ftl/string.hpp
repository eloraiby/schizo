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
#ifndef FTL_STRING_HPP
#define FTL_STRING_HPP

#ifndef FTL_ARRAY_HPP
#	include "array.hpp"
#endif

#include <string.h>

namespace ftl {

struct string : public array<char> {
	string(const char* str) : array(string::length(str) + 1, str)	{}
	string(const string& str) : array(str)				{}
	string() : array()						{}

	string		operator + (const string& right) const
	{
		auto	arr	= array<char>(size() + right.size() - 1, '\0');
		string&	str	= static_cast<string&>(arr);
		auto	dst	= const_cast<char*>(str.__fast_array);
		memcpy(dst, __fast_array, length());
		memcpy(dst + length(), right.__fast_array, right.size());
		return str;
	}

	size_t			length() const				{ return string::length(*this);	}
	static size_t		length(const string& str)		{ return str.size() - 1; }
	static size_t		length(const char* str)			{ return strlen(str);	 }
};

}	// namespace ftl

#endif // FTL_STRING_HPP
