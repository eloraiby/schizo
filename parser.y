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
cell		::= CELL_CONS.				/* cons cell */
cell		::= CELL_LAMBDA.			/* closure */
cell		::= CELL_PROCEDURE.			/* foreign function interface */
cell		::= CELL_IF.				/* if */

/* ( ... ) */
cell(A)		::= QUOTE LPAR RPAR.			{ cell_id_t nil = { 0 }; A = cell_new_cons(s, nil); }
cell(A)		::= QUOTE cell(B).			{ A = cell_new_cons(s, B); }
cell(A)		::= LPAR cell_list(B) RPAR.		{ A = B; }

cell_list(A)	::= cell(B).				{ A = cell_new_cons(s, B); }
cell_list(A)	::= cell_list(B) cell(C).		{ A = cell_cons(s, C, B); }
