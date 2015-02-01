
#line 1 "/home/aifu/projects/schizo/src/lexer.rl"
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



#line 68 "/home/aifu/projects/schizo/src/lexer.cpp"
static const char _scanner_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 12, 1, 13, 1, 
	14, 1, 15, 1, 16, 1, 17, 1, 
	18, 1, 19, 1, 20, 1, 21, 1, 
	22, 1, 23, 1, 24, 1, 25, 1, 
	27, 1, 28, 1, 29, 1, 30, 1, 
	31, 1, 32, 1, 33, 1, 34, 1, 
	35, 1, 36, 2, 0, 26, 2, 4, 
	5, 2, 4, 6, 2, 4, 7, 2, 
	4, 8, 2, 4, 9, 2, 4, 10, 
	2, 4, 11
};

static const unsigned char _scanner_key_offsets[] = {
	0, 0, 3, 3, 5, 6, 13, 14, 
	17, 19, 23, 25, 30, 31, 33, 36, 
	69, 71, 84, 87, 104, 108, 110, 120, 
	125, 134, 144, 146, 159, 173, 180, 181, 
	189, 197, 205, 213, 221
};

static const char _scanner_trans_keys[] = {
	10, 34, 92, 39, 92, 39, 39, 98, 
	110, 114, 116, 48, 57, 39, 39, 48, 
	57, 48, 57, 43, 45, 48, 57, 48, 
	57, 46, 69, 101, 48, 57, 10, 10, 
	42, 10, 42, 47, 10, 34, 35, 36, 
	39, 40, 41, 44, 46, 47, 48, 59, 
	64, 91, 92, 93, 94, 96, 105, 123, 
	125, 33, 42, 43, 45, 49, 57, 58, 
	63, 65, 122, 124, 126, 33, 126, 33, 
	45, 47, 58, 94, 124, 126, 37, 38, 
	42, 43, 60, 63, 91, 102, 116, 33, 
	46, 48, 58, 94, 124, 126, 37, 38, 
	42, 43, 45, 47, 49, 57, 60, 63, 
	69, 101, 48, 57, 48, 57, 46, 69, 
	95, 101, 48, 57, 65, 90, 97, 122, 
	95, 65, 90, 97, 122, 43, 45, 95, 
	48, 57, 65, 90, 97, 122, 46, 69, 
	95, 101, 48, 57, 65, 90, 97, 122, 
	48, 57, 33, 45, 47, 58, 94, 124, 
	126, 37, 38, 42, 43, 60, 63, 10, 
	33, 45, 47, 58, 94, 124, 126, 37, 
	38, 42, 43, 60, 63, 95, 48, 57, 
	65, 90, 97, 122, 35, 95, 110, 48, 
	57, 65, 90, 97, 122, 95, 102, 48, 
	57, 65, 90, 97, 122, 95, 105, 48, 
	57, 65, 90, 97, 122, 95, 120, 48, 
	57, 65, 90, 97, 122, 95, 108, 48, 
	57, 65, 90, 97, 122, 0
};

static const char _scanner_single_lengths[] = {
	0, 3, 0, 2, 1, 5, 1, 1, 
	0, 2, 0, 3, 1, 2, 3, 21, 
	0, 7, 3, 7, 2, 0, 4, 1, 
	3, 4, 0, 7, 8, 1, 1, 2, 
	2, 2, 2, 2, 0
};

static const char _scanner_range_lengths[] = {
	0, 0, 0, 0, 0, 1, 0, 1, 
	1, 1, 1, 1, 0, 0, 0, 6, 
	1, 3, 0, 5, 1, 1, 3, 2, 
	3, 3, 1, 3, 3, 3, 0, 3, 
	3, 3, 3, 3, 0
};

static const unsigned char _scanner_index_offsets[] = {
	0, 0, 4, 5, 8, 10, 17, 19, 
	22, 24, 28, 30, 35, 37, 40, 44, 
	72, 74, 85, 89, 102, 106, 108, 116, 
	120, 127, 135, 137, 148, 160, 165, 167, 
	173, 179, 185, 191, 197
};

static const char _scanner_indicies[] = {
	1, 2, 3, 0, 0, 1, 5, 4, 
	6, 1, 7, 7, 7, 7, 7, 8, 
	1, 9, 1, 9, 8, 1, 11, 10, 
	13, 13, 14, 12, 14, 12, 11, 17, 
	17, 16, 15, 19, 18, 21, 22, 20, 
	21, 22, 23, 20, 25, 0, 27, 28, 
	29, 30, 31, 28, 33, 34, 35, 37, 
	28, 39, 28, 40, 26, 28, 41, 42, 
	43, 26, 32, 36, 26, 38, 26, 24, 
	12, 24, 26, 26, 26, 26, 26, 26, 
	26, 26, 26, 26, 44, 46, 47, 48, 
	45, 26, 49, 35, 26, 26, 26, 26, 
	26, 26, 26, 36, 26, 44, 17, 17, 
	11, 50, 14, 50, 11, 53, 52, 53, 
	16, 52, 52, 51, 52, 52, 52, 54, 
	13, 13, 52, 14, 52, 52, 54, 11, 
	53, 52, 53, 36, 52, 52, 51, 11, 
	45, 26, 26, 55, 26, 26, 26, 26, 
	26, 26, 26, 44, 19, 55, 55, 55, 
	55, 55, 55, 55, 55, 55, 55, 18, 
	38, 38, 38, 38, 12, 57, 56, 38, 
	59, 38, 38, 38, 58, 38, 60, 38, 
	38, 38, 58, 38, 61, 38, 38, 38, 
	58, 38, 62, 38, 38, 38, 58, 38, 
	63, 38, 38, 38, 58, 1, 0
};

static const char _scanner_trans_targs[] = {
	1, 0, 15, 2, 4, 5, 15, 6, 
	7, 15, 15, 20, 15, 10, 21, 15, 
	11, 9, 12, 15, 13, 13, 14, 36, 
	16, 16, 17, 18, 15, 3, 15, 15, 
	19, 26, 27, 22, 25, 15, 29, 15, 
	30, 31, 15, 15, 15, 15, 15, 15, 
	15, 8, 15, 15, 23, 24, 15, 28, 
	15, 15, 15, 32, 33, 34, 35, 29
};

static const char _scanner_trans_actions[] = {
	0, 0, 19, 0, 0, 0, 15, 0, 
	0, 17, 55, 74, 57, 0, 0, 53, 
	0, 0, 0, 59, 0, 1, 0, 3, 
	80, 77, 0, 0, 37, 0, 21, 23, 
	9, 0, 0, 65, 0, 35, 71, 29, 
	0, 0, 25, 27, 45, 51, 31, 13, 
	11, 0, 47, 39, 0, 68, 41, 9, 
	49, 33, 43, 0, 0, 0, 0, 62
};

static const char _scanner_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 5, 0, 5, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const char _scanner_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 7, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const unsigned char _scanner_eof_trans[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	11, 13, 13, 16, 11, 0, 0, 0, 
	13, 45, 46, 45, 51, 51, 52, 55, 
	55, 52, 46, 45, 45, 13, 57, 59, 
	59, 59, 59, 59, 0
};

static const int scanner_start = 15;
static const int scanner_error = 0;

static const int scanner_en_c_comment = 13;
static const int scanner_en_main = 15;


#line 166 "/home/aifu/projects/schizo/src/lexer.rl"


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

	
#line 321 "/home/aifu/projects/schizo/src/lexer.cpp"
	{
	cs = scanner_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 259 "/home/aifu/projects/schizo/src/lexer.rl"

	
#line 332 "/home/aifu/projects/schizo/src/lexer.cpp"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_acts = _scanner_actions + _scanner_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 3:
#line 1 "NONE"
	{ts = p;}
	break;
#line 353 "/home/aifu/projects/schizo/src/lexer.cpp"
		}
	}

	_keys = _scanner_trans_keys + _scanner_key_offsets[cs];
	_trans = _scanner_index_offsets[cs];

	_klen = _scanner_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _scanner_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _scanner_indicies[_trans];
_eof_trans:
	cs = _scanner_trans_targs[_trans];

	if ( _scanner_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _scanner_actions + _scanner_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 76 "/home/aifu/projects/schizo/src/lexer.rl"
	{ ++line; }
	break;
	case 1:
#line 78 "/home/aifu/projects/schizo/src/lexer.rl"
	{ {cs = 15; goto _again;} }
	break;
	case 4:
#line 1 "NONE"
	{te = p+1;}
	break;
	case 5:
#line 83 "/home/aifu/projects/schizo/src/lexer.rl"
	{act = 3;}
	break;
	case 6:
#line 115 "/home/aifu/projects/schizo/src/lexer.rl"
	{act = 7;}
	break;
	case 7:
#line 117 "/home/aifu/projects/schizo/src/lexer.rl"
	{act = 8;}
	break;
	case 8:
#line 128 "/home/aifu/projects/schizo/src/lexer.rl"
	{act = 9;}
	break;
	case 9:
#line 143 "/home/aifu/projects/schizo/src/lexer.rl"
	{act = 11;}
	break;
	case 10:
#line 156 "/home/aifu/projects/schizo/src/lexer.rl"
	{act = 21;}
	break;
	case 11:
#line 164 "/home/aifu/projects/schizo/src/lexer.rl"
	{act = 25;}
	break;
	case 12:
#line 81 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE( boolean );}}
	break;
	case 13:
#line 82 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE( boolean );}}
	break;
	case 14:
#line 86 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( char );
									POP_TS();
									POP_TE();
								}}
	break;
	case 15:
#line 95 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( char );
									POP_TS();
									POP_TE();
								}}
	break;
	case 16:
#line 104 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( string );
									POP_TS();
									POP_TE();
								}}
	break;
	case 17:
#line 146 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( TOK_LPAR );}}
	break;
	case 18:
#line 147 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( TOK_RPAR );}}
	break;
	case 19:
#line 148 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( TOK_LBR );}}
	break;
	case 20:
#line 149 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( TOK_RBR );}}
	break;
	case 21:
#line 150 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( TOK_LSQB );}}
	break;
	case 22:
#line 152 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( TOK_LHASH );}}
	break;
	case 23:
#line 153 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( TOK_RHASH );}}
	break;
	case 24:
#line 154 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( TOK_SEMICOL );}}
	break;
	case 25:
#line 159 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;{ printf("unexpected character %c\n", *ts); }}
	break;
	case 26:
#line 163 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p+1;}
	break;
	case 27:
#line 115 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p;p--;{ ADVANCE( sint64 ); }}
	break;
	case 28:
#line 117 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p;p--;{
									fprintf(stderr, "Error: invalid number:\n    ");

									for( i = ts; i < te; ++i ) {
										fprintf(stderr, "%c", *i);
									}
									fprintf(stderr, "\n");
									cs	= scanner_error;
								}}
	break;
	case 29:
#line 128 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p;p--;{ ADVANCE( symbol );}}
	break;
	case 30:
#line 129 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p;p--;{
										copy_token(ts, te, tmp);

										if( strcmp("+", tmp) == 0 ) {
											ADVANCE_OP(symbol, TOK_OP_BIN3);
										} else if( strcmp("*", tmp) == 0 ) {
											ADVANCE_OP(symbol, TOK_OP_BIN0);
										} else {
											ADVANCE( symbol );
										}
								}}
	break;
	case 31:
#line 143 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p;p--;{ ADVANCE( real64 );}}
	break;
	case 32:
#line 151 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p;p--;{ ADVANCE_TOKEN( TOK_RSQB );}}
	break;
	case 33:
#line 159 "/home/aifu/projects/schizo/src/lexer.rl"
	{te = p;p--;{ printf("unexpected character %c\n", *ts); }}
	break;
	case 34:
#line 115 "/home/aifu/projects/schizo/src/lexer.rl"
	{{p = ((te))-1;}{ ADVANCE( sint64 ); }}
	break;
	case 35:
#line 129 "/home/aifu/projects/schizo/src/lexer.rl"
	{{p = ((te))-1;}{
										copy_token(ts, te, tmp);

										if( strcmp("+", tmp) == 0 ) {
											ADVANCE_OP(symbol, TOK_OP_BIN3);
										} else if( strcmp("*", tmp) == 0 ) {
											ADVANCE_OP(symbol, TOK_OP_BIN0);
										} else {
											ADVANCE( symbol );
										}
								}}
	break;
	case 36:
#line 1 "NONE"
	{	switch( act ) {
	case 3:
	{{p = ((te))-1;} fprintf(stderr, "INFIXL\n"); }
	break;
	case 7:
	{{p = ((te))-1;} ADVANCE( sint64 ); }
	break;
	case 8:
	{{p = ((te))-1;}
									fprintf(stderr, "Error: invalid number:\n    ");

									for( i = ts; i < te; ++i ) {
										fprintf(stderr, "%c", *i);
									}
									fprintf(stderr, "\n");
									cs	= scanner_error;
								}
	break;
	case 9:
	{{p = ((te))-1;} ADVANCE( symbol );}
	break;
	case 11:
	{{p = ((te))-1;} ADVANCE( real64 );}
	break;
	case 21:
	{{p = ((te))-1;} ++line; }
	break;
	default:
	{{p = ((te))-1;}}
	break;
	}
	}
	break;
#line 639 "/home/aifu/projects/schizo/src/lexer.cpp"
		}
	}

_again:
	_acts = _scanner_actions + _scanner_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 2:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 652 "/home/aifu/projects/schizo/src/lexer.cpp"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	if ( _scanner_eof_trans[cs] > 0 ) {
		_trans = _scanner_eof_trans[cs] - 1;
		goto _eof_trans;
	}
	}

	_out: {}
	}

#line 261 "/home/aifu/projects/schizo/src/lexer.rl"

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
