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
 grammar parser: using Lemon, not YACC nor BISON
*/
%name parser

%token_type	{ cell_id_t }
%extra_argument { state_t* s }

%include {
#include <stdio.h>
#include "schizo.h"
}

%type cell	{ cell_id_t }
%type program	{ cell_id_t }
%type cell_list	{ cell_id_t }

%syntax_error {
	int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
	for (int i = 0; i < n; ++i) {
		int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
		if (a < YYNSTATE + YYNRULE) {
			printf("possible token: %s\n", yyTokenName[i]);
		}
	}
}

%start_symbol program

/* a program is a cell */
program		::= cell(B).				{ s->root = B; }

/* literals */
cell(A)		::= CELL_SYMBOL(B).			{ A = B; }
cell(A)		::= CELL_BOOL(B).			{ A = B; }
cell(A)		::= CELL_CHAR(B).			{ A = B; }
cell(A)		::= CELL_SINT64(B).			{ A = B; }
cell(A)		::= CELL_REAL64(B).			{ A = B; }
cell(A)		::= CELL_STRING(B).			{ A = B; }

/* NEVER USED: these are a hack to have the token ids */
cell		::= CELL_FREE.				/* a free cell */
cell		::= CELL_PAIR.				/* list */
cell		::= CELL_VECTOR.			/* a vector of cells */
cell		::= CELL_APPLY.				/* syntax/closure/foreign function interface */
cell		::= CELL_ENVIRONMENT.			/* environment */
cell		::= CELL_FRAME.				/* arguments */

/* ( ... ) */
cell(A)		::= LPAR cell_members(B) RPAR.		{ A = cell_reverse_in_place(s, B); }

cell_list(A)	::= cell(B).				{ A = cell_new_pair(s, B); }
cell_list(A)	::= cell_list(B) cell(C).		{ A = cell_cons(s, C, B); }

cell_members(A)	::=.					{ cell_id_t nil = { 0 }; A = cell_new_pair(s, nil); }
cell_members(A)	::= cell_list(B).			{ A = B; }
