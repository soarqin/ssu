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

%extra_argument {SSUParseStruct *sps}

%syntax_error {
  onSyntaxError(sps);
}

%stack_overflow {
  onStackOverflow(sps);
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
eoption ::= OPTION okeyword ASSIGN ovalue DELIM. {sps->ss.options[sps->ssh.tname] = sps->ssh.name; sps->ssh.reset();}
okeyword ::= string(A). {sps->ssh.tname = A;}
ovalue ::= string(A). {sps->ssh.name = A;}

/* Package keyword */
epackage ::= PACKAGE pname DELIM. {sps->ss.packageName = sps->ssh.name; sps->ssh.reset();}
pname ::= string(A). {sps->ssh.name = A;}

/* Import keyword */
eimport ::= IMPORT iname DELIM.
iname ::= string(A). { importFile(sps, A); }

/* Enum keyword */
eenum ::= eenumstart LBRACE evallist RBRACE. {endEnum(sps);}

eenumstart ::= ENUM ename. {addEnum(sps); sps->ssh.reset();}

ename ::= string(A). {sps->ssh.name = A;}

evallist ::= evallist eval.
evallist ::= evallist comment.
evallist ::= .

eval ::= evalname ASSIGN evalval DELIM. {addEnumVal(sps); sps->ssh.reset();}
evalname ::= string(A). {sps->ssh.name = A;}
evalval ::= integer(A). {sps->ssh.order = A;}

/* Struct/message/class keyword */
estruct ::= estructstart LBRACE deflist RBRACE. {endStruct(sps);}

estructstart ::= STRUCT sname. {addStruct(sps); sps->ssh.reset();}

sname ::= string(A). {sps->ssh.order = 0; sps->ssh.name = A;}
sname ::= string(A) LSBRACKET integer(B) RSBRACKET. {sps->ssh.order = B; sps->ssh.name = A;}


/* Field defines */
deflist ::= deflist edef.
deflist ::= deflist comment.
deflist ::= .

edef ::= preoption vconstraint vtype vname vorder vdefault DELIM. {appendField(sps); sps->ssh.reset();}
edef ::= estruct.
edef ::= eenum.

preoption ::= LSBRACKET voption RSBRACKET.
preoption ::= .

voption ::= fieldoption.
voption ::= voption COMMA fieldoption.

fieldoption ::= REQUIRED. {sps->ssh.constraint = 1;}
fieldoption ::= OPTIONAL. {sps->ssh.constraint = 2;}
fieldoption ::= REPEATED. {sps->ssh.constraint = 3;}
fieldoption ::= ORDERMAP. {sps->ssh.constraint = 4;}
fieldoption ::= integer(I). {sps->ssh.order = I;}

vconstraint ::= REQUIRED. {sps->ssh.constraint = 1;}
vconstraint ::= OPTIONAL. {sps->ssh.constraint = 2;}
vconstraint ::= REPEATED. {sps->ssh.constraint = 3;}
vconstraint ::= ORDERMAP. {sps->ssh.constraint = 4;}
vconstraint ::= . { if(sps->ssh.constraint == 0) sps->ssh.constraint = 1; }

vtype ::= INTEGER. {sps->ssh.type = TYPE_INT; sps->ssh.tname = "";}
vtype ::= SINTEGER. {sps->ssh.type = TYPE_SINT; sps->ssh.tname = "";}
vtype ::= UINTEGER. {sps->ssh.type = TYPE_UINT; sps->ssh.tname = "";}
vtype ::= INTEGER64. {sps->ssh.type = TYPE_INT64; sps->ssh.tname = "";}
vtype ::= SINTEGER64. {sps->ssh.type = TYPE_SINT64; sps->ssh.tname = "";}
vtype ::= UINTEGER64. {sps->ssh.type = TYPE_UINT64; sps->ssh.tname = "";}
vtype ::= FLOAT. {sps->ssh.type = TYPE_FLOAT; sps->ssh.tname = "";}
vtype ::= DOUBLE. {sps->ssh.type = TYPE_DOUBLE; sps->ssh.tname = "";}
vtype ::= STRING. {sps->ssh.type = TYPE_STRING; sps->ssh.tname = "";}
vtype ::= BOOL. {sps->ssh.type = TYPE_BOOL; sps->ssh.tname = "";}
vtype ::= string(A). {sps->ssh.type = TYPE_STRUCT; sps->ssh.tname = A;}

vname ::= string(A). {sps->ssh.name = A;}

vorder ::= ASSIGN integer(A). {sps->ssh.order = A;}
vorder ::= .

vdefault ::= LSBRACKET DEFAULT ASSIGN string(A) RSBRACKET . {sps->ssh.defVal = A;}
vdefault ::= LSBRACKET PACKED ASSIGN string(A) RSBRACKET . {sps->ssh.defVal = A;}
vdefault ::= .


/* Comment */
comment ::= COMMENT(A). {if(sps->ssh.comment[0] != 0) strcat(sps->ssh.comment, "\n"); strcat(sps->ssh.comment, A);}

/* Basic types */
%type integer {int}
integer(I) ::= CUSTOM(A). {I = str_to_uint(sps, A);}

%type string {const char *}
string(N) ::= CUSTOM(A). {N = A;}
