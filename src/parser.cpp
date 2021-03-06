/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#include <assert.h>
#line 28 "/home/aifu/projects/schizo/src/parser.y"

#include "parser.h"
#include <schizo/schizo.hpp>

using namespace schizo;

#define PR(V)		parser::add_token(s, V)
#define LIST		new exp::list
#define LENGTH		exp::list::length
#define HEAD		exp::list::head

exp::iptr
make_binary_op(parser* s, exp::iptr op, exp::iptr left, exp::iptr right)
{ return LIST(PR(LIST(op,
		      PR(LIST(LENGTH(left) == 1 ? HEAD(left) : left,
			      PR(LIST(LENGTH(right) == 1 ? HEAD(right) : right,
			      nullptr))))))
		      , nullptr);
}
#line 29 "/home/aifu/projects/schizo/src/parser.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/*
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands.
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    parserTOKENTYPE     is the data type used for minor tokens given
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is parserTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    parserARG_SDECL     A static variable declaration for the %extra_argument
**    parserARG_PDECL     A parameter declaration for the %extra_argument
**    parserARG_STORE     Code to store %extra_argument into yypParser
**    parserARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 37
#define YYACTIONTYPE unsigned char
#define parserTOKENTYPE  exp* 
typedef union {
  int yyinit;
  parserTOKENTYPE yy0;
  exp* yy4;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define parserARG_SDECL  parser* s ;
#define parserARG_PDECL , parser* s 
#define parserARG_FETCH  parser* s  = yypParser->s 
#define parserARG_STORE yypParser->s  = s 
#define YYNSTATE 47
#define YYNRULE 33
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
#define YY_ACTTAB_COUNT (157)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    46,   45,   44,   43,   42,   41,    3,   27,    2,   38,
 /*    10 */    29,   40,    1,    4,   31,   11,   46,   45,   44,   43,
 /*    20 */    42,   41,    3,   81,    2,   15,   36,   37,    1,   10,
 /*    30 */    31,   11,   46,   45,   44,   43,   42,   41,    3,    8,
 /*    40 */     2,   23,   36,    9,    1,   25,   36,   11,   13,   47,
 /*    50 */    12,   28,   33,   22,   21,   19,   17,   25,   36,   26,
 /*    60 */     4,    7,   12,   35,   33,   22,   21,   19,   17,   25,
 /*    70 */    36,    5,   14,   82,   12,   28,   33,   22,   21,   19,
 /*    80 */    17,   25,   36,   24,    6,   82,   12,   35,   33,   22,
 /*    90 */    21,   19,   17,   46,   45,   44,   43,   42,   41,    3,
 /*   100 */    82,    2,   25,   36,   82,    1,   82,   12,   30,   33,
 /*   110 */    22,   21,   19,   17,   25,   36,   82,   82,   82,   12,
 /*   120 */    39,   33,   22,   21,   16,   29,   25,   36,   82,   82,
 /*   130 */    82,   12,   82,   33,   22,   18,   25,   36,   82,   82,
 /*   140 */    82,   12,   82,   33,   20,   82,   25,   36,   82,   25,
 /*   150 */    36,   12,   82,   34,   12,   82,   32,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    2,    3,    4,    5,    6,    7,   12,    9,   10,
 /*    10 */    15,    8,   13,   11,   15,   16,    1,    2,    3,    4,
 /*    20 */     5,    6,    7,   22,    9,   24,   25,   14,   13,   17,
 /*    30 */    15,   16,    1,    2,    3,    4,    5,    6,    7,   19,
 /*    40 */     9,   24,   25,   18,   13,   24,   25,   16,   27,    0,
 /*    50 */    29,   30,   31,   32,   33,   34,   35,   24,   25,   26,
 /*    60 */    11,   20,   29,   30,   31,   32,   33,   34,   35,   24,
 /*    70 */    25,   28,   27,   36,   29,   30,   31,   32,   33,   34,
 /*    80 */    35,   24,   25,   26,   28,   36,   29,   30,   31,   32,
 /*    90 */    33,   34,   35,    1,    2,    3,    4,    5,    6,    7,
 /*   100 */    36,    9,   24,   25,   36,   13,   36,   29,   30,   31,
 /*   110 */    32,   33,   34,   35,   24,   25,   36,   36,   36,   29,
 /*   120 */    10,   31,   32,   33,   34,   15,   24,   25,   36,   36,
 /*   130 */    36,   29,   36,   31,   32,   33,   24,   25,   36,   36,
 /*   140 */    36,   29,   36,   31,   32,   36,   24,   25,   36,   24,
 /*   150 */    25,   29,   36,   31,   29,   36,   31,
};
#define YY_SHIFT_USE_DFLT (-6)
#define YY_SHIFT_COUNT (26)
#define YY_SHIFT_MIN   (-5)
#define YY_SHIFT_MAX   (110)
static const signed char yy_shift_ofst[] = {
 /*     0 */    92,   31,   31,   31,   31,   -1,   15,   31,   31,   31,
 /*    10 */    31,   31,   92,   -5,  110,   49,   20,   41,   25,   20,
 /*    20 */    12,   25,   12,    2,   13,    2,    3,
};
#define YY_REDUCE_USE_DFLT (-1)
#define YY_REDUCE_COUNT (14)
#define YY_REDUCE_MIN   (0)
#define YY_REDUCE_MAX   (125)
static const signed char yy_reduce_ofst[] = {
 /*     0 */     1,   57,   45,   33,   21,   78,   78,   90,  102,  112,
 /*    10 */   125,  122,   17,   56,   43,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */    80,   62,   66,   62,   66,   80,   80,   80,   80,   80,
 /*    10 */    80,   80,   69,   80,   80,   80,   78,   79,   76,   77,
 /*    20 */    74,   75,   73,   61,   80,   60,   80,   57,   67,   64,
 /*    30 */    68,   65,   72,   71,   70,   63,   59,   58,   56,   55,
 /*    40 */    54,   53,   52,   51,   50,   49,   48,
};

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
**
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
			 ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
			 ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  parserARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/*
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void parserTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = {
  "$",             "TOK_SYMBOL",    "TOK_BOOL",      "TOK_CHAR",    
  "TOK_SINT64",    "TOK_REAL64",    "TOK_STRING",    "TOK_LPAR",    
  "TOK_RPAR",      "TOK_LBR",       "TOK_RBR",       "TOK_LSQB",    
  "TOK_RSQB",      "TOK_LHASH",     "TOK_RHASH",     "TOK_SEMICOL", 
  "TOK_OP_UNARY",  "TOK_OP_BIN0",   "TOK_OP_BIN1",   "TOK_OP_BIN2", 
  "TOK_OP_BIN3",   "error",         "program",       "cell_list",   
  "sexpr",         "atom",          "se_members",    "member_list", 
  "sc",            "list",          "be_members",    "unary",       
  "binop0",        "binop1",        "binop2",        "binop3",      
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "program ::= sexpr",
 /*   1 */ "atom ::= TOK_SYMBOL",
 /*   2 */ "atom ::= TOK_BOOL",
 /*   3 */ "atom ::= TOK_CHAR",
 /*   4 */ "atom ::= TOK_SINT64",
 /*   5 */ "atom ::= TOK_REAL64",
 /*   6 */ "atom ::= TOK_STRING",
 /*   7 */ "sexpr ::= TOK_LPAR se_members TOK_RPAR",
 /*   8 */ "sexpr ::= TOK_LBR member_list TOK_RBR",
 /*   9 */ "sexpr ::= TOK_LBR member_list sc TOK_RBR",
 /*  10 */ "sexpr ::= sexpr TOK_LSQB member_list TOK_RSQB",
 /*  11 */ "sexpr ::= TOK_LHASH se_members TOK_RHASH",
 /*  12 */ "sexpr ::= atom",
 /*  13 */ "list ::= sexpr",
 /*  14 */ "list ::= list sexpr",
 /*  15 */ "se_members ::=",
 /*  16 */ "se_members ::= be_members",
 /*  17 */ "sc ::= TOK_SEMICOL",
 /*  18 */ "sc ::= sc TOK_SEMICOL",
 /*  19 */ "member_list ::=",
 /*  20 */ "member_list ::= be_members",
 /*  21 */ "member_list ::= member_list sc be_members",
 /*  22 */ "unary ::= list",
 /*  23 */ "unary ::= TOK_OP_UNARY unary",
 /*  24 */ "binop0 ::= unary",
 /*  25 */ "binop0 ::= binop0 TOK_OP_BIN0 unary",
 /*  26 */ "binop1 ::= binop0",
 /*  27 */ "binop1 ::= binop1 TOK_OP_BIN1 binop0",
 /*  28 */ "binop2 ::= binop1",
 /*  29 */ "binop2 ::= binop2 TOK_OP_BIN2 binop1",
 /*  30 */ "binop3 ::= binop2",
 /*  31 */ "binop3 ::= binop3 TOK_OP_BIN3 binop2",
 /*  32 */ "be_members ::= binop3",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
	      yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/*
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to parser and parserFree.
*/
void *parser_alloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  UNUSED YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  UNUSED parserARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/*
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from parserAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void parser_free(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int parser_stack_peak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;

  if( stateno>YY_SHIFT_COUNT
   || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
	     && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
	if( yyTraceFILE ){
	  fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
	     yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
	}
#endif
	return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
	int j = i - iLookAhead + YYWILDCARD;
	if(
#if YY_SHIFT_MIN+YYWILDCARD<0
	  j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
	  j<YY_ACTTAB_COUNT &&
#endif
	  yy_lookahead[j]==YYWILDCARD
	){
#ifndef NDEBUG
	  if( yyTraceFILE ){
	    fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
	       yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
	  }
#endif /* NDEBUG */
	  return yy_action[j];
	}
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, UNUSED YYMINORTYPE *yypMinor){
   parserARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
   parserARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 22, 1 },
  { 25, 1 },
  { 25, 1 },
  { 25, 1 },
  { 25, 1 },
  { 25, 1 },
  { 25, 1 },
  { 24, 3 },
  { 24, 3 },
  { 24, 4 },
  { 24, 4 },
  { 24, 3 },
  { 24, 1 },
  { 29, 1 },
  { 29, 2 },
  { 26, 0 },
  { 26, 1 },
  { 28, 1 },
  { 28, 2 },
  { 27, 0 },
  { 27, 1 },
  { 27, 3 },
  { 31, 1 },
  { 31, 2 },
  { 32, 1 },
  { 32, 3 },
  { 33, 1 },
  { 33, 3 },
  { 34, 1 },
  { 34, 3 },
  { 35, 1 },
  { 35, 3 },
  { 30, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  parserARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0
	&& yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0: /* program ::= sexpr */
#line 65 "/home/aifu/projects/schizo/src/parser.y"
{ s->root = PR(yymsp[0].minor.yy0); }
#line 769 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 1: /* atom ::= TOK_SYMBOL */
      case 2: /* atom ::= TOK_BOOL */ yytestcase(yyruleno==2);
      case 3: /* atom ::= TOK_CHAR */ yytestcase(yyruleno==3);
      case 4: /* atom ::= TOK_SINT64 */ yytestcase(yyruleno==4);
      case 5: /* atom ::= TOK_REAL64 */ yytestcase(yyruleno==5);
      case 6: /* atom ::= TOK_STRING */ yytestcase(yyruleno==6);
#line 68 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = yymsp[0].minor.yy0; }
#line 779 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 7: /* sexpr ::= TOK_LPAR se_members TOK_RPAR */
      case 11: /* sexpr ::= TOK_LHASH se_members TOK_RHASH */ yytestcase(yyruleno==11);
#line 76 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(yymsp[-1].minor.yy0); }
#line 785 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 8: /* sexpr ::= TOK_LBR member_list TOK_RBR */
#line 77 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(new exp::list(PR(new exp::atom_symbol("begin")), ( yymsp[-1].minor.yy0 && yymsp[-1].minor.yy0->type() == exp::EXP_LIST ) ? PR(exp::list::reverse(yymsp[-1].minor.yy0).get()) : yymsp[-1].minor.yy0)); }
#line 790 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 9: /* sexpr ::= TOK_LBR member_list sc TOK_RBR */
#line 78 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(new exp::list(PR(new exp::atom_symbol("begin")), ( yymsp[-2].minor.yy0 && yymsp[-2].minor.yy0->type() == exp::EXP_LIST ) ? PR(exp::list::reverse(yymsp[-2].minor.yy0).get()) : yymsp[-2].minor.yy0)); }
#line 795 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 10: /* sexpr ::= sexpr TOK_LSQB member_list TOK_RSQB */
#line 80 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(new exp::list(PR(new exp::atom_symbol("vector.get")),
									  PR(new exp::list(yymsp[-3].minor.yy0, ( yymsp[-1].minor.yy0 && yymsp[-1].minor.yy0->type() == exp::EXP_LIST ) ? PR(exp::list::reverse(yymsp[-1].minor.yy0).get()) : yymsp[-1].minor.yy0)))); }
#line 801 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 12: /* sexpr ::= atom */
      case 16: /* se_members ::= be_members */ yytestcase(yyruleno==16);
      case 24: /* binop0 ::= unary */ yytestcase(yyruleno==24);
      case 26: /* binop1 ::= binop0 */ yytestcase(yyruleno==26);
      case 28: /* binop2 ::= binop1 */ yytestcase(yyruleno==28);
      case 30: /* binop3 ::= binop2 */ yytestcase(yyruleno==30);
      case 32: /* be_members ::= binop3 */ yytestcase(yyruleno==32);
#line 85 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(yymsp[0].minor.yy0); }
#line 812 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 13: /* list ::= sexpr */
#line 87 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(LIST(yymsp[0].minor.yy0, nullptr)); }
#line 817 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 14: /* list ::= list sexpr */
#line 88 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(LIST(yymsp[0].minor.yy0, yymsp[-1].minor.yy0)); }
#line 822 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 15: /* se_members ::= */
      case 19: /* member_list ::= */ yytestcase(yyruleno==19);
#line 90 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = nullptr; }
#line 828 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 20: /* member_list ::= be_members */
#line 99 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(new exp::list((exp::list::length(yymsp[0].minor.yy0) == 1) ? exp::list::head(yymsp[0].minor.yy0) : yymsp[0].minor.yy0, nullptr)); }
#line 833 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 21: /* member_list ::= member_list sc be_members */
#line 100 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(new exp::list((exp::list::length(yymsp[0].minor.yy0) == 1) ? exp::list::head(yymsp[0].minor.yy0) : yymsp[0].minor.yy0, yymsp[-2].minor.yy0)); }
#line 838 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 22: /* unary ::= list */
#line 103 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(exp::list::reverse(yymsp[0].minor.yy0).get()); }
#line 843 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 23: /* unary ::= TOK_OP_UNARY unary */
#line 104 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(LIST(yymsp[-1].minor.yy0, PR(LIST(LENGTH(yymsp[0].minor.yy0) == 1 ? PR(LIST(HEAD(yymsp[0].minor.yy0), nullptr)) : yymsp[0].minor.yy0, nullptr)))); }
#line 848 "/home/aifu/projects/schizo/src/parser.c"
        break;
      case 25: /* binop0 ::= binop0 TOK_OP_BIN0 unary */
      case 27: /* binop1 ::= binop1 TOK_OP_BIN1 binop0 */ yytestcase(yyruleno==27);
      case 29: /* binop2 ::= binop2 TOK_OP_BIN2 binop1 */ yytestcase(yyruleno==29);
      case 31: /* binop3 ::= binop3 TOK_OP_BIN3 binop2 */ yytestcase(yyruleno==31);
#line 107 "/home/aifu/projects/schizo/src/parser.y"
{ yygotominor.yy0 = PR(make_binary_op(s, yymsp[-1].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0).get()); }
#line 856 "/home/aifu/projects/schizo/src/parser.c"
        break;
      default:
      /* (17) sc ::= TOK_SEMICOL */ yytestcase(yyruleno==17);
      /* (18) sc ::= sc TOK_SEMICOL */ yytestcase(yyruleno==18);
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  parserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  parserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  UNUSED int yymajor,                   /* The major type of the error token */
  UNUSED YYMINORTYPE yyminor            /* The minor type of the error token */
){
  parserARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 51 "/home/aifu/projects/schizo/src/parser.y"

	int i = 0;
	int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
	for ( i = 0; i < n; ++i) {
		int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
		if (a < YYNSTATE + YYNRULE) {
			printf("possible token: %s\n", yyTokenName[i]);
		}
	}
#line 930 "/home/aifu/projects/schizo/src/parser.c"
  parserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  parserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  parserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "parserAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void parser_advance(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  parserTOKENTYPE yyminor       /* The value for the token */
  parserARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  parserARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
	fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
	yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
	if( yyTraceFILE ){
	  fprintf(yyTraceFILE,"%sDiscard input token %s\n",
	     yyTracePrompt,yyTokenName[yymajor]);
	}
#endif
	yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
	yymajor = YYNOCODE;
      }else{
	 while(
	  yypParser->yyidx >= 0 &&
	  yymx != YYERRORSYMBOL &&
	  (yyact = yy_find_reduce_action(
			yypParser->yystack[yypParser->yyidx].stateno,
			YYERRORSYMBOL)) >= YYNSTATE
	){
	  yy_pop_parser_stack(yypParser);
	}
	if( yypParser->yyidx < 0 || yymajor==0 ){
	  yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
	  yy_parse_failed(yypParser);
	  yymajor = YYNOCODE;
	}else if( yymx!=YYERRORSYMBOL ){
	  YYMINORTYPE u2;
	  u2.YYERRSYMDT = 0;
	  yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
	}
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor,yyminorunion);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;

#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
	yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
	yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
