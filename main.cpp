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

	const char*	prog[]	= { "()",
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

				    "("\
				    "  (lambda ()" \
				    "          (define str \"hello\")" \
				    "          (define b #t)" \
				    "          (if b" \
				    "              (display \"true\")" \
				    "           else" \
				    "              (display \"false\")" \
				    "          )" \
				    "  )" \
				    "  ()" \
				    ")"
	};

	char*		example	= "("\
				  "  (lambda ()" \
				  "          (define str \"hello\")" \
				  "          (define b #t)" \
				  "          (if b" \
				  "              (display \"true\")" \
				  "           else" \
				  "              (display \"false\")" \
				  "          )" \
				  "  )" \
				  "  ()" \
				  ")";

	/* char*		example	= "((lambda () (define str \"hello\") (display str))())"; */
	size_t		i	= 0;
	state_t*	s	= state_new();

	for( i = 0; i < sizeof(prog) / sizeof(const char*); ++i ) {
		parse(s, prog[i]);
		print_cell(s, s->parser.root, 0);
		fprintf(stderr, "\n");
	}


	state_release(s);

	s	= state_new();
	parse(s, example);
	set_cell(s->registers.exp_stack, s->parser.root);
	eval(s);
	gettimeofday(&start, NULL);
	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = (long)(((seconds) * 1000 + useconds/1000.0) + 0.5);
	fprintf(stderr, "took %ld ms to scan memory\n", mtime);
	state_release(s);

	return 0;
}