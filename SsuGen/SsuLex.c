/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
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
**    ssuParserTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ssuParserTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ssuParserARG_SDECL     A static variable declaration for the %extra_argument
**    ssuParserARG_PDECL     A parameter declaration for the %extra_argument
**    ssuParserARG_STORE     Code to store %extra_argument into yypParser
**    ssuParserARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 62
#define YYACTIONTYPE unsigned char
#define ssuParserTOKENTYPE const char*
typedef union {
  int yyinit;
  ssuParserTOKENTYPE yy0;
  const char * yy26;
  int yy40;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ssuParserARG_SDECL SSUStruct *pss;
#define ssuParserARG_PDECL ,SSUStruct *pss
#define ssuParserARG_FETCH SSUStruct *pss = yypParser->pss
#define ssuParserARG_STORE yypParser->pss = pss
#define YYNSTATE 95
#define YYNRULE 65
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
#define YY_ACTTAB_COUNT (106)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    63,   62,   61,   60,   59,   58,   57,   56,   55,   54,
 /*    10 */    47,   48,   69,   87,   95,   13,   89,   35,   11,   28,
 /*    20 */    26,    9,    8,   12,   32,   46,    6,   70,   14,   94,
 /*    30 */    93,   92,   91,   90,   44,   43,   42,   41,   35,   38,
 /*    40 */    85,   86,   76,   10,   83,   32,   52,   51,   50,   49,
 /*    50 */    40,   78,   73,    9,    8,    4,   81,   37,   88,   77,
 /*    60 */    22,   23,   39,   80,   82,   34,   76,   87,   84,   36,
 /*    70 */    33,   79,   74,   31,   76,   64,   75,   40,   21,   53,
 /*    80 */   161,    1,   71,   20,   72,   68,   17,   67,   16,   45,
 /*    90 */    66,   19,    2,    3,    7,   30,   78,   18,   15,   24,
 /*   100 */    87,   29,   27,   25,   65,    5,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */    16,   17,   18,   19,   20,   21,   22,   23,   24,   25,
 /*    10 */    35,   36,   37,   29,    0,    1,    3,   42,    4,   26,
 /*    20 */    27,    7,    8,    2,   49,   10,   11,   52,   53,   33,
 /*    30 */    34,   35,   36,   37,   12,   13,   14,   15,   42,   38,
 /*    40 */     3,   40,   28,    2,    6,   49,   12,   13,   14,   15,
 /*    50 */    48,   29,    6,    7,    8,    9,   37,   39,   40,   40,
 /*    60 */     5,   59,   60,    3,   45,   46,   28,   29,   40,   41,
 /*    70 */    47,   48,   40,   40,   28,   40,   44,   48,    5,   40,
 /*    80 */    31,   32,   10,    9,   51,    3,    2,   10,    2,   60,
 /*    90 */    10,   56,   50,   43,   55,   48,   29,   57,    2,    9,
 /*   100 */    29,   58,   40,   40,   48,   54,
};
#define YY_SHIFT_USE_DFLT (-17)
#define YY_SHIFT_COUNT (38)
#define YY_SHIFT_MIN   (-16)
#define YY_SHIFT_MAX   (96)
static const signed char yy_shift_ofst[] = {
 /*     0 */   -17,   14,   46,   38,   22,  -16,   22,   71,   71,   71,
 /*    10 */    67,   71,   71,   71,   34,   67,   71,   71,   90,   96,
 /*    20 */    67,  -17,  -17,   15,   -7,   80,   86,   77,   84,   82,
 /*    30 */    72,   74,   73,   60,   41,   55,   37,   13,   21,
};
#define YY_REDUCE_USE_DFLT (-26)
#define YY_REDUCE_COUNT (22)
#define YY_REDUCE_MIN   (-25)
#define YY_REDUCE_MAX   (63)
static const signed char yy_reduce_ofst[] = {
 /*     0 */    49,   -4,  -25,   19,    2,   39,   29,   35,   33,   32,
 /*    10 */    23,   28,   18,    1,   51,   56,   63,   62,   43,   40,
 /*    20 */    47,   42,   50,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   101,  160,  127,  160,  160,  160,  160,  160,  160,  160,
 /*    10 */   160,  160,  160,  160,  139,  160,  160,  160,  156,  153,
 /*    20 */   160,  122,  112,  160,  160,  160,  160,  160,  160,  160,
 /*    30 */   160,  118,  160,  160,  160,  160,  160,  160,  160,  128,
 /*    40 */   134,  133,  132,  131,  130,  129,  126,  125,  124,  138,
 /*    50 */   137,  136,  135,  150,  149,  148,  147,  146,  145,  144,
 /*    60 */   143,  142,  141,  140,  151,  152,  155,  154,  123,  121,
 /*    70 */   120,  119,  117,  116,  109,  108,  157,  114,  158,  115,
 /*    80 */   113,  111,  110,  107,  106,  105,  103,  159,  104,  102,
 /*    90 */   100,   99,   98,   97,   96,
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
  ssuParserARG_SDECL                /* A place to hold %extra_argument */
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
void ssuParserTrace(FILE *TraceFILE, char *zTracePrompt){
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
  "$",             "OPTION",        "ASSIGN",        "DELIM",       
  "PACKAGE",       "LBRACE",        "RBRACE",        "ENUM",        
  "STRUCT",        "LSBRACKET",     "RSBRACKET",     "COMMA",       
  "REQUIRED",      "OPTIONAL",      "REPEATED",      "ORDERMAP",    
  "INTEGER",       "SINTEGER",      "UINTEGER",      "INTEGER64",   
  "SINTEGER64",    "UINTEGER64",    "FLOAT",         "DOUBLE",      
  "STRING",        "BOOL",          "DEFAULT",       "PACKED",      
  "COMMENT",       "CUSTOM",        "error",         "input",       
  "exprlist",      "eoption",       "epackage",      "eenum",       
  "estruct",       "comment",       "okeyword",      "ovalue",      
  "string",        "pname",         "eenumstart",    "evallist",    
  "ename",         "eval",          "evalname",      "evalval",     
  "integer",       "estructstart",  "deflist",       "sname",       
  "edef",          "preoption",     "vconstraint",   "vtype",       
  "vname",         "vorder",        "vdefault",      "voption",     
  "fieldoption", 
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "input ::= exprlist",
 /*   1 */ "exprlist ::= exprlist eoption",
 /*   2 */ "exprlist ::= exprlist epackage",
 /*   3 */ "exprlist ::= exprlist eenum",
 /*   4 */ "exprlist ::= exprlist estruct",
 /*   5 */ "exprlist ::= exprlist comment",
 /*   6 */ "exprlist ::=",
 /*   7 */ "eoption ::= OPTION okeyword ASSIGN ovalue DELIM",
 /*   8 */ "okeyword ::= string",
 /*   9 */ "ovalue ::= string",
 /*  10 */ "epackage ::= PACKAGE pname DELIM",
 /*  11 */ "pname ::= string",
 /*  12 */ "eenum ::= eenumstart LBRACE evallist RBRACE",
 /*  13 */ "eenumstart ::= ENUM ename",
 /*  14 */ "ename ::= string",
 /*  15 */ "evallist ::= evallist eval",
 /*  16 */ "evallist ::= evallist comment",
 /*  17 */ "evallist ::=",
 /*  18 */ "eval ::= evalname ASSIGN evalval DELIM",
 /*  19 */ "evalname ::= string",
 /*  20 */ "evalval ::= integer",
 /*  21 */ "estruct ::= estructstart LBRACE deflist RBRACE",
 /*  22 */ "estructstart ::= STRUCT sname",
 /*  23 */ "sname ::= string",
 /*  24 */ "sname ::= string LSBRACKET integer RSBRACKET",
 /*  25 */ "deflist ::= deflist edef",
 /*  26 */ "deflist ::= deflist comment",
 /*  27 */ "deflist ::=",
 /*  28 */ "edef ::= preoption vconstraint vtype vname vorder vdefault DELIM",
 /*  29 */ "edef ::= estruct",
 /*  30 */ "edef ::= eenum",
 /*  31 */ "preoption ::= LSBRACKET voption RSBRACKET",
 /*  32 */ "preoption ::=",
 /*  33 */ "voption ::= fieldoption",
 /*  34 */ "voption ::= voption COMMA fieldoption",
 /*  35 */ "fieldoption ::= REQUIRED",
 /*  36 */ "fieldoption ::= OPTIONAL",
 /*  37 */ "fieldoption ::= REPEATED",
 /*  38 */ "fieldoption ::= ORDERMAP",
 /*  39 */ "fieldoption ::= integer",
 /*  40 */ "vconstraint ::= REQUIRED",
 /*  41 */ "vconstraint ::= OPTIONAL",
 /*  42 */ "vconstraint ::= REPEATED",
 /*  43 */ "vconstraint ::= ORDERMAP",
 /*  44 */ "vconstraint ::=",
 /*  45 */ "vtype ::= INTEGER",
 /*  46 */ "vtype ::= SINTEGER",
 /*  47 */ "vtype ::= UINTEGER",
 /*  48 */ "vtype ::= INTEGER64",
 /*  49 */ "vtype ::= SINTEGER64",
 /*  50 */ "vtype ::= UINTEGER64",
 /*  51 */ "vtype ::= FLOAT",
 /*  52 */ "vtype ::= DOUBLE",
 /*  53 */ "vtype ::= STRING",
 /*  54 */ "vtype ::= BOOL",
 /*  55 */ "vtype ::= string",
 /*  56 */ "vname ::= string",
 /*  57 */ "vorder ::= ASSIGN integer",
 /*  58 */ "vorder ::=",
 /*  59 */ "vdefault ::= LSBRACKET DEFAULT ASSIGN string RSBRACKET",
 /*  60 */ "vdefault ::= LSBRACKET PACKED ASSIGN string RSBRACKET",
 /*  61 */ "vdefault ::=",
 /*  62 */ "comment ::= COMMENT",
 /*  63 */ "integer ::= CUSTOM",
 /*  64 */ "string ::= CUSTOM",
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
** to ssuParser and ssuParserFree.
*/
void *ssuParserAlloc(void *(*mallocProc)(size_t)){
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
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ssuParserARG_FETCH;
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
      /* TERMINAL Destructor */
    case 1: /* OPTION */
    case 2: /* ASSIGN */
    case 3: /* DELIM */
    case 4: /* PACKAGE */
    case 5: /* LBRACE */
    case 6: /* RBRACE */
    case 7: /* ENUM */
    case 8: /* STRUCT */
    case 9: /* LSBRACKET */
    case 10: /* RSBRACKET */
    case 11: /* COMMA */
    case 12: /* REQUIRED */
    case 13: /* OPTIONAL */
    case 14: /* REPEATED */
    case 15: /* ORDERMAP */
    case 16: /* INTEGER */
    case 17: /* SINTEGER */
    case 18: /* UINTEGER */
    case 19: /* INTEGER64 */
    case 20: /* SINTEGER64 */
    case 21: /* UINTEGER64 */
    case 22: /* FLOAT */
    case 23: /* DOUBLE */
    case 24: /* STRING */
    case 25: /* BOOL */
    case 26: /* DEFAULT */
    case 27: /* PACKED */
    case 28: /* COMMENT */
    case 29: /* CUSTOM */
{
#line 31 "D:/Projects/ssu/SsuGen/SsuLex.y"
free((void *)(yypminor->yy0));
#line 492 "D:/Projects/ssu/SsuGen/SsuLex.c"
}
      break;
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
**       obtained from ssuParserAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ssuParserFree(
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
int ssuParserStackPeak(void *p){
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
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ssuParserARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
#line 39 "D:/Projects/ssu/SsuGen/SsuLex.y"

  fprintf(stderr, "Stack overflow!\n"); exit(0);
#line 677 "D:/Projects/ssu/SsuGen/SsuLex.c"
   ssuParserARG_STORE; /* Suppress warning about unused %extra_argument var */
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
  { 31, 1 },
  { 32, 2 },
  { 32, 2 },
  { 32, 2 },
  { 32, 2 },
  { 32, 2 },
  { 32, 0 },
  { 33, 5 },
  { 38, 1 },
  { 39, 1 },
  { 34, 3 },
  { 41, 1 },
  { 35, 4 },
  { 42, 2 },
  { 44, 1 },
  { 43, 2 },
  { 43, 2 },
  { 43, 0 },
  { 45, 4 },
  { 46, 1 },
  { 47, 1 },
  { 36, 4 },
  { 49, 2 },
  { 51, 1 },
  { 51, 4 },
  { 50, 2 },
  { 50, 2 },
  { 50, 0 },
  { 52, 7 },
  { 52, 1 },
  { 52, 1 },
  { 53, 3 },
  { 53, 0 },
  { 59, 1 },
  { 59, 3 },
  { 60, 1 },
  { 60, 1 },
  { 60, 1 },
  { 60, 1 },
  { 60, 1 },
  { 54, 1 },
  { 54, 1 },
  { 54, 1 },
  { 54, 1 },
  { 54, 0 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 55, 1 },
  { 56, 1 },
  { 57, 2 },
  { 57, 0 },
  { 58, 5 },
  { 58, 5 },
  { 58, 0 },
  { 37, 1 },
  { 48, 1 },
  { 40, 1 },
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
  ssuParserARG_FETCH;
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
      case 7: /* eoption ::= OPTION okeyword ASSIGN ovalue DELIM */
#line 58 "D:/Projects/ssu/SsuGen/SsuLex.y"
{setOption(pss); reset(pss);  yy_destructor(yypParser,1,&yymsp[-4].minor);
  yy_destructor(yypParser,2,&yymsp[-2].minor);
  yy_destructor(yypParser,3,&yymsp[0].minor);
}
#line 859 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 8: /* okeyword ::= string */
#line 59 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->tname = yymsp[0].minor.yy26;}
#line 864 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 9: /* ovalue ::= string */
      case 11: /* pname ::= string */ yytestcase(yyruleno==11);
      case 14: /* ename ::= string */ yytestcase(yyruleno==14);
      case 19: /* evalname ::= string */ yytestcase(yyruleno==19);
      case 56: /* vname ::= string */ yytestcase(yyruleno==56);
#line 60 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->name = yymsp[0].minor.yy26;}
#line 873 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 10: /* epackage ::= PACKAGE pname DELIM */
#line 63 "D:/Projects/ssu/SsuGen/SsuLex.y"
{setPackage(pss); reset(pss);  yy_destructor(yypParser,4,&yymsp[-2].minor);
  yy_destructor(yypParser,3,&yymsp[0].minor);
}
#line 880 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 12: /* eenum ::= eenumstart LBRACE evallist RBRACE */
#line 67 "D:/Projects/ssu/SsuGen/SsuLex.y"
{endEnum(pss);  yy_destructor(yypParser,5,&yymsp[-2].minor);
  yy_destructor(yypParser,6,&yymsp[0].minor);
}
#line 887 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 13: /* eenumstart ::= ENUM ename */
#line 69 "D:/Projects/ssu/SsuGen/SsuLex.y"
{addEnum(pss); reset(pss);  yy_destructor(yypParser,7,&yymsp[-1].minor);
}
#line 893 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 18: /* eval ::= evalname ASSIGN evalval DELIM */
#line 77 "D:/Projects/ssu/SsuGen/SsuLex.y"
{addEnumVal(pss); reset(pss);  yy_destructor(yypParser,2,&yymsp[-2].minor);
  yy_destructor(yypParser,3,&yymsp[0].minor);
}
#line 900 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 20: /* evalval ::= integer */
      case 39: /* fieldoption ::= integer */ yytestcase(yyruleno==39);
#line 79 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->order = yymsp[0].minor.yy40;}
#line 906 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 21: /* estruct ::= estructstart LBRACE deflist RBRACE */
#line 82 "D:/Projects/ssu/SsuGen/SsuLex.y"
{endStruct(pss);  yy_destructor(yypParser,5,&yymsp[-2].minor);
  yy_destructor(yypParser,6,&yymsp[0].minor);
}
#line 913 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 22: /* estructstart ::= STRUCT sname */
#line 84 "D:/Projects/ssu/SsuGen/SsuLex.y"
{addStruct(pss); reset(pss);  yy_destructor(yypParser,8,&yymsp[-1].minor);
}
#line 919 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 23: /* sname ::= string */
#line 86 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->order = 0; pss->name = yymsp[0].minor.yy26;}
#line 924 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 24: /* sname ::= string LSBRACKET integer RSBRACKET */
#line 87 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->order = yymsp[-1].minor.yy40; pss->name = yymsp[-3].minor.yy26;  yy_destructor(yypParser,9,&yymsp[-2].minor);
  yy_destructor(yypParser,10,&yymsp[0].minor);
}
#line 931 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 28: /* edef ::= preoption vconstraint vtype vname vorder vdefault DELIM */
#line 95 "D:/Projects/ssu/SsuGen/SsuLex.y"
{appendField(pss); reset(pss);  yy_destructor(yypParser,3,&yymsp[0].minor);
}
#line 937 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 31: /* preoption ::= LSBRACKET voption RSBRACKET */
#line 99 "D:/Projects/ssu/SsuGen/SsuLex.y"
{
  yy_destructor(yypParser,9,&yymsp[-2].minor);
  yy_destructor(yypParser,10,&yymsp[0].minor);
}
#line 945 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 34: /* voption ::= voption COMMA fieldoption */
#line 103 "D:/Projects/ssu/SsuGen/SsuLex.y"
{
  yy_destructor(yypParser,11,&yymsp[-1].minor);
}
#line 952 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 35: /* fieldoption ::= REQUIRED */
      case 40: /* vconstraint ::= REQUIRED */ yytestcase(yyruleno==40);
#line 105 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->constraint = 1;  yy_destructor(yypParser,12,&yymsp[0].minor);
}
#line 959 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 36: /* fieldoption ::= OPTIONAL */
      case 41: /* vconstraint ::= OPTIONAL */ yytestcase(yyruleno==41);
#line 106 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->constraint = 2;  yy_destructor(yypParser,13,&yymsp[0].minor);
}
#line 966 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 37: /* fieldoption ::= REPEATED */
      case 42: /* vconstraint ::= REPEATED */ yytestcase(yyruleno==42);
#line 107 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->constraint = 3;  yy_destructor(yypParser,14,&yymsp[0].minor);
}
#line 973 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 38: /* fieldoption ::= ORDERMAP */
      case 43: /* vconstraint ::= ORDERMAP */ yytestcase(yyruleno==43);
#line 108 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->constraint = 4;  yy_destructor(yypParser,15,&yymsp[0].minor);
}
#line 980 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 44: /* vconstraint ::= */
#line 115 "D:/Projects/ssu/SsuGen/SsuLex.y"
{ if(pss->constraint == 0) pss->constraint = 1; }
#line 985 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 45: /* vtype ::= INTEGER */
#line 117 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_INT; pss->tname = "";  yy_destructor(yypParser,16,&yymsp[0].minor);
}
#line 991 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 46: /* vtype ::= SINTEGER */
#line 118 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_SINT; pss->tname = "";  yy_destructor(yypParser,17,&yymsp[0].minor);
}
#line 997 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 47: /* vtype ::= UINTEGER */
#line 119 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_UINT; pss->tname = "";  yy_destructor(yypParser,18,&yymsp[0].minor);
}
#line 1003 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 48: /* vtype ::= INTEGER64 */
#line 120 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_INT64; pss->tname = "";  yy_destructor(yypParser,19,&yymsp[0].minor);
}
#line 1009 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 49: /* vtype ::= SINTEGER64 */
#line 121 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_SINT64; pss->tname = "";  yy_destructor(yypParser,20,&yymsp[0].minor);
}
#line 1015 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 50: /* vtype ::= UINTEGER64 */
#line 122 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_UINT64; pss->tname = "";  yy_destructor(yypParser,21,&yymsp[0].minor);
}
#line 1021 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 51: /* vtype ::= FLOAT */
#line 123 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_FLOAT; pss->tname = "";  yy_destructor(yypParser,22,&yymsp[0].minor);
}
#line 1027 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 52: /* vtype ::= DOUBLE */
#line 124 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_DOUBLE; pss->tname = "";  yy_destructor(yypParser,23,&yymsp[0].minor);
}
#line 1033 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 53: /* vtype ::= STRING */
#line 125 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_STRING; pss->tname = "";  yy_destructor(yypParser,24,&yymsp[0].minor);
}
#line 1039 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 54: /* vtype ::= BOOL */
#line 126 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_BOOL; pss->tname = "";  yy_destructor(yypParser,25,&yymsp[0].minor);
}
#line 1045 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 55: /* vtype ::= string */
#line 127 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->type = TYPE_STRUCT; pss->tname = yymsp[0].minor.yy26;}
#line 1050 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 57: /* vorder ::= ASSIGN integer */
#line 131 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->order = yymsp[0].minor.yy40;  yy_destructor(yypParser,2,&yymsp[-1].minor);
}
#line 1056 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 59: /* vdefault ::= LSBRACKET DEFAULT ASSIGN string RSBRACKET */
#line 134 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->defVal = yymsp[-1].minor.yy26;  yy_destructor(yypParser,9,&yymsp[-4].minor);
  yy_destructor(yypParser,26,&yymsp[-3].minor);
  yy_destructor(yypParser,2,&yymsp[-2].minor);
  yy_destructor(yypParser,10,&yymsp[0].minor);
}
#line 1065 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 60: /* vdefault ::= LSBRACKET PACKED ASSIGN string RSBRACKET */
#line 135 "D:/Projects/ssu/SsuGen/SsuLex.y"
{pss->defVal = yymsp[-1].minor.yy26;  yy_destructor(yypParser,9,&yymsp[-4].minor);
  yy_destructor(yypParser,27,&yymsp[-3].minor);
  yy_destructor(yypParser,2,&yymsp[-2].minor);
  yy_destructor(yypParser,10,&yymsp[0].minor);
}
#line 1074 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 62: /* comment ::= COMMENT */
#line 140 "D:/Projects/ssu/SsuGen/SsuLex.y"
{if(pss->comment[0] != 0) strcat(pss->comment, "\n"); strcat(pss->comment, yymsp[0].minor.yy0);}
#line 1079 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 63: /* integer ::= CUSTOM */
#line 144 "D:/Projects/ssu/SsuGen/SsuLex.y"
{yygotominor.yy40 = str_to_uint(yymsp[0].minor.yy0);}
#line 1084 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      case 64: /* string ::= CUSTOM */
#line 147 "D:/Projects/ssu/SsuGen/SsuLex.y"
{yygotominor.yy26 = yymsp[0].minor.yy0;}
#line 1089 "D:/Projects/ssu/SsuGen/SsuLex.c"
        break;
      default:
      /* (0) input ::= exprlist */ yytestcase(yyruleno==0);
      /* (1) exprlist ::= exprlist eoption */ yytestcase(yyruleno==1);
      /* (2) exprlist ::= exprlist epackage */ yytestcase(yyruleno==2);
      /* (3) exprlist ::= exprlist eenum */ yytestcase(yyruleno==3);
      /* (4) exprlist ::= exprlist estruct */ yytestcase(yyruleno==4);
      /* (5) exprlist ::= exprlist comment */ yytestcase(yyruleno==5);
      /* (6) exprlist ::= */ yytestcase(yyruleno==6);
      /* (15) evallist ::= evallist eval */ yytestcase(yyruleno==15);
      /* (16) evallist ::= evallist comment */ yytestcase(yyruleno==16);
      /* (17) evallist ::= */ yytestcase(yyruleno==17);
      /* (25) deflist ::= deflist edef */ yytestcase(yyruleno==25);
      /* (26) deflist ::= deflist comment */ yytestcase(yyruleno==26);
      /* (27) deflist ::= */ yytestcase(yyruleno==27);
      /* (29) edef ::= estruct */ yytestcase(yyruleno==29);
      /* (30) edef ::= eenum */ yytestcase(yyruleno==30);
      /* (32) preoption ::= */ yytestcase(yyruleno==32);
      /* (33) voption ::= fieldoption */ yytestcase(yyruleno==33);
      /* (58) vorder ::= */ yytestcase(yyruleno==58);
      /* (61) vdefault ::= */ yytestcase(yyruleno==61);
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
  ssuParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
  ssuParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ssuParserARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 35 "D:/Projects/ssu/SsuGen/SsuLex.y"

  fprintf(stderr, "Syntax error!\n"); exit(0);
#line 1173 "D:/Projects/ssu/SsuGen/SsuLex.c"
  ssuParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ssuParserARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
  ssuParserARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ssuParserAlloc" which describes the current state of the parser.
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
void ssuParser(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ssuParserTOKENTYPE yyminor       /* The value for the token */
  ssuParserARG_PDECL               /* Optional %extra_argument parameter */
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
  ssuParserARG_STORE;

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
