#include <stdio.h>

#include "ast.h"


int
main(int argc,
     char* argv[])
{
	const char*	prog[]	= {
		"(10y -10y +10y)",
		"(10 -10 +10)",
		"(10s -10s +10s)",
		"(10l -10l +10l)",
		"(10uy -10uy +10uy)",
		"(10u -10u +10u)",
		"(10us -10us +10us)",
		"(10ul -10ul +10ul)",
		"(a 10 -11 true ++ - +)",
		"(b (false -- +10 10l) * &)",
	};

	state_t*	state;

	for(size_t i = 0; i < sizeof(prog) / sizeof(const char*); ++i ) {
		state	= parse(prog[i]);
		print_cell(state->root, 0);
		fprintf(stderr, "---------------\n");
	}

	return 0;
}
