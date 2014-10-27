
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "ast.h"

#define ADVANCE(A)	state->token_start	= ts; \
			state->token_end	= te; \
			state->token_line	= line; \
			copy_token(ts, te, tmp); \
			state->current_cell	= token_to_##A(tmp); \
			parser_advance(parser, state->current_cell->type, state->current_cell, state)

#define ADVANCE_TOKEN(A)	parser_advance(parser, A, NULL, state)

#define PUSH_TE()	const char* tmp_te = te
#define POP_TE()	te	= tmp_te
#define PUSH_TS()	const char* tmp_ts = ts
#define POP_TS()	ts	= tmp_ts

/* EOF char used to flush out that last token. This should be a whitespace
 * token. */

#define LAST_CHAR 0

extern void*	parser_alloc(void *(*mallocProc)(size_t));
extern void	parser_free(void *p, void (*freeProc)(void*));
extern void	parser_advance(void *yyp, int yymajor, cell_t* yyminor, state_t* state);


#line 58 "/home/kin/Projects/schizo/lexer.c"
static const char _scanner_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 19, 1, 20, 1, 
	21, 1, 22, 1, 23, 1, 24, 1, 
	25, 1, 27, 1, 28, 1, 29, 1, 
	30, 1, 31, 1, 32, 1, 33, 1, 
	34, 1, 35, 1, 36, 2, 0, 26, 
	2, 4, 5, 2, 4, 6, 2, 4, 
	7, 2, 4, 8, 2, 4, 9, 2, 
	4, 10, 2, 4, 11, 2, 4, 12, 
	2, 4, 13, 2, 4, 14, 2, 4, 
	15, 2, 4, 16, 2, 4, 17, 2, 
	4, 18
};

static const short _scanner_key_offsets[] = {
	0, 0, 3, 3, 5, 6, 13, 14, 
	17, 21, 23, 24, 29, 31, 34, 70, 
	72, 85, 101, 109, 114, 124, 126, 132, 
	136, 149, 163, 177, 186, 194, 208, 215, 
	223, 231, 239, 247, 255, 263, 271
};

static const char _scanner_trans_keys[] = {
	10, 34, 92, 39, 92, 39, 39, 98, 
	110, 114, 116, 48, 57, 39, 39, 48, 
	57, 43, 45, 48, 57, 48, 57, 10, 
	46, 69, 101, 48, 57, 10, 42, 10, 
	42, 47, 10, 33, 34, 39, 40, 41, 
	44, 46, 47, 48, 59, 64, 94, 95, 
	102, 116, 124, 126, 35, 36, 37, 42, 
	43, 45, 49, 57, 58, 63, 65, 90, 
	91, 96, 97, 122, 123, 125, 33, 126, 
	33, 45, 47, 58, 94, 124, 126, 37, 
	38, 42, 43, 60, 63, 33, 45, 47, 
	48, 58, 94, 124, 126, 37, 38, 42, 
	43, 49, 57, 60, 63, 95, 108, 115, 
	121, 65, 90, 97, 122, 95, 65, 90, 
	97, 122, 95, 108, 115, 121, 48, 57, 
	65, 90, 97, 122, 48, 57, 69, 70, 
	101, 102, 48, 57, 70, 102, 48, 57, 
	33, 45, 47, 58, 94, 124, 126, 37, 
	38, 42, 43, 60, 63, 10, 33, 45, 
	47, 58, 94, 124, 126, 37, 38, 42, 
	43, 60, 63, 46, 69, 95, 101, 108, 
	115, 117, 121, 48, 57, 65, 90, 97, 
	122, 43, 45, 95, 48, 57, 65, 90, 
	97, 122, 95, 108, 115, 121, 65, 90, 
	97, 122, 46, 69, 95, 101, 108, 115, 
	117, 121, 48, 57, 65, 90, 97, 122, 
	95, 48, 57, 65, 90, 97, 122, 95, 
	97, 48, 57, 65, 90, 98, 122, 95, 
	108, 48, 57, 65, 90, 97, 122, 95, 
	115, 48, 57, 65, 90, 97, 122, 95, 
	101, 48, 57, 65, 90, 97, 122, 95, 
	114, 48, 57, 65, 90, 97, 122, 95, 
	117, 48, 57, 65, 90, 97, 122, 95, 
	101, 48, 57, 65, 90, 97, 122, 0
};

static const char _scanner_single_lengths[] = {
	0, 3, 0, 2, 1, 5, 1, 1, 
	2, 0, 1, 3, 2, 3, 18, 0, 
	7, 8, 4, 1, 4, 0, 4, 2, 
	7, 8, 8, 3, 4, 8, 1, 2, 
	2, 2, 2, 2, 2, 2, 0
};

static const char _scanner_range_lengths[] = {
	0, 0, 0, 0, 0, 1, 0, 1, 
	1, 1, 0, 1, 0, 0, 9, 1, 
	3, 4, 2, 2, 3, 1, 1, 1, 
	3, 3, 3, 3, 2, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 0
};

static const short _scanner_index_offsets[] = {
	0, 0, 4, 5, 8, 10, 17, 19, 
	22, 26, 28, 30, 35, 38, 42, 70, 
	72, 83, 96, 103, 107, 115, 117, 123, 
	127, 138, 150, 162, 169, 176, 188, 193, 
	199, 205, 211, 217, 223, 229, 235
};

static const char _scanner_indicies[] = {
	1, 2, 3, 0, 0, 1, 5, 4, 
	6, 1, 7, 7, 7, 7, 7, 8, 
	1, 9, 1, 9, 8, 1, 11, 11, 
	12, 10, 12, 10, 15, 14, 17, 19, 
	19, 18, 16, 21, 22, 20, 21, 22, 
	23, 20, 25, 26, 0, 28, 29, 30, 
	27, 32, 33, 34, 27, 27, 26, 36, 
	37, 38, 26, 26, 27, 26, 31, 35, 
	26, 36, 27, 36, 27, 24, 10, 24, 
	26, 26, 26, 26, 26, 26, 26, 26, 
	26, 26, 39, 26, 26, 26, 40, 26, 
	26, 26, 26, 26, 26, 41, 26, 39, 
	43, 44, 45, 46, 43, 43, 42, 43, 
	43, 43, 10, 43, 44, 45, 46, 41, 
	43, 43, 42, 17, 47, 19, 49, 19, 
	49, 17, 48, 49, 49, 12, 48, 26, 
	26, 50, 26, 26, 26, 26, 26, 26, 
	26, 39, 15, 50, 50, 50, 50, 50, 
	50, 50, 50, 50, 50, 14, 17, 51, 
	43, 51, 44, 45, 52, 46, 18, 43, 
	43, 42, 11, 11, 43, 12, 43, 43, 
	53, 43, 55, 56, 57, 43, 43, 54, 
	17, 51, 43, 51, 44, 45, 52, 46, 
	35, 43, 43, 42, 36, 36, 36, 36, 
	10, 36, 59, 36, 36, 36, 58, 36, 
	60, 36, 36, 36, 58, 36, 61, 36, 
	36, 36, 58, 36, 62, 36, 36, 36, 
	58, 36, 63, 36, 36, 36, 58, 36, 
	64, 36, 36, 36, 58, 36, 65, 36, 
	36, 36, 58, 1, 0
};

static const char _scanner_trans_targs[] = {
	1, 0, 14, 2, 4, 5, 14, 6, 
	7, 14, 14, 9, 23, 14, 10, 14, 
	14, 22, 11, 8, 12, 12, 13, 38, 
	15, 15, 16, 14, 3, 14, 14, 17, 
	21, 24, 26, 29, 30, 31, 35, 14, 
	18, 20, 14, 19, 19, 19, 19, 14, 
	14, 14, 25, 27, 28, 14, 14, 19, 
	19, 19, 14, 32, 33, 34, 30, 36, 
	37, 30
};

static const char _scanner_trans_actions[] = {
	0, 0, 15, 0, 0, 0, 11, 0, 
	0, 13, 43, 0, 0, 39, 0, 45, 
	41, 57, 0, 0, 0, 1, 0, 3, 
	87, 84, 0, 23, 0, 19, 21, 0, 
	0, 0, 63, 0, 54, 0, 0, 27, 
	0, 0, 31, 81, 60, 66, 69, 37, 
	29, 17, 9, 81, 0, 35, 33, 72, 
	75, 78, 25, 0, 0, 0, 51, 0, 
	0, 48
};

static const char _scanner_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 5, 0, 5, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const char _scanner_from_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 7, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const short _scanner_eof_trans[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	11, 11, 14, 17, 0, 0, 0, 11, 
	40, 40, 43, 11, 43, 48, 49, 49, 
	40, 40, 43, 54, 55, 43, 11, 59, 
	59, 59, 59, 59, 59, 59, 0
};

static const int scanner_start = 14;
static const int scanner_error = 0;

static const int scanner_en_c_comment = 12;
static const int scanner_en_main = 14;


#line 192 "/home/kin/Projects/schizo/lexer.rl"


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

static cell_t*
token_to_symbol(const char* b) {
	return cell_new_symbol(b);
}

static cell_t*
token_to_boolean(const char* b) {
	if( !strcmp(b, "true") ) {
		return cell_new_boolean(TRUE);
	} else {
		return cell_new_boolean(FALSE);
	}
}

static cell_t*
token_to_char(const char* b) {
	return cell_new_char(*b);
}

static cell_t*
token_to_sint8(const char* b) {
	sint64	v	= 0;
	sscanf(b, "%ld", &v);
	/* TODO: check limit */
	return cell_new_sint8(v);
}

static cell_t*
token_to_sint16(const char* b) {
	sint64	v	= 0;
	sscanf(b, "%ld", &v);
	/* TODO: check limit */
	return cell_new_sint16(v);
}

static cell_t*
token_to_sint32(const char* b) {
	sint64	v	= 0;
	sscanf(b, "%ld", &v);
	/* TODO: check limit */
	return cell_new_sint32(v);
}

static cell_t*
token_to_sint64(const char* b) {
	sint64	v	= 0;
	sscanf(b, "%ld", &v);
	/* TODO: check limit */
	return cell_new_sint64(v);
}

static cell_t*
token_to_uint8(const char* b) {
	uint64	v	= 0;
	sscanf(b, "%lu", &v);
	/* TODO: check limit */
	return cell_new_uint8(v);
}

static cell_t*
token_to_uint16(const char* b) {
	uint64	v	= 0;
	sscanf(b, "%lu", &v);
	/* TODO: check limit */
	return cell_new_uint16(v);
}

static cell_t*
token_to_uint32(const char* b) {
	uint64	v	= 0;
	sscanf(b, "%lu", &v);
	/* TODO: check limit */
	return cell_new_uint32(v);
}

static cell_t*
token_to_uint64(const char* b) {
	uint64	v	= 0;
	sscanf(b, "%lu", &v);
	/* TODO: check limit */
	return cell_new_uint64(v);
}

static cell_t*
token_to_real32(const char* b) {
	real64	v	= 0;
	sscanf(b, "%lf", &v);
	/* TODO: check limit */
	return cell_new_real32(v);
}

static cell_t*
token_to_real64(const char* b) {
	real64	v	= 0;
	sscanf(b, "%lf", &v);
	/* TODO: check limit */
	return cell_new_real64(v);
}

static cell_t*
token_to_string(const char* b) {
	return cell_new_string(b);
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

	memset(state, 0, sizeof(state_t));

	parser	= parser_alloc(malloc);

	memset(tmp, 0, sizeof(tmp));

	
#line 369 "/home/kin/Projects/schizo/lexer.c"
	{
	cs = scanner_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 329 "/home/kin/Projects/schizo/lexer.rl"

	const char*	p = str;
	const char*	pe = p + strlen(str) + 1;
	const char*	eof = 0;

	
#line 384 "/home/kin/Projects/schizo/lexer.c"
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
#line 405 "/home/kin/Projects/schizo/lexer.c"
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
#line 67 "/home/kin/Projects/schizo/lexer.rl"
	{ ++line; }
	break;
	case 1:
#line 69 "/home/kin/Projects/schizo/lexer.rl"
	{ {cs = 14; goto _again;} }
	break;
	case 4:
#line 1 "NONE"
	{te = p+1;}
	break;
	case 5:
#line 72 "/home/kin/Projects/schizo/lexer.rl"
	{act = 1;}
	break;
	case 6:
#line 73 "/home/kin/Projects/schizo/lexer.rl"
	{act = 2;}
	break;
	case 7:
#line 104 "/home/kin/Projects/schizo/lexer.rl"
	{act = 6;}
	break;
	case 8:
#line 109 "/home/kin/Projects/schizo/lexer.rl"
	{act = 9;}
	break;
	case 9:
#line 113 "/home/kin/Projects/schizo/lexer.rl"
	{act = 10;}
	break;
	case 10:
#line 120 "/home/kin/Projects/schizo/lexer.rl"
	{act = 11;}
	break;
	case 11:
#line 122 "/home/kin/Projects/schizo/lexer.rl"
	{act = 12;}
	break;
	case 12:
#line 129 "/home/kin/Projects/schizo/lexer.rl"
	{act = 13;}
	break;
	case 13:
#line 136 "/home/kin/Projects/schizo/lexer.rl"
	{act = 14;}
	break;
	case 14:
#line 150 "/home/kin/Projects/schizo/lexer.rl"
	{act = 16;}
	break;
	case 15:
#line 157 "/home/kin/Projects/schizo/lexer.rl"
	{act = 17;}
	break;
	case 16:
#line 164 "/home/kin/Projects/schizo/lexer.rl"
	{act = 18;}
	break;
	case 17:
#line 182 "/home/kin/Projects/schizo/lexer.rl"
	{act = 21;}
	break;
	case 18:
#line 190 "/home/kin/Projects/schizo/lexer.rl"
	{act = 25;}
	break;
	case 19:
#line 76 "/home/kin/Projects/schizo/lexer.rl"
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
#line 85 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( char );
									POP_TS();
									POP_TE();
								}}
	break;
	case 21:
#line 94 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{
									PUSH_TE();
									PUSH_TS();
									++ts; --te;
									ADVANCE( string );
									POP_TS();
									POP_TE();
								}}
	break;
	case 22:
#line 108 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE( real32 );}}
	break;
	case 23:
#line 179 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( LPAR );}}
	break;
	case 24:
#line 180 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ ADVANCE_TOKEN( RPAR );}}
	break;
	case 25:
#line 185 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;{ printf("unexpected character %c\n", *ts); }}
	break;
	case 26:
#line 189 "/home/kin/Projects/schizo/lexer.rl"
	{te = p+1;}
	break;
	case 27:
#line 104 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( symbol );}}
	break;
	case 28:
#line 105 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( symbol );}}
	break;
	case 29:
#line 109 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( real64 );}}
	break;
	case 30:
#line 120 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ ADVANCE( sint32 );}}
	break;
	case 31:
#line 143 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{
									PUSH_TE();
									--te;
									ADVANCE( uint32 );
									POP_TE();
								}}
	break;
	case 32:
#line 164 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{
									fprintf(stderr, "invalid character trailer:\n    ");

									for( const char* i = ts; i < te; ++i ) {
										fprintf(stderr, "%c", *i);
									}
									fprintf(stderr, "\n");
									PUSH_TE();
									--te;
									ADVANCE( sint8 );
									POP_TE();
								}}
	break;
	case 33:
#line 185 "/home/kin/Projects/schizo/lexer.rl"
	{te = p;p--;{ printf("unexpected character %c\n", *ts); }}
	break;
	case 34:
#line 105 "/home/kin/Projects/schizo/lexer.rl"
	{{p = ((te))-1;}{ ADVANCE( symbol );}}
	break;
	case 35:
#line 120 "/home/kin/Projects/schizo/lexer.rl"
	{{p = ((te))-1;}{ ADVANCE( sint32 );}}
	break;
	case 36:
#line 1 "NONE"
	{	switch( act ) {
	case 1:
	{{p = ((te))-1;} ADVANCE( boolean );}
	break;
	case 2:
	{{p = ((te))-1;} ADVANCE( boolean );}
	break;
	case 6:
	{{p = ((te))-1;} ADVANCE( symbol );}
	break;
	case 9:
	{{p = ((te))-1;} ADVANCE( real64 );}
	break;
	case 10:
	{{p = ((te))-1;}
									PUSH_TE();
									--te;
									ADVANCE( sint64 );
									POP_TE();
								}
	break;
	case 11:
	{{p = ((te))-1;} ADVANCE( sint32 );}
	break;
	case 12:
	{{p = ((te))-1;}
									PUSH_TE();
									--te;
									ADVANCE( sint16 );
									POP_TE();
								}
	break;
	case 13:
	{{p = ((te))-1;}
									PUSH_TE();
									--te;
									ADVANCE( sint8 );
									POP_TE();
								}
	break;
	case 14:
	{{p = ((te))-1;}
									PUSH_TE();
									te -= 2;
									ADVANCE( uint64 );
									POP_TE();
								}
	break;
	case 16:
	{{p = ((te))-1;}
									PUSH_TE();
									te -= 2;
									ADVANCE( uint16 );
									POP_TE();
								}
	break;
	case 17:
	{{p = ((te))-1;}
									PUSH_TE();
									te -= 2;
									ADVANCE( uint8 );
									POP_TE();
								}
	break;
	case 18:
	{{p = ((te))-1;}
									fprintf(stderr, "invalid character trailer:\n    ");

									for( const char* i = ts; i < te; ++i ) {
										fprintf(stderr, "%c", *i);
									}
									fprintf(stderr, "\n");
									PUSH_TE();
									--te;
									ADVANCE( sint8 );
									POP_TE();
								}
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
#line 733 "/home/kin/Projects/schizo/lexer.c"
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
#line 746 "/home/kin/Projects/schizo/lexer.c"
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

#line 335 "/home/kin/Projects/schizo/lexer.rl"

	/* Check if we failed. */
	if ( cs == scanner_error ) {
		/* Machine failed before finding a token. */
		printf("PARSE ERROR\n");
		exit(1);
	}

	parser_advance(parser, 0, 0, state);

	parser_free(parser, free);

	return state;

}
