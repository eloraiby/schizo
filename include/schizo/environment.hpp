#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
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

#include "../ftl/function.hpp"

namespace schizo {
struct exp;

struct environment {
	typedef ftl::intrusive_ptr<environment>	iptr;
	typedef ftl::intrusive_ptr<exp>		exp_iptr;

	struct operator_entry {
		inline operator_entry(ftl::string symbol, ftl::string synonym, uint32 priority, bool is_unary) :
			symbol_(symbol),
			synonym_(synonym),
			priority_(priority),
			is_unary_(is_unary)
		{}

		inline operator_entry(const operator_entry& other) :
			symbol_(other.symbol()),
			synonym_(other.synonym()),
			priority_(other.priority()),
			is_unary_(other.is_unary())
		{}

		inline ftl::string	symbol() const		{ return symbol_;   }
		inline ftl::string	synonym() const		{ return synonym_;  }
		inline uint32		priority() const	{ return priority_; }
		inline bool		is_unary() const	{ return is_unary_; }

	private:
		ftl::string	symbol_;
		ftl::string	synonym_;
		uint32		priority_;
		bool		is_unary_;
	};

	struct symbol_entry {
		symbol_entry(ftl::string symbol, exp_iptr expression);

	private:
		ftl::string	symbol_;
		exp_iptr	expression_;
	};

	iptr		add_operator(ftl::string symbol, ftl::string synonym, uint32 priority, bool is_unary) const;
	iptr		add_symbol(ftl::string symbol, exp_iptr expression) const;

	operator_entry	find_operator(ftl::string op);
	symbol_entry	find_symbol(ftl::string str);

protected:

	typedef ftl::list<operator_entry>	operator_list;

	typedef ftl::list<symbol_entry>		symbol_list;

	operator_list	operators_;
	symbol_list	symbols_;
};

}

#endif // ENVIRONMENT_HPP
