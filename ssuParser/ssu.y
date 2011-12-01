%token_prefix TK_
%token_type {const char*}
%token_destructor {free((void *)$$);}

%extra_argument {SSUStruct *pss}

%syntax_error {
  fprintf(stderr, "Syntax error!\n"); exit(0);
}

%stack_overflow {
  fprintf(stderr, "Stack overflow!\n"); exit(0);
}

%name ssuParser


input ::= exprlist.


/* Structure defines */
exprlist ::= exprlist eoption.
exprlist ::= exprlist epackage.
exprlist ::= exprlist eenum.
exprlist ::= exprlist estruct.
exprlist ::= exprlist comment.
exprlist ::= .

/* Option keyword */
eoption ::= OPTION okeyword ASSIGN ovalue DELIM. {setOption(pss); reset(pss);}
okeyword ::= string(A). {pss->tname = A;}
ovalue ::= string(A). {pss->name = A;}

/* Package keyword */
epackage ::= PACKAGE pname DELIM. {setPackage(pss); reset(pss);}
pname ::= string(A). {pss->name = A;}

/* Enum keyword */
eenum ::= eenumstart LBRACE evallist RBRACE. {endEnum(pss);}

eenumstart ::= ENUM ename. {addEnum(pss); reset(pss);}

ename ::= string(A). {pss->name = A;}

evallist ::= evallist eval.
evallist ::= evallist comment.
evallist ::= .

eval ::= evalname ASSIGN evalval DELIM. {addEnumVal(pss); reset(pss);}
evalname ::= string(A). {pss->name = A;}
evalval ::= integer(A). {pss->order = A;}

/* Struct/message/class keyword */
estruct ::= estructstart LBRACE deflist RBRACE. {endStruct(pss);}

estructstart ::= STRUCT sname. {addStruct(pss); reset(pss);}

sname ::= string(A). {pss->order = 0; pss->name = A;}
sname ::= string(A) LSBRACKET integer(B) RSBRACKET. {pss->order = B; pss->name = A;}


/* Field defines */
deflist ::= deflist edef.
deflist ::= deflist comment.
deflist ::= .

edef ::= preoption vconstraint vtype vname vorder vdefault DELIM. {appendField(pss); reset(pss);}
edef ::= estruct.

preoption ::= LSBRACKET voption RSBRACKET.
preoption ::= .

voption ::= fieldoption.
voption ::= voption COMMA fieldoption.

fieldoption ::= REQUIRED. {pss->constraint = 1;}
fieldoption ::= OPTIONAL. {pss->constraint = 2;}
fieldoption ::= REPEATED. {pss->constraint = 3;}
fieldoption ::= ORDERMAP. {pss->constraint = 4;}
fieldoption ::= integer(I). {pss->order = I;}

vconstraint ::= REQUIRED. {pss->constraint = 1;}
vconstraint ::= OPTIONAL. {pss->constraint = 2;}
vconstraint ::= REPEATED. {pss->constraint = 3;}
vconstraint ::= ORDERMAP. {pss->constraint = 4;}
vconstraint ::= . { if(pss->constraint == 0) pss->constraint = 1; }

vtype ::= INTEGER. {pss->type = TYPE_INT; pss->tname = "";}
vtype ::= SINTEGER. {pss->type = TYPE_SINT; pss->tname = "";}
vtype ::= UINTEGER. {pss->type = TYPE_UINT; pss->tname = "";}
vtype ::= INTEGER64. {pss->type = TYPE_INT64; pss->tname = "";}
vtype ::= SINTEGER64. {pss->type = TYPE_SINT64; pss->tname = "";}
vtype ::= UINTEGER64. {pss->type = TYPE_UINT64; pss->tname = "";}
vtype ::= FLOAT. {pss->type = TYPE_FLOAT; pss->tname = "";}
vtype ::= DOUBLE. {pss->type = TYPE_DOUBLE; pss->tname = "";}
vtype ::= STRING. {pss->type = TYPE_STRING; pss->tname = "";}
vtype ::= BOOL. {pss->type = TYPE_BOOL; pss->tname = "";}
vtype ::= VECTOR. {pss->type = TYPE_VECTOR; pss->tname = "";}
vtype ::= string(A). {pss->type = TYPE_CUSTOM; pss->tname = A;}

vname ::= string(A). {pss->name = A;}

vorder ::= ASSIGN integer(A). {pss->order = A;}
vorder ::= .

vdefault ::= LSBRACKET DEFAULT ASSIGN string(A) RSBRACKET . {pss->defVal = A;}
vdefault ::= .


/* Comment */
comment ::= COMMENT(A). {if(pss->comment[0] != 0) strcat(pss->comment, "\n"); strcat(pss->comment, A);}

/* Basic types */
%type integer {int}
integer(I) ::= CUSTOM(A). {I = str_to_uint(A);}

%type string {const char *}
string(N) ::= CUSTOM(A). {N = A;}
