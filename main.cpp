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

#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#include <schizo/schizo.hpp>
#include <ftl/function.hpp>

using namespace schizo;

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
				    "{ a + b + c; a2 + b2 + c2; }",
				    "{ a + b * 2 }",
				    "{ a x y + b z w * 2 }",

/*				    "("\
				    "  (lambda ()" \
				    "          (bind str \"hello\")" \
				    "          (bind b #t)" \
				    "          (if b" \
				    "              (display \"true\")" \
				    "           else" \
				    "              (display \"false\")" \
				    "          )" \
				    "  )" \
				    "  ()" \
				    ")"
*/	};

	const char*	example	= "("\
				  "  (lambda ()" \
				  "          (bind str \"hello\")" \
				  "          (bind b #t)" \
				  "          (if b" \
				  "              (display \"res -> true\")" \
				  "           else" \
				  "              (display \"res -> false\")" \
				  "          )" \
				  "          (if (lt 2 4)" \
				  "              (display (add 1 2 3))" \
				  "           else" \
				  "              (display (sub 6 3 2))" \
				  "          )" \
				  "  )" \
				  "  ()" \
				  ")";

	/* char*		example	= "((lambda () (define str \"hello\") (display str))())"; */
	size_t		i	= 0;

	for( i = 0; i < sizeof(prog) / sizeof(const char*); ++i ) {
		exp::iptr root	= parse(prog[i]);
		exp::print(root, 0);
		fprintf(stderr, "\n");
	}

	exp::iptr root = parse(example);
	eval(default_env(), root);
	gettimeofday(&start, NULL);
	gettimeofday(&end, NULL);

	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;

	mtime = (long)(((seconds) * 1000 + useconds/1000.0) + 0.5);
	fprintf(stderr, "took %ld ms to scan memory\n", mtime);

	ftl::string	hello("hello");
	ftl::string	world("world");
	ftl::string	hello_world	= hello + world;

	fprintf(stderr, "string: %s\n", hello_world.get());

	fprintf(stderr, "sizeof(mutex): %u\n", sizeof(pthread_mutex_t));

	return 0;
}
