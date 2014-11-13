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
program		::= atom(B).				{ s->root = B; }
program		::= sexpr(B).				{ s->root = B; }

/* literals */
atom(A)		::= ATOM_SYMBOL(B).			{ A = B; }
atom(A)		::= ATOM_BOOL(B).			{ A = B; }
atom(A)		::= ATOM_CHAR(B).			{ A = B; }
atom(A)		::= ATOM_SINT64(B).			{ A = B; }
atom(A)		::= ATOM_REAL64(B).			{ A = B; }
atom(A)		::= ATOM_STRING(B).			{ A = B; }

/* NEVER USED: these are a hack to have the token ids */
sexpr		::= CELL_FREE.				/* a free cell */
sexpr		::= CELL_PAIR.				/* list */
sexpr		::= CELL_VECTOR.			/* a vector of cells */
sexpr		::= CELL_APPLY.				/* syntax/closure/foreign function interface */
sexpr		::= CELL_ENVIRONMENT.			/* environment */
sexpr		::= CELL_FRAME.				/* arguments */

/* ( ... ) */
sexpr(A)	::= LPAR members(B) RPAR.		{ A = list_reverse_in_place(s, B); }
sexpr(A)	::= LBR bexpr(B) RBR.			{ A = list_cons(s, atom_new_symbol(s, "begin"), list_reverse_in_place(s, B)); }
sexpr(A)	::= LBR bexpr(B) col RBR.			{ A = list_cons(s, atom_new_symbol(s, "begin"), list_reverse_in_place(s, B)); }

ilist(A)	::= atom(B).				{ A = B; }
ilist(A)	::= sexpr(B).				{ A = B; }

list(A)		::= ilist(B).				{ A = list_new(s, B); }
list(A)		::= list(B) ilist(C).			{ A = list_cons(s, C, B); }

members(A)	::=.					{ cell_id_t nil = { 0 }; A = list_new(s, nil); }
members(A)	::= list(B).				{ A = B; }

col		::= COL.
col		::= col COL.

bexpr(A)	::= .					{ cell_id_t nil = { 0 }; A = list_new(s, nil); }
bexpr(A)	::= list(B).				{ A = list_new(s, list_reverse_in_place(s, B)); }
bexpr(A)	::= bexpr(B) col list(C).		{ A = list_cons(s, list_reverse_in_place(s, C), B); }
bexpr		::= error.				{ fprintf(stderr, "Error: unexpected token: %d\n", yyruleno); }
