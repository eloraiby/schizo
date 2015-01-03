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

#include <schizo/schizo.hpp>

namespace schizo {
////////////////////////////////////////////////////////////////////////////////
/// built-in functions
////////////////////////////////////////////////////////////////////////////////
static exp::iptr
less_than(exp::iptr args)
{
	exp::iptr	left	= exp::list::head(args);
	exp::iptr	right	= exp::list::head(exp::list::tail(args));

	if( left->type() != right->type() &&
	    left->type() != exp::EXP_REAL64 &&
	    left->type() != exp::EXP_SINT64 ) {
		return new exp::error("lt takes 2 values of the same number type");
	}

	switch(left->type()) {
	case exp::EXP_REAL64:
		return new exp::boolean(static_cast<exp::real64*>(left.get())->value() < static_cast<exp::real64*>(right.get())->value());
	case exp::EXP_SINT64:
		return new exp::boolean(static_cast<exp::sint64*>(left.get())->value() < static_cast<exp::sint64*>(right.get())->value());
	default:
		return new exp::error("lt should not pass here");
	}

}

static exp::iptr
display(exp::iptr args)
{
	exp::print(args, 0);
	return nullptr;
}

struct built_in_entry {
	const char*	name;
	sint32		arg_count;
	exp::ffi::call	call;
};

static built_in_entry s_entries[]	= {
	{ "lt",		2, less_than },
	{ "display",   -1, display   },
};

exp::iptr
__get_built_ins(exp::iptr env)
{
	for( size_t i = 0; i < sizeof(s_entries) / sizeof(built_in_entry); ++i ) {
		env = state::add_ffi(env, s_entries[i].name,  s_entries[i].arg_count, s_entries[i].call);
	}
	return env;
}

}	// namespace schizo
