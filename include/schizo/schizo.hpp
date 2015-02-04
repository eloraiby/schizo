#ifndef SCHIZO_H
#define SCHIZO_H
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

#include "environment.hpp"
#include "expression.hpp"

namespace schizo {
exp::special::val	eval(exp::iptr env, exp::iptr e);
exp::iptr		eval_list(exp::iptr env, exp::iptr l);

/// parser.y / lexer.rl
struct parser {
	exp::iptr	token_list;
	exp::iptr	root;		///< root exp
	const char*	token_start;
	const char*	token_end;
	uint32		token_line;

	parser() : token_list(nullptr), root(nullptr), token_start(nullptr), token_end(nullptr), token_line(0) {}

	static inline exp* add_token(parser* s, exp* c) {
		s->token_list	= new exp::list(c, s->token_list);
		return c;
	}
};

exp::iptr	parse(const char* str);

exp::iptr	default_env();
exp::iptr	add_ffi(exp::iptr env, const char* sym, sint32 arg_count, exp::ffi::call proc);
exp::iptr	add_special(exp::iptr env, const char* sym, sint32 arg_count, exp::special::call proc);
exp::iptr	lookup(exp::iptr env, const char* sym);

/* lists */
static inline exp::iptr	pair(exp::iptr fst, exp::iptr snd) { return new exp::list((fst), new exp::list((snd), nullptr)); }

}	// namespace schizo

#endif /* SCHIZO_H */
