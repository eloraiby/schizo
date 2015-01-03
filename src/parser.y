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

%token_type	{ exp* }
%extra_argument { state* s }

%include {
#include "parser.h"
#include <schizo/schizo.hpp>

using namespace schizo;

#define PR(V)	state::add_token(s, V)

}

%type program	{ exp* }
%type cell_list	{ exp* }

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

/* a program is a exp */
program		::= atom(B).				{ s->set_root(PR(B)); }
program		::= sexpr(B).				{ s->set_root(PR(B)); }

/* literals */
atom(A)		::= TOK_SYMBOL(B).			{ A = B; }
atom(A)		::= TOK_BOOL(B).			{ A = B; }
atom(A)		::= TOK_CHAR(B).			{ A = B; }
atom(A)		::= TOK_SINT64(B).			{ A = B; }
atom(A)		::= TOK_REAL64(B).			{ A = B; }
atom(A)		::= TOK_STRING(B).			{ A = B; }

/* ( ... ) */
sexpr(A)	::= TOK_LPAR se_members(B) TOK_RPAR.	{ A = PR(B); }
sexpr(A)	::= TOK_LBR member_list(B) TOK_RBR.	{ A = PR(new exp::list(PR(new exp::symbol("begin")), ( B && B->type() == exp::EXP_LIST ) ? PR(exp::list::reverse(B).get()) : B)); }
sexpr(A)	::= TOK_LBR member_list(B) sc TOK_RBR.	{ A = PR(new exp::list(PR(new exp::symbol("begin")), ( B && B->type() == exp::EXP_LIST ) ? PR(exp::list::reverse(B).get()) : B)); }
sexpr(A)	::= ilist(B) TOK_LSQB member_list(C) TOK_RSQB.	{ A = PR(new exp::list(PR(new exp::symbol("vector.get")),
									  PR(new exp::list(B, ( C && C->type() == exp::EXP_LIST ) ? PR(exp::list::reverse(C).get()) : C)))); }
ilist(A)	::= atom(B).				{ A = PR(B); }
ilist(A)	::= sexpr(B).				{ A = PR(B); }

list(A)		::= ilist(B).				{ A = PR(new exp::list(B, nullptr)); }
list(A)		::= list(B) ilist(C).			{ A = PR(new exp::list(C, B)); }

se_members(A)	::=.					{ A = nullptr; }
se_members(A)	::= list(B).				{ A = PR(exp::list::reverse(B).get()); }

/* ; ;;... */
sc		::= TOK_SEMICOL.
sc		::= sc TOK_SEMICOL.

be_members(A)	::= list(B).				{ A = PR(exp::list::reverse(B).get()); }

/* { ... } */
member_list(A)	::=.					{ A = nullptr; }
member_list(A)	::= be_members(B).			{ A = PR(new exp::list((exp::list::length(B) == 1) ? exp::list::head(B) : B, nullptr)); }
member_list(A)	::= member_list(B) sc be_members(C).	{ A = PR(new exp::list((exp::list::length(C) == 1) ? exp::list::head(C) : C, B)); }

/* TODO: operator precedence */
