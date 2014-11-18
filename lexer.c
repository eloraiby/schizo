
#line 1 "/home/kin/Projects/schizo/lexer.rl"
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

#define ADVANCE(A)	state->token_start	= ts; \
			state->token_end	= te; \
			state->token_line	= line; \
			copy_token(ts, te, tmp); \
			state->current_cell	= token_to_##A(state, tmp); \
			parser_advance(parser, state->gc_block.cells[state->current_cell.index].type, state->current_cell, state)

#define ADVANCE_TOKEN(A)	parser_advance(parser, A, nil, state)

#define PUSH_TE()	const char* tmp_te = te
#define POP_TE()	te	= tmp_te
#define PUSH_TS()	const char* tmp_ts = ts
#define POP_TS()	ts	= tmp_ts

/* EOF char used to flush out that last token. This should be a whitespace
 * token. */

#define LAST_CHAR 0

extern void*	parser_alloc(void *(*mallocProc)(size_t));
extern void	parser_free(void *p, void (*freeProc)(void*));
extern void	parser_advance(void *yyp, int yymajor, cell_id_t yyminor, state_t* state);


#line 56 "/home/kin/Projects/schizo/lexer.c"
static const char _scanner_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 16, 1, 17, 1, 
	18, 1, 19, 1, 20, 1, 21, 1, 
	22, 1, 23, 1, 24, 1, 25, 1, 
	26, 1, 27, 1, 28, 1, 30, 1, 
	31, 1, 32, 1, 33, 1, 34, 1, 
	35, 1, 36, 1, 37, 1, 38, 1, 
	39, 1, 40, 2, 0, 29, 2, 4, 
	5, 2, 4, 6, 2, 4, 7, 2, 
	4, 8, 2, 4, 9, 2, 4, 10, 
	2, 4, 11, 2, 4, 12, 2, 4, 
	13, 2, 4, 14, 2, 4, 15
};

static const unsigned char _scanner_key_offsets[] = {
	0, 0, 3, 3, 5, 6, 13, 14, 
	17, 19, 23, 25, 30, 31, 33, 36, 
	69, 71, 84, 86, 103, 107, 109, 119, 
	124, 133, 143, 160, 162, 175, 189, 196
};

static const char _scanner_trans_keys[] = {
	10, 34, 92, 39, 92, 39, 39, 98, 
	110, 114, 116, 48, 57, 39, 39, 48, 
	57, 48, 57, 43, 45, 48, 57, 48, 
	57, 46, 69, 101, 48, 57, 10, 10, 
	42, 10, 42, 47, 10, 33, 34, 35, 
	36, 39, 40, 41, 43, 44, 45, 46, 
	47, 48, 59, 64, 91, 92, 93, 94, 
	96, 123, 125, 37, 42, 49, 57, 58, 
	63, 65, 122, 124, 126, 33, 126, 33, 
	45, 47, 58, 94, 124, 126, 37, 38, 
	42, 43, 60, 63, 102, 116, 33, 42, 
	43, 46, 48, 58, 94, 124, 126, 37, 
	38, 45, 47, 49, 57, 60, 63, 69, 
	101, 48, 57, 48, 57, 46, 69, 95, 
	101, 48, 57, 65, 90, 97, 122, 95, 
	65, 90, 97, 122, 43, 45, 95, 48, 
	57, 65, 90, 97, 122, 46, 69, 95, 
	101, 48, 57, 65, 90, 97, 122, 33, 
	45, 46, 47, 48, 58, 94, 124, 126, 
	37, 38, 42, 43, 49, 57, 60, 63, 
	48, 57, 33, 45, 47, 58, 94, 124, 
	126, 37, 38, 42, 43, 60, 63, 10, 
	33, 45, 47, 58, 94, 124, 126, 37, 
	38, 42, 43, 60, 63, 95, 48, 57, 
	65, 90, 97, 122, 0
};

static const char _scanner_single_lengths[] = {
	0, 3, 0, 2, 1, 5, 1, 1, 
	0, 2, 0, 3, 1, 2, 3, 23, 
	0, 7, 2, 9, 2, 0, 4, 1, 
	3, 4, 9, 0, 7, 8, 1, 0
};

static const char _scanner_range_lengths[] = {
	0, 0, 0, 0, 0, 1, 0, 1, 
	1, 1, 1, 1, 0, 0, 0, 5, 
	1, 3, 0, 4, 1, 1, 3, 2, 
	3, 3, 4, 1, 3, 3, 3, 0
};

static const unsigned char _scanner_index_offsets[] = {
	0, 0, 4, 5, 8, 10, 17, 19, 
	22, 24, 28, 30, 35, 37, 40, 44, 
	73, 75, 86, 89, 103, 107, 109, 117, 
	121, 128, 136, 150, 152, 163, 175, 180
};

static const char _scanner_indicies[] = {
	1, 2, 3, 0, 0, 1, 5, 4, 
	6, 1, 7, 7, 7, 7, 7, 8, 
	1, 9, 1, 9, 8, 1, 11, 10, 
	12, 12, 13, 10, 13, 10, 11, 16, 
	16, 15, 14, 19, 18, 21, 22, 20, 
	21, 22, 23, 20, 25, 26, 0, 27, 
	28, 30, 31, 32, 33, 28, 34, 35, 
	36, 37, 39, 28, 41, 28, 42, 29, 
	28, 43, 44, 29, 38, 29, 40, 29, 
	24, 10, 24, 29, 29, 29, 29, 29, 
	29, 29, 29, 29, 29, 10, 46, 47, 
	45, 29, 29, 49, 50, 37, 29, 29, 
	29, 29, 29, 29, 38, 29, 48, 16, 
	16, 11, 51, 13, 51, 11, 54, 53, 
	54, 15, 53, 53, 52, 53, 53, 53, 
	55, 12, 12, 53, 13, 53, 53, 55, 
	11, 54, 53, 54, 38, 53, 53, 52, 
	29, 57, 50, 29, 37, 29, 29, 29, 
	29, 29, 29, 38, 29, 56, 11, 45, 
	29, 29, 59, 29, 29, 29, 29, 29, 
	29, 29, 58, 19, 59, 59, 59, 59, 
	59, 59, 59, 59, 59, 59, 18, 40, 
	40, 40, 40, 60, 1, 0
};

static const char _scanner_trans_targs[] = {
	1, 0, 15, 2, 4, 5, 15, 6, 
	7, 15, 15, 20, 10, 21, 15, 11, 
	9, 15, 12, 15, 13, 13, 14, 31, 
	16, 16, 17, 18, 15, 17, 3, 15, 
	15, 19, 26, 27, 28, 22, 25, 15, 
	30, 15, 15, 15, 15, 15, 15, 15, 
	15, 17, 8, 15, 15, 23, 24, 15, 
	15, 17, 15, 29, 15
};

static const char _scanner_trans_actions[] = {
	0, 0, 19, 0, 0, 0, 15, 0, 
	0, 17, 57, 80, 0, 0, 55, 0, 
	0, 53, 0, 59, 0, 1, 0, 3, 
	92, 89, 62, 0, 35, 77, 0, 21, 
	23, 71, 74, 0, 0, 83, 0, 33, 
	0, 29, 31, 25, 27, 51, 13, 11, 
	37, 65, 0, 45, 47, 0, 86, 49, 
	39, 68, 43, 9, 41
};

static const char _scanner_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 5, 0, 5, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0
};

static const char _scanner_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 7, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0
};

static const unsigned char _scanner_eof_trans[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	11, 11, 11, 15, 18, 0, 0, 0, 
	11, 11, 46, 49, 52, 52, 53, 56, 
	56, 53, 57, 46, 59, 59, 61, 0
};

static const int scanner_start = 15;
static const int scanner_error = 0;

static const int scanner_en_c_comment = 13;
static const int scanner_en_main = 15;


#line 151 "/home/kin/Projects/schizo/lexer.rl"


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

static cell_id_t
token_to_symbol(state_t* s, const char* b) {
	return atom_new_symbol(s, b);
}

static cell_id_t
token_to_unary_op(state_t* s, const char* op) {
	return atom_new_unary_op(s, op);
}

static cell_id_t
token_to_binary_op(state_t* s, const char* op) {
	return atom_new_binary_op(s, op);
}

static cell_id_t
token_to_boolean(state_t* s, const char* b) {
	if( !strcmp(b, "#t") ) {
		return atom_new_boolean(s, true);
	} else {
		return atom_new_boolean(s, false);
	}
}

static cell_id_t
token_to_char(state_t* s, const char* b) {
	return atom_new_char(s, *b);
}

static cell_id_t
token_to_sint64(state_t* s, const char* b) {
	sint64	v	= 0;
	sscanf(b, "%ld", &v);
	/* TODO: check limit */
	return atom_new_sint64(s, v);
}

static cell_id_t
token_to_real64(state_t* s, const char* b) {
	real64	v	= 0;
	sscanf(b, "%lf", &v);
	/* TODO: check limit */
	return atom_new_real64(s, v);
}

static cell_id_t
token_to_string(state_t* s, const char* b) {
	return atom_new_string(s, b);
}


state_t*
parse(const char* str)
{
	state_t*	state	= (state_t*)malloc(sizeof(state_t));
	void*		parser;
	size_t		line	= 1;
	const char*	ts	= str;
	const char*	te	= str;
	int		act	= 0;
	int		cs	= 0;
	char		tmp[4096];
	cell_id_t	nil	= { 0 };

	memset(state, 0, sizeof(state_t));

	parser	= parser_alloc(malloc);

	memset(tmp, 0, sizeof(tmp));

	
#line 289 "/home/kin/Projects/schizo/lexer.c"
	{
	cs = scanner_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 235 "/home/kin/Projects/schizo/lexer.rl"

	const char*	p = str;
	const char*	pe = p + strlen(str) + 1;
	const char*	eof = 0;

	
#line 304 "/home/kin/Projects/schizo/lexer.c"
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
#line 325 "/home/kin/Projects/schizo/lexer.c"
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
#line 64 "/home/kin/Projects/schizo/lexer.rl"
	{ ++line; }
	break;
	case 1:
#line 66 "/home/kin/Projects/schizo/lexer.rl"
	{ {cs = 15; goto _again;} }
	break;
	case 4:
#line 1 "NONE"
	{te = p+1;}
	break;
	case 5:
#line 74 "/home/kin/Projects/schizo/lexer.rl"
	{act = 3;}
	break;
	case 6:
#line 75 "/home/kin/Projects/schizo/lexer.rl"
	{act = 4;}
	break;
	case 7:
#line 76 "/home/kin/Projects/schizo/lexer.rl"
	{act = 5;}
	break;
	case 8:
#line 78 "/home/kin/Projects/schizo/lexer.rl"
	{act = 6;}
	break;
	case 9:
#line 79 "/home/kin/Projects/schizo/lexer.rl"
	{act = 7;}
	break;
	case 10:
#line 111 "/home/kin/Projects/schizo/lexer.rl"
	{act = 12;}
	break;
	case 11:
#line 114 "/home/kin/Projects/schizo/lexer.rl"
	{act = 13;}
	break;
	case 12:
#line 118 "/home/kin/Projects/schizo/lexer.rl"
	{act = 14;}
	break;
	case 13:
#line 120 "/home/kin/Projects/schizo/lexer.rl"
	{act = 15;}
	break;
	case 14:
#line 141 "/home/kin/Projects/schizo/lexer.rl"
	{act = 23;}
	break;
	case 15:
#line 149 "/home/kin/Projects/schizo/lexer.rl"
	{act = 27;}
	break;
	case 16:
#line 69 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE( boolean );}}
	break;
	case 17:
#line 70 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE( boolean );}}
	break;
	case 18:
#line 82 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( char );
									POP_TS();
									POP_TE();
								}}
	break;
	case 19:
#line 91 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( char );
									POP_TS();
									POP_TE();
								}}
	break;
	case 20:
#line 100 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( string );
									POP_TS();
									POP_TE();
								}}
	break;
	case 21:
#line 132 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( LPAR );}}
	break;
	case 22:
#line 133 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( RPAR );}}
	break;
	case 23:
#line 134 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( LBR );}}
	break;
	case 24:
#line 135 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( RBR );}}
	break;
	case 25:
#line 136 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( LSQB );}}
	break;
	case 26:
#line 137 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( RSQB );}}
	break;
	case 27:
#line 138 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( COL );}}
	break;
	case 28:
#line 144 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ printf("unexpected character %c\n", *ts); }}
	break;
	case 29:
#line 148 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;}
	break;
	case 30:
#line 78 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( binary_op ); }}
	break;
	case 31:
#line 79 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( binary_op ); }}
	break;
	case 32:
#line 110 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( symbol );}}
	break;
	case 33:
#line 111 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( symbol );}}
	break;
	case 34:
#line 114 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( real64 );}}
	break;
	case 35:
#line 118 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( sint64 ); }}
	break;
	case 36:
#line 120 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{
									fprintf(stderr, "Error: invalid number:\n    ");

									for( const char* i = ts; i < te; ++i ) {
										fprintf(stderr, "%c", *i);
									}
									fprintf(stderr, "\n");
									cs	= scanner_error;
								}}
	break;
	case 37:
#line 144 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ printf("unexpected character %c\n", *ts); }}
	break;
	case 38:
#line 111 "/home/kin/Projects/schizo/lexer.rl"
	{{p = ((te))-1;}{ ADVANCE( symbol );}}
	break;
	case 39:
#line 118 "/home/kin/Projects/schizo/lexer.rl"
	{{p = ((te))-1;}{ ADVANCE( sint64 ); }}
	break;
	case 40:
#line 1 "NONE"
	{	switch( act ) {
	case 3:
	{{p = ((te))-1;} ADVANCE( unary_op ); }
	break;
	case 4:
	{{p = ((te))-1;} ADVANCE( unary_op ); }
	break;
	case 5:
	{{p = ((te))-1;} ADVANCE( unary_op ); }
	break;
	case 6:
	{{p = ((te))-1;} ADVANCE( binary_op ); }
	break;
	case 7:
	{{p = ((te))-1;} ADVANCE( binary_op ); }
	break;
	case 12:
	{{p = ((te))-1;} ADVANCE( symbol );}
	break;
	case 13:
	{{p = ((te))-1;} ADVANCE( real64 );}
	break;
	case 14:
	{{p = ((te))-1;} ADVANCE( sint64 ); }
	break;
	case 15:
	{{p = ((te))-1;}
									fprintf(stderr, "Error: invalid number:\n    ");

									for( const char* i = ts; i < te; ++i ) {
										fprintf(stderr, "%c", *i);
									}
									fprintf(stderr, "\n");
									cs	= scanner_error;
								}
	break;
	case 23:
	{{p = ((te))-1;} ++line; }
	break;
	default:
	{{p = ((te))-1;}}
	break;
	}
	}
	break;
#line 619 "/home/kin/Projects/schizo/lexer.c"
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
#line 632 "/home/kin/Projects/schizo/lexer.c"
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

#line 241 "/home/kin/Projects/schizo/lexer.rl"

	/* Check if we failed. */
	if ( cs == scanner_error ) {
		/* Machine failed before finding a token. */
		printf("PARSE ERROR\n");
		exit(1);
	}

	parser_advance(parser, 0, nil, state);

	parser_free(parser, free);

	return state;

}
