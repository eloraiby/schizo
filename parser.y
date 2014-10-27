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

%token_type	{ cell_t* }
%extra_argument { state_t* current_state }

%include {
#include <stdio.h>
#include "ast.h"
}

%type exp	{ cell_t* }
%type program	{ cell_t* }
%type exp_list	{ cell_t* }

%start_symbol program

/* a program is a statement list */
program		::= exp(B).			{ current_state->root = B; }

/* literals */
exp(A)		::= CELL_SYMBOL(B).		{ A = B; }
exp(A)		::= CELL_BOOL(B).		{ A = B; }
exp(A)		::= CELL_CHAR(B).		{ A = B; }
exp(A)		::= CELL_SINT8(B).		{ A = B; }
exp(A)		::= CELL_SINT16(B).		{ A = B; }
exp(A)		::= CELL_SINT32(B).		{ A = B; }
exp(A)		::= CELL_SINT64(B).		{ A = B; }
exp(A)		::= CELL_UINT8(B).		{ A = B; }
exp(A)		::= CELL_UINT16(B).		{ A = B; }
exp(A)		::= CELL_UINT32(B).		{ A = B; }
exp(A)		::= CELL_UINT64(B).		{ A = B; }
exp(A)		::= CELL_REAL32(B).		{ A = B; }
exp(A)		::= CELL_REAL64(B).		{ A = B; }
exp(A)		::= CELL_STRING(B).		{ A = B; }

/* NEVER USED: these are a hack to have the token ids */
exp		::= CELL_CONS.
exp		::= CELL_CLOSURE.

/* ( ... ) */
exp(A)		::= LPAR RPAR.			{ A = cell_new_cons(NULL); }
exp(A)		::= LPAR exp_list(B) RPAR.	{ A = B; }

exp_list(A)	::= exp(B).			{ A = cell_new_cons(B); }
exp_list(A)	::= exp_list(B) exp(C).		{ A = cell_cons(cell_new_cons(C), B); }

