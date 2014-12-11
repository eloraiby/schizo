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

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

int
main(int argc,
     char* argv[])
{
	struct timeval start, end;

	long mtime, seconds, useconds;

	const char*	prog[]	= {
		"(a)",
		"((((a))))",
		"(10 -11 +12)",
		"(10.0 -10.1 +10.2)",
		"(a 10 -11 true ++ - +)",
		"(b (false -- +10 10) * & #t)",
		"((a b c) (d e f g))",
		"{ }",
		"{}",
		"{ a b c; 1 2 3 }",
		"{ a b c; 1 2 3; 4 5 6 }",
		"{ a b c; 1 2 3;;; d e f}",
		"{ a b c; 1 2 3; 4 5 6;;; }",
		"{ a }",
		"{ a b }",
		"{ a; b }",
		"{ a; b c }",
		"{ if (a == b) { do this } else {do that}}",
		"{ a[10] }",
		"{ a[10][11] }",
		"{ a[10]; b[11]; c[x + y + z] }",
		"{ a[10][11] b[11]; c[x + y + z] }",
		"( a[10] )",
		"( a[10][11] )",
		"{ (a b c)[10] }",
		"{ a[10] + 10.5 asb }",
		"{ 5 + (a b c)[10] }",
		"{ a b c[10] d; ef g }",
		"{ !a }",
		"{ a + b }",
		"{ !a + b }",
		"{ a + !b }",
		"{ !a + !b }",

	};

	state_t*	state	= state_new();

	for(size_t i = 0; i < sizeof(prog) / sizeof(const char*); ++i ) {
		parse(state, prog[i]);
		print_cell(state, state->root, 0);
		fprintf(stderr, "\n");
		fprintf(stderr, "freed: %u\n", gc(state));
	}


	state_release(state);

	state	= state_new();
	char* example	= "((lambda () (define str \"hello\") (display str))())";
	parse(state, example);
	eval(state, state->root);
	gettimeofday(&start, NULL);
	fprintf(stderr, "freed: %u\n", gc(state));
	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = (long)(((seconds) * 1000 + useconds/1000.0) + 0.5);
	fprintf(stderr, "took %ld ms to scan memory\n", mtime);
	state_release(state);

	return 0;
}
