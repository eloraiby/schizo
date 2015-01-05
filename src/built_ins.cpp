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
#include <cmath>

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
greater_than(exp::iptr args)
{
	exp::iptr	left	= exp::list::head(args);
	exp::iptr	right	= exp::list::head(exp::list::tail(args));

	if( left->type() != right->type() &&
	    left->type() != exp::EXP_REAL64 &&
	    left->type() != exp::EXP_SINT64 ) {
		return new exp::error("gt takes 2 values of the same number type");
	}

	switch(left->type()) {
	case exp::EXP_REAL64:
		return new exp::boolean(static_cast<exp::real64*>(left.get())->value() > static_cast<exp::real64*>(right.get())->value());
	case exp::EXP_SINT64:
		return new exp::boolean(static_cast<exp::sint64*>(left.get())->value() > static_cast<exp::sint64*>(right.get())->value());
	default:
		return new exp::error("gt should not pass here");
	}
}

static exp::iptr
less_or_eq(exp::iptr args)
{
	exp::iptr	left	= exp::list::head(args);
	exp::iptr	right	= exp::list::head(exp::list::tail(args));

	if( left->type() != right->type() &&
	    left->type() != exp::EXP_REAL64 &&
	    left->type() != exp::EXP_SINT64 ) {
		return new exp::error("leq takes 2 values of the same number type");
	}

	switch(left->type()) {
	case exp::EXP_REAL64:
		return new exp::boolean(static_cast<exp::real64*>(left.get())->value() <= static_cast<exp::real64*>(right.get())->value());
	case exp::EXP_SINT64:
		return new exp::boolean(static_cast<exp::sint64*>(left.get())->value() <= static_cast<exp::sint64*>(right.get())->value());
	default:
		return new exp::error("leq should not pass here");
	}
}

static exp::iptr
greater_or_eq(exp::iptr args)
{
	exp::iptr	left	= exp::list::head(args);
	exp::iptr	right	= exp::list::head(exp::list::tail(args));

	if( left->type() != right->type() &&
	    left->type() != exp::EXP_REAL64 &&
	    left->type() != exp::EXP_SINT64 ) {
		return new exp::error("geq takes 2 values of the same number type");
	}

	switch(left->type()) {
	case exp::EXP_REAL64:
		return new exp::boolean(static_cast<exp::real64*>(left.get())->value() >= static_cast<exp::real64*>(right.get())->value());
	case exp::EXP_SINT64:
		return new exp::boolean(static_cast<exp::sint64*>(left.get())->value() >= static_cast<exp::sint64*>(right.get())->value());
	default:
		return new exp::error("geq should not pass here");
	}
}

static exp::iptr
equal(exp::iptr args)
{
	exp::iptr	left	= exp::list::head(args);
	exp::iptr	right	= exp::list::head(exp::list::tail(args));

	if( left->type() != right->type() &&
	    left->type() != exp::EXP_REAL64 &&
	    left->type() != exp::EXP_SINT64 &&
	    left->type() != exp::EXP_BOOLEAN &&
	    left->type() != exp::EXP_STRING &&
	    left->type() != exp::EXP_SYMBOL ) {
		return new exp::error("eq takes 2 values of the same type");
	}

	switch(left->type()) {
	case exp::EXP_REAL64:
		return new exp::boolean(static_cast<exp::real64*>(left.get())->value() == static_cast<exp::real64*>(right.get())->value());
	case exp::EXP_SINT64:
		return new exp::boolean(static_cast<exp::sint64*>(left.get())->value() == static_cast<exp::sint64*>(right.get())->value());
	case exp::EXP_BOOLEAN:
		return new exp::boolean(static_cast<exp::boolean*>(left.get())->value() == static_cast<exp::boolean*>(right.get())->value());
	case exp::EXP_SYMBOL:
		return new exp::boolean(strcmp(static_cast<exp::symbol*>(left.get())->value(), static_cast<exp::symbol*>(right.get())->value()) == 0);
	case exp::EXP_STRING:
		return new exp::boolean(strcmp(static_cast<exp::string*>(left.get())->value(), static_cast<exp::string*>(right.get())->value()) == 0);
	default:
		return new exp::error("eq should not pass here");
	}
}

exp::iptr
add(exp::iptr args)
{
	exp::iptr	v	= exp::list::head(args);
	exp::iptr	tmp	= exp::list::tail(args);

	exp::TYPE	t	= v->type();

	if( t == exp::EXP_REAL64 || t == exp::EXP_SINT64) {

		// validate type
		while( tmp && exp::list::head(tmp) && t == exp::list::head(tmp)->type() ) {
			tmp	= exp::list::tail(tmp);
		}

		if( tmp ) {
			return new exp::error("add takes values of the same number type");
		}

		switch( t ) {
		case exp::EXP_REAL64: {
			real64	r	= static_cast<exp::real64*>(v.get())->value();
			tmp		= exp::list::tail(args);
			while( tmp ) {
				r	+= static_cast<exp::real64*>(exp::list::head(tmp).get())->value();
				tmp	= exp::list::tail(args);
			}

			return new exp::real64(r);
		}

		case exp::EXP_SINT64: {
			sint64	i	= static_cast<exp::sint64*>(v.get())->value();
			tmp		= exp::list::tail(args);
			while( tmp ) {
				i	+= static_cast<exp::sint64*>(exp::list::head(tmp).get())->value();
				tmp	= exp::list::tail(tmp);
			}

			return new exp::sint64(i);
		}

		default:
			return new exp::error("add should not pass here");
		}
	}


	return new exp::error("add takes values of the same number type");
}

exp::iptr
sub(exp::iptr args)
{
	exp::iptr	v	= exp::list::head(args);
	exp::iptr	tmp	= exp::list::tail(args);

	exp::TYPE	t	= v->type();

	if( t == exp::EXP_REAL64 || t == exp::EXP_SINT64) {

		// validate type
		while( tmp && exp::list::head(tmp) && t == exp::list::head(tmp)->type() ) {
			tmp	= exp::list::tail(tmp);
		}

		if( tmp ) {
			return new exp::error("add takes values of the same number type");
		}

		switch( t ) {
		case exp::EXP_REAL64: {
			real64	r	= static_cast<exp::real64*>(v.get())->value();
			tmp		= exp::list::tail(args);
			while( tmp ) {
				r	-= static_cast<exp::real64*>(exp::list::head(tmp).get())->value();
				tmp	= exp::list::tail(args);
			}

			return new exp::real64(r);
		}

		case exp::EXP_SINT64: {
			sint64	i	= static_cast<exp::sint64*>(v.get())->value();
			tmp		= exp::list::tail(args);
			while( tmp ) {
				i	-= static_cast<exp::sint64*>(exp::list::head(tmp).get())->value();
				tmp	= exp::list::tail(tmp);
			}

			return new exp::sint64(i);
		}

		default:
			return new exp::error("add should not pass here");
		}
	}

	return new exp::error("add takes values of the same number type");
}

static exp::iptr
div(exp::iptr args)
{
	exp::iptr	left	= exp::list::head(args);
	exp::iptr	right	= exp::list::head(exp::list::tail(args));

	if( left->type() != right->type() &&
	    left->type() != exp::EXP_REAL64 &&
	    left->type() != exp::EXP_SINT64 ) {
		return new exp::error("div takes 2 values of the same number type");
	}

	switch(left->type()) {
	case exp::EXP_REAL64:
		return new exp::real64(static_cast<exp::real64*>(left.get())->value() / static_cast<exp::real64*>(right.get())->value());
	case exp::EXP_SINT64:
		return new exp::sint64(static_cast<exp::sint64*>(left.get())->value() / static_cast<exp::sint64*>(right.get())->value());
	default:
		return new exp::error("div should not pass here");
	}
}

static exp::iptr
mod(exp::iptr args)
{
	exp::iptr	left	= exp::list::head(args);
	exp::iptr	right	= exp::list::head(exp::list::tail(args));

	if( left->type() != right->type() &&
	    left->type() != exp::EXP_REAL64 &&
	    left->type() != exp::EXP_SINT64 ) {
		return new exp::error("mod takes 2 values of the same number type");
	}

	switch(left->type()) {
	case exp::EXP_REAL64:
		return new exp::real64(fmod(static_cast<exp::real64*>(left.get())->value(), static_cast<exp::real64*>(right.get())->value()));
	case exp::EXP_SINT64:
		return new exp::sint64(static_cast<exp::sint64*>(left.get())->value() % static_cast<exp::sint64*>(right.get())->value());
	default:
		return new exp::error("mod should not pass here");
	}
}

static exp::iptr
head(exp::iptr args)
{
	exp::iptr	h	= exp::list::head(args);

	if( h->type() != exp::EXP_LIST ) {
		return new exp::error("head takes a list");
	}

	return exp::list::head(h);
}

static exp::iptr
tail(exp::iptr args)
{
	exp::iptr	h	= exp::list::head(args);

	if( h->type() != exp::EXP_LIST ) {
		return new exp::error("tail takes a list");
	}

	return exp::list::tail(h);
}

static exp::iptr
cons(exp::iptr args)
{
	exp::iptr	left	= exp::list::head(args);
	exp::iptr	right	= exp::list::head(exp::list::tail(args));

	if( right && right->type() != exp::EXP_LIST ) {
		return new exp::error("cons requires the second argument to be a list");
	}

	return new exp::list(left, right);
}

static exp::iptr
display(exp::iptr args)
{
	exp::print(args, 0);
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct built_in_entry {
	const char*	name;
	sint32		arg_count;
	exp::ffi::call	call;
};

static built_in_entry s_entries[] = {
	{ "lt",		2, less_than },
	{ "gt",		2, greater_than },
	{ "leq",	2, less_or_eq },
	{ "geq",	2, greater_or_eq },

	{ "eq",		2, equal },

	{ "add",       -1, add	},
	{ "sub",       -1, sub	},
	{ "div",	2, div },
	{ "mod",	2, mod },

	{ "head",	1, head },
	{ "tail",	1, tail },
	{ "cons",	2, cons },

	{ "display",   -1, display   },
};

exp::iptr
__get_built_ins(exp::iptr env)
{
	for( size_t i = 0; i < sizeof(s_entries) / sizeof(built_in_entry); ++i ) {
		env = add_ffi(env, s_entries[i].name,  s_entries[i].arg_count, s_entries[i].call);
	}
	return env;
}

}	// namespace schizo
