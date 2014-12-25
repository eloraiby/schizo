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
/*
 * A C like scanner. Uses the longest match construction.
 * Taken from Ragel site
 */

#include <stdio.h>

#include "schizo.h"

extern void*	parser_alloc(void *(*mallocProc)(size_t));
extern void	parser_free(void *p, void (*freeProc)(void*));
extern void	parser_advance(void *yyp, int yymajor, schizo::cell* yyminor, schizo::state* s);

namespace schizo {

#define ADVANCE(A)	s->parser_.token_start	= ts; \
			s->parser_.token_end	= te; \
			s->parser_.token_line	= line; \
			copy_token(ts, te, tmp); \
			s->parser_.current_cell	= token_to_##A(tmp); \
			parser_advance(parser, s->parser_.current_cell->type(), s->parser_.current_cell.get(), s)

#define ADVANCE_TOKEN(A)	parser_advance(parser, A, nullptr, s)

#define PUSH_TE()	const char* tmp_te = te
#define POP_TE()	te	= tmp_te
#define PUSH_TS()	const char* tmp_ts = ts
#define POP_TS()	ts	= tmp_ts

/* EOF char used to flush out that last token. This should be a whitespace
 * token. */

#define LAST_CHAR 0


%%{
	machine scanner;
	write data nofinal;

	# operator char
	op_char_left	= [\+\-\*/\|\&\>\<\=\~\!\^\%\?\:]+;
	op_char_right	= '/*' | '*/' | '//';
	op_char		= ( op_char_left - op_char_right );
	# Floating literals.
	fract_const	= digit* '.' digit+ | digit+ '.';
	exponent	= [eE] [+\-]? digit+;

	action inc_line { ++line; }

	c_comment	:= (any | '\n'@inc_line)* :>> '*/' @{ fgoto main; };

	main := |*
		'#t'						{ ADVANCE( boolean );};
		'#f'						{ ADVANCE( boolean );};

		# Single and double literals.
		( "'" (any - ['\\] ) "'" )			{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( char );
									POP_TS();
									POP_TE();
								};

		( "'" [\\] ([bnrt\']|([0-9]+))"'" )		{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( char );
									POP_TS();
									POP_TE();
								};

		( '"' ( [^"\\\n] | /\\./ )* '"' )		{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( string );
									POP_TS();
									POP_TE();
								};

		# Identifiers
		( [a-zA-Z_] [a-zA-Z0-9_]* )			{ ADVANCE( symbol );};
		( op_char op_char* )				{ ADVANCE( symbol );};

		# Floating literals.
		( [+\-]? fract_const exponent? | [+\-]? digit+ exponent ) 	{ ADVANCE( real64 );};


		# Integer decimal. Leading part buffered by float.
		( [+\-]? ( '0' | [1-9] [0-9]* ) )		{ ADVANCE( sint64 ); };

		( [+\-]? ( '0' | [1-9] [0-9]* ) [a-zA-Z_]+ )	{
									fprintf(stderr, "Error: invalid number:\n    ");

									for( i = ts; i < te; ++i ) {
										fprintf(stderr, "%c", *i);
									}
									fprintf(stderr, "\n");
									cs	= scanner_error;
								};

		# Only buffer the second item, first buffered by symbol. */
		'('						{ ADVANCE_TOKEN( LPAR );};
		')'						{ ADVANCE_TOKEN( RPAR );};
		'{'						{ ADVANCE_TOKEN( LBR );};
		'}'						{ ADVANCE_TOKEN( RBR );};
		'['						{ ADVANCE_TOKEN( LSQB );};
		']'						{ ADVANCE_TOKEN( RSQB );};
		';'						{ ADVANCE_TOKEN( SEMICOL );};

		'\n'						{ ++line; };

		# Single char symbols.
		( punct - [_"'] )				{ printf("unexpected character %c\n", *ts); };

		# Comments and whitespace.
		'/*'						{ fgoto c_comment; };
		'//' [^\n]* '\n'@inc_line;
		( any - 33..126 )+;
	*|;
}%%

static uint32
copy_token(const char* ts, const char *te, char* dst) {
	uint32	index	= 0;
	while( ts < te ) {
		dst[index++]	= *ts;
		++ts;
	}
	dst[index] = '\0';
	return index;
}

static cell::iptr
token_to_symbol(const char* sym) {
	return new symbol(sym);
}

/*
static cell_ptr_t
token_to_unary_op(state_t* s, const char* op) {
	return atom_new_unary_op(s, op);
}

static cell_ptr_t
token_to_binary_op(state_t* s, const char* op) {
	return atom_new_binary_op(s, op);
}
*/

static cell::iptr
token_to_boolean(const char* b) {
	if( !strcmp(b, "#t") ) {
		return new bool_cell(true);
	} else {
		return new bool_cell(false);
	}
}

static cell::iptr
token_to_char(const char* ch) {
	return new char_cell(*ch);
}

static cell::iptr
token_to_sint64(const char* i) {
	sint64	v	= 0;
	sscanf(i, "%ld", &v);
	/* TODO: check limit */
	return new sint64_cell(v);
}

static cell::iptr
token_to_real64(const char* r) {
	real64	v	= 0;
	sscanf(r, "%lf", &v);
	/* TODO: check limit */
	return new real64_cell(v);
}

static cell::iptr
token_to_string(const char* str) {
	return new string_cell(str);
}


void
state::parse(state* s, const char* str)
{
	void*		parser;
	size_t		line	= 1;
	const char*	ts	= str;
	const char*	te	= str;
	const char*	i	= NULL;

	const char*	p	= str;
	const char*	pe	= p + strlen(str) + 1;
	const char*	eof	= NULL;

	int		act	= 0;
	int		cs	= 0;
	char		tmp[4096];

	s->parser_.root	= nullptr;
	s->parser_.token_list	= nullptr;

	parser	= parser_alloc(malloc);

	memset(tmp, 0, sizeof(tmp));

	%% write init;

	%% write exec;

	/* Check if we failed. */
	if ( cs == scanner_error ) {
		/* Machine failed before finding a token. */
		printf("PARSE ERROR\n");
		exit(1);
	}

	parser_advance(parser, 0, nullptr, s);

	parser_free(parser, free);
}
}
