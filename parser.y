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

