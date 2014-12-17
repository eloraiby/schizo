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

%token_type	{ cell_ptr_t }
%extra_argument { state_t* s }

%include {
#include <stdio.h>
#include "schizo.h"
}

%type program	{ cell_ptr_t }
%type cell_list	{ cell_ptr_t }

/*
%left BIN_OP3
%left BIN_OP2
%left BIN_OP1
%left BIN_OP0

%left UN_OP3
%left UN_OP2
%left UN_OP1
%left UN_OP0
*/

%syntax_error {
	int i = 0;
	int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
	for ( i = 0; i < n; ++i) {
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

/* NEVER USED in the parser: these are a hack to have the token ids */
sexpr		::= CELL_FREE.				/* a free cell */
sexpr		::= CELL_PAIR.				/* list */
sexpr		::= CELL_VECTOR.			/* a vector of cells */
sexpr		::= CELL_CLOSURE.			/* closure */
sexpr		::= CELL_FFI.				/* foreign function interface */
sexpr		::= CELL_LAMBDA.			/* lambda */
sexpr		::= CELL_QUOTE.				/* quote (this should could have been replaced with objects, but will increase the complexity of the evaluator) */
sexpr		::= CELL_BIND.				/* bind symbol */
sexpr		::= ATOM_ERROR.				/* error */

/* ( ... ) */
sexpr(A)	::= LPAR se_members(B) RPAR.		{ A = B; }
sexpr(A)	::= LBR member_list(B) RBR.		{ A = list_cons(s, atom_new_symbol(s, "scope"), ( cell_type(s, B) == CELL_PAIR ) ? list_reverse_in_place(s, B) : B); }
sexpr(A)	::= LBR member_list(B) sc RBR.		{ A = list_cons(s, atom_new_symbol(s, "scope"), ( cell_type(s, B) == CELL_PAIR ) ? list_reverse_in_place(s, B) : B); }
sexpr(A)	::= ilist(B) LSQB member_list(C) RSQB.	{ A = list_cons(s, atom_new_symbol(s, "item"),
									   list_cons(s, B, ( cell_type(s, C) == CELL_PAIR ) ? list_reverse_in_place(s, C) : C)); }
ilist(A)	::= atom(B).				{ A = B; }
ilist(A)	::= sexpr(B).				{ A = B; }

list(A)		::= ilist(B).				{ A = list_new(s, B); }
list(A)		::= list(B) ilist(C).			{ A = list_cons(s, C, B); }

se_members(A)	::=.					{ A = list_new(s, NIL_CELL); }
se_members(A)	::= list(B).				{ A = list_reverse_in_place(s, B); }

/* ; ;;... */
sc		::= SEMICOL.
sc		::= sc SEMICOL.

be_members(A)	::= list(B).				{ A = list_reverse_in_place(s, B); }

/* { ... } */
member_list(A)	::=.					{ A = list_new(s, NIL_CELL); }
member_list(A)	::= be_members(B).			{ A = list_new(s, (list_length(s, B) == 1) ? list_head(s, B) : B); }
member_list(A)	::= member_list(B) sc be_members(C).	{ A = list_cons(s, (list_length(s, C) == 1) ? list_head(s, C) : C,  B); }

/* TODO: operator precedence */
