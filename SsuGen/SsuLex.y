/*
 * Copyright (c) 2011, Soar Qin<soarchin@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

%token_prefix TK_
%token_type {const char*}
%token_destructor {free((void *)$$);}

%extra_argument {SSUStruct *pss}

%syntax_error {
  onSyntaxError(pss);
}

%stack_overflow {
  onStackOverflow(pss);
}

%name ssuParser


input ::= exprlist.


/* Structure defines */
exprlist ::= exprlist eoption.
exprlist ::= exprlist epackage.
exprlist ::= exprlist eimport.
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

/* Import keyword */
eimport ::= IMPORT iname DELIM.
iname ::= string(A). { importFile(pss, A); }

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
edef ::= eenum.

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
vtype ::= string(A). {pss->type = TYPE_STRUCT; pss->tname = A;}

vname ::= string(A). {pss->name = A;}

vorder ::= ASSIGN integer(A). {pss->order = A;}
vorder ::= .

vdefault ::= LSBRACKET DEFAULT ASSIGN string(A) RSBRACKET . {pss->defVal = A;}
vdefault ::= LSBRACKET PACKED ASSIGN string(A) RSBRACKET . {pss->defVal = A;}
vdefault ::= .


/* Comment */
comment ::= COMMENT(A). {if(pss->comment[0] != 0) strcat(pss->comment, "\n"); strcat(pss->comment, A);}

/* Basic types */
%type integer {int}
integer(I) ::= CUSTOM(A). {I = str_to_uint(pss, A);}

%type string {const char *}
string(N) ::= CUSTOM(A). {N = A;}
