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

environment::environment() : count__(0) {}

environment::environment(const environment& other) :
	operators_(other.operators_),
	symbols_(other.symbols_),
	count__(0)
{}


environment::iptr
environment::add_operator(ftl::string symbol,
			  ftl::string synonym,
			  uint32 priority,
			  bool is_unary) const
{
	iptr	env	= new environment(*this);
	env->operators_	= operators_.push_front(operator_entry(symbol, synonym, priority, is_unary));
	return env;
}

environment::iptr
environment::add_symbol(ftl::string symbol,
			environment::exp_iptr expression) const
{
	iptr	env	= new environment(*this);
	env->symbols_	= symbols_.push_front(symbol_entry(symbol, expression));
	return env;
}

environment::operator_entry
environment::find_operator(ftl::string op) const
{

}

environment::symbol_entry
environment::find_symbol(ftl::string str) const
{

}

}
