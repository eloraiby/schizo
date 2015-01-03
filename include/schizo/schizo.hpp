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

#include "expression.hpp"

namespace schizo {

struct state : public exp {

	state();
	virtual		~state() override;

	inline iptr	root() const		{ return parser_.root; }
	inline void	set_root(iptr root)	{ parser_.root = root; }

	static iptr	eval(iptr env, iptr exp);

	/// parser.y / lexer.rl
	static void	parse(state* state, const char* str);

	static iptr	default_env();
	static iptr	add_ffi(exp::iptr env, const char* sym, sint32 arg_count, ffi::call proc);
	static iptr	add_special(exp::iptr env, const char* sym, sint32 arg_count, special::call proc);
	static iptr	lookup(exp::iptr env, const char* sym);

	static inline exp* add_token(state* s, exp* c) {
		s->parser_.token_list	= new list(c, s->parser_.token_list);
		return c;
	}

protected:

	static iptr	eval_list(iptr env, iptr l);

	static iptr	apply_bind(iptr env, iptr bexp);

	struct {
		exp::iptr	token_list;
		exp::iptr	root;		///< root exp
		const char*	token_start;
		const char*	token_end;
		uint32		token_line;
	} parser_;
};

/* lists */
static inline exp::iptr	pair(exp::iptr fst, exp::iptr snd) { return new exp::list((fst), new exp::list((snd), nullptr)); }



}	// namespace schizo

#endif /* SCHIZO_H */
