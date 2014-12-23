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
program		::= atom(B).				{ set_cell(s->parser.root, B); }
program		::= sexpr(B).				{ set_cell(s->parser.root, B); }

/* literals */
atom(A)		::= ATOM_SYMBOL(B).			{ A = B; }
atom(A)		::= ATOM_BOOL(B).			{ A = B; }
atom(A)		::= ATOM_CHAR(B).			{ A = B; }
atom(A)		::= ATOM_SINT64(B).			{ A = B; }
atom(A)		::= ATOM_REAL64(B).			{ A = B; }
atom(A)		::= ATOM_STRING(B).			{ A = B; }

/* NEVER USED in the parser: with these, lemon will also generates the ids automatically, so the enums are continious */
sexpr		::= ATOM_ERROR.				/* error */
sexpr		::= CELL_PAIR.				/* list */
sexpr		::= CELL_VECTOR.			/* a vector of cells */
sexpr		::= CELL_LAMBDA.			/* lambda */
sexpr		::= CELL_QUOTE.				/* quote (this should could have been replaced with objects, but will increase the complexity of the evaluator) */

sexpr		::= OP_APPLY_CLOSURE.			/* closure */
sexpr		::= OP_APPLY_FFI.			/* foreign function interface */
sexpr		::= OP_BIND.				/* bind symbols */
sexpr		::= OP_EVAL_EXPR.			/* eval expression */
sexpr		::= OP_EVAL_ARGS.			/* eval operator arguments */

/* ( ... ) */
sexpr(A)	::= LPAR se_members(B) RPAR.		{ A = B; }
sexpr(A)	::= LBR member_list(B) RBR.		{ A = list_cons(s, atom_new_symbol(s, "begin"), ( cell_type(s, B) == CELL_PAIR ) ? list_reverse(s, B) : B); }
sexpr(A)	::= LBR member_list(B) sc RBR.		{ A = list_cons(s, atom_new_symbol(s, "begin"), ( cell_type(s, B) == CELL_PAIR ) ? list_reverse(s, B) : B); }
sexpr(A)	::= ilist(B) LSQB member_list(C) RSQB.	{ A = list_cons(s, atom_new_symbol(s, "vector.get"),
									   list_cons(s, B, ( cell_type(s, C) == CELL_PAIR ) ? list_reverse(s, C) : C)); }
ilist(A)	::= atom(B).				{ A = B; }
ilist(A)	::= sexpr(B).				{ A = B; }

list(A)		::= ilist(B).				{ A = list_cons(s, B, NIL_CELL); }
list(A)		::= list(B) ilist(C).			{ A = list_cons(s, C, B); }

se_members(A)	::=.					{ A = NIL_CELL; }
se_members(A)	::= list(B).				{ A = list_reverse(s, B); }

/* ; ;;... */
sc		::= SEMICOL.
sc		::= sc SEMICOL.

be_members(A)	::= list(B).				{ A = list_reverse(s, B); }

/* { ... } */
member_list(A)	::=.					{ A = NIL_CELL; }
member_list(A)	::= be_members(B).			{ A = list_cons(s, (list_length(s, B) == 1) ? list_head(s, B) : B, NIL_CELL); }
member_list(A)	::= member_list(B) sc be_members(C).	{ A = list_cons(s, (list_length(s, C) == 1) ? list_head(s, C) : C,  B); }

/* TODO: operator precedence */
