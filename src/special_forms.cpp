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
/// special forms
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief symbol_bind
 * @param s
 * @param env
 * @param args
 * @return
 */
static exp::iptr
symbol_bind(exp::iptr UNUSED env,
	    exp::iptr args)
{
	exp::iptr sym	= exp::list::head(args);
	exp::iptr body	= exp::list::head(exp::list::tail(args));

	exp::iptr pair_	= pair(sym, body);

	return new exp::bind(new exp::list(pair_, nullptr));
}

/**
 * @brief make_closure
 * @param s
 * @param env
 * @param args
 * @return
 */
static exp::iptr
make_closure(exp::iptr env,
	     exp::iptr args)
{
	// the new nullptr list is needed in case we pass a NIL argument ()
	exp::iptr	syms	= exp::list::head(args) ? args : new exp::list(nullptr, nullptr);
	exp::iptr	body	= exp::list::tail(args);

	exp::iptr	lam	= new exp::lambda(syms, body);

	return new exp::closure(env, lam);
}

static exp::iptr
if_else(exp::iptr env,
	exp::iptr args)
{
	if( exp::list::length(args) != 4 ) {
		return new exp::error("ERROR in \"if\" usage: if cond exp0 else exp1");
	}

	exp::iptr cond		= exp::list::head(args);
	exp::iptr exp0		= exp::list::head(exp::list::tail(args));
	exp::iptr elsym	= exp::list::head(exp::list::tail(exp::list::tail(args)));
	exp::iptr exp1		= exp::list::head(exp::list::tail(exp::list::tail(exp::list::tail(args))));

	if( elsym->type() == exp::EXP_SYMBOL && strcmp(static_cast<exp::symbol*>(elsym.get())->value(), "else") == 0 ) {
		exp::iptr b	= state::eval(env, cond);
		switch( b->type() ) {
		case exp::EXP_ERROR:
			return b;

		case exp::EXP_BOOLEAN:
			if( static_cast<exp::boolean*>(b.get())->value() ) {
				return exp0;
			} else {
				return exp1;
			}

		default:
			return new exp::error("ERROR: \"if\" requires condition to be boolean");
		}
	} else {
		return new exp::error("ERROR: \"if\" requires \"else\" keyword: if cond exp0 else exp1");
	}
}

struct special_form_entry {
	const char*		name;
	sint32			arg_count;
	exp::special::call	call;
};

static special_form_entry s_entries[]	= {
	{ "lambda", -1, make_closure },
	{ "define",  2,	symbol_bind },
	{ "if",      4,	if_else }
};

exp::iptr
__get_special_forms(exp::iptr env)
{
	for( size_t i = 0; i < sizeof(s_entries) / sizeof(special_form_entry); ++i ) {
		env = state::add_special(env, s_entries[i].name,  s_entries[i].arg_count, s_entries[i].call);
	}
	return env;
}

}	// namespace schizo
