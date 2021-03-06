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
/*
 * A C like scanner. Uses the longest match construction.
 * Taken from Ragel site
 */

#include <stdio.h>

#include "parser.h"
#include <schizo/schizo.hpp>

extern void*	parser_alloc(void *(*mallocProc)(size_t));
extern void	parser_free(void *p, void (*freeProc)(void*));
extern void	parser_advance(void *yyp, int yymajor, schizo::exp* yyminor, schizo::parser* s);

namespace schizo {
#define ADVANCE(A)	parser_.token_start	= ts; \
			parser_.token_end	= te; \
			parser_.token_line	= line; \
			copy_token(ts, te, tmp); \
			exp::iptr tmpc = token_to_##A(tmp); \
			parser_.token_list	= new exp::list(tmpc, parser_.token_list); \
			parser_advance(yyparser, tmpc->type(), tmpc.get(), &parser_)

#define ADVANCE_OP(A, T)	parser_.token_start	= ts; \
			parser_.token_end	= te; \
			parser_.token_line	= line; \
			copy_token(ts, te, tmp); \
			exp::iptr tmpc = token_to_##A(tmp); \
			parser_.token_list	= new exp::list(tmpc, parser_.token_list); \
			parser_advance(yyparser, T, tmpc.get(), &parser_)

#define ADVANCE_TOKEN(A)	parser_advance(yyparser, A, nullptr, &parser_)

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
		'infixl'					{ fprintf(stderr, "INFIXL\n"); };

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

		# Identifiers
		( [a-zA-Z_] [a-zA-Z0-9_]* )			{ ADVANCE( symbol );};
		( op_char op_char* )				{
										copy_token(ts, te, tmp);

										if( strcmp("+", tmp) == 0 ) {
											ADVANCE_OP(symbol, TOK_OP_BIN3);
										} else if( strcmp("*", tmp) == 0 ) {
											ADVANCE_OP(symbol, TOK_OP_BIN0);
										} else {
											ADVANCE( symbol );
										}
								};


		# Floating literals.
		( [+\-]? fract_const exponent? | [+\-]? digit+ exponent ) 	{ ADVANCE( real64 );};

		# Only buffer the second item, first buffered by symbol. */
		'('						{ ADVANCE_TOKEN( TOK_LPAR );};
		')'						{ ADVANCE_TOKEN( TOK_RPAR );};
		'{'						{ ADVANCE_TOKEN( TOK_LBR );};
		'}'						{ ADVANCE_TOKEN( TOK_RBR );};
		'['						{ ADVANCE_TOKEN( TOK_LSQB );};
		']'						{ ADVANCE_TOKEN( TOK_RSQB );};
		'#['						{ ADVANCE_TOKEN( TOK_LHASH );};
		']#'						{ ADVANCE_TOKEN( TOK_RHASH );};
		';'						{ ADVANCE_TOKEN( TOK_SEMICOL );};

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

static exp::iptr
token_to_symbol(const char* sym) {
	return new exp::atom_symbol(sym);
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

static exp::iptr
token_to_boolean(const char* b) {
	if( !strcmp(b, "#t") ) {
		return new exp::atom_boolean(true);
	} else {
		return new exp::atom_boolean(false);
	}
}

static exp::iptr
token_to_char(const char* ch) {
	return new exp::atom_character(*ch);
}

static exp::iptr
token_to_sint64(const char* i) {
	sint64	v	= 0;
	sscanf(i, "%ld", &v);
	/* TODO: check limit */
	return new exp::atom_sint64(v);
}

static exp::iptr
token_to_real64(const char* r) {
	real64	v	= 0;
	sscanf(r, "%lf", &v);
	/* TODO: check limit */
	return new exp::atom_real64(v);
}

static exp::iptr
token_to_string(const char* str) {
	return new exp::atom_string(str);
}


exp::iptr
parse(const char* str)
{
	parser		parser_;

	void*		yyparser;
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

	parser_.root	= nullptr;
	exp::iptr token_list	= nullptr;

	yyparser	= parser_alloc(malloc);

	memset(tmp, 0, sizeof(tmp));

	%% write init;

	%% write exec;

	/* Check if we failed. */
	if ( cs == scanner_error ) {
		/* Machine failed before finding a token. */
		printf("PARSE ERROR\n");
		exit(1);
	}

	parser_advance(yyparser, 0, nullptr, &parser_);

	parser_free(yyparser, free);
	return parser_.root;
}
}
