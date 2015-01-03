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
#include <stdio.h>

namespace schizo {

exp::~exp() {

}

////////////////////////////////////////////////////////////////////////////////
/// printing
////////////////////////////////////////////////////////////////////////////////
static void
print_level(uint32 level) {
	uint32	i	= 0;
	for( i = 0; i < level; ++i ) {
		fprintf(stderr, "  ");
	}
}

void
exp::print(exp::iptr c,
	   uint32 level)
{
	print_level(level);
	if( c ) {
		switch( c->type() ) {
		case exp::EXP_BOOLEAN:
			if( static_cast<exp::boolean*>(c.get())->value() ) {
				fprintf(stderr, "#t");
			} else {
				fprintf(stderr, "#f");
			}
			break;

		case exp::EXP_SYMBOL:
			fprintf(stderr, "%s", static_cast<exp::symbol*>(c.get())->value());
			break;

		case exp::EXP_CHARACTER:
			fprintf(stderr, "'%c'", static_cast<exp::character*>(c.get())->value());
			break;

		case exp::EXP_SINT64:
			fprintf(stderr, "%ld", static_cast<exp::sint64*>(c.get())->value());
			break;

		case exp::EXP_REAL64:
			fprintf(stderr, "%lf", static_cast<exp::real64*>(c.get())->value());
			break;

		case exp::EXP_STRING:
			fprintf(stderr, "\"%s\"", static_cast<exp::string*>(c.get())->value());
			break;

		case exp::EXP_ERROR:
			fprintf(stderr, "<ERROR>");
			break;

		case exp::EXP_CLOSURE:
			fprintf(stderr, "<CLOSURE>");
			break;

		case exp::EXP_FFI:
			fprintf(stderr, "<FFI>");
			break;

		case exp::EXP_LIST:
			if( exp::list::head(c) ) {
				fprintf(stderr, "(");
				exp::print(exp::list::head(c), uint32(0));
			} else {
				fprintf(stderr, "(nil");
			}

			if( exp::list::tail(c) ) {
				exp::iptr n	= exp::list::tail(c);
				while( n ) {
					if( exp::list::head(n) ) {
						exp::print(exp::list::head(n), 1);
					} else {
						print_level(level + 1);
						fprintf(stderr, "nil");
					}

					n	= exp::list::tail(n);
				}
			}
			fprintf(stderr, ")");
			break;
		default:
			break;
		}
	} else {
		fprintf(stderr, "nil");
	}
}

void
exp::print_env(exp::iptr env)
{
	fprintf(stderr, "------------------------\n");
	while( env ) {
		fprintf(stderr, "* %s :: ", static_cast<exp::symbol*>(exp::list::head(exp::list::head(env)).get())->value());
		exp::print(exp::list::head(exp::list::tail(exp::list::head(env))), 0);
		fprintf(stderr, "\n");
		env	= exp::list::tail(env);
	}
}

////////////////////////////////////////////////////////////////////////////////
/// printing
////////////////////////////////////////////////////////////////////////////////
uint32
exp::list::length(exp::iptr l)
{
	exp::iptr	curr	= l;
	uint32		len	= 0;

	while( curr ) {
		assert( l->type() == EXP_LIST );
		++len;
		curr	= list::tail(curr);
	}
	return len;
}

exp::iptr
exp::list::reverse(exp::iptr l)
{
	exp::iptr	nl	= nullptr;

	while( l ) {
		nl	= new list(list::head(l), nl);
		l	= list::tail(l);
	}

	return nl;
}

}	// namespace schizo
