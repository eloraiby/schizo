/*
  Schizo programming language
  Copyright (C) 2014  Wael El Oraiby

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

#include <stdio.h>

#include "schizo.h"


int
main(int argc,
     char* argv[])
{
	const char*	prog[]	= {
		"((((a))))",
		"(10 -10 +10)",
		"(10.0 -10.1 +10.2)",
		"(a 10 -11 true ++ - +)",
		"(b (false -- +10 10) * &)",
	};

	state_t*	state;

	for(size_t i = 0; i < sizeof(prog) / sizeof(const char*); ++i ) {
		state	= parse(prog[i]);
		print_cell(state, state->root, 0);
		fprintf(stderr, "---------------\n");
	}

	return 0;
}
