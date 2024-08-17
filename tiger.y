%{
#include <stdio.h>
#include "util.h"
#include "errormsg.h"

int yylex(void); /* function prototype */

void yyerror(char *s)
{
 EM_error(EM_tokPos, "%s", s);
}
%}


%union {
	int pos;
	int ival;
	string sval;
	}

%token <sval> ID STRING
%token <ival> INT

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK 
  LBRACE RBRACE DOT 
  PLUS MINUS TIMES DIVIDE EQ NEQ LT LE GT GE
  AND OR ASSIGN
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF 
  BREAK NIL
  FUNCTION VAR TYPE 

%start program

// slove S/R conflict
%nonassoc OF DO
// dangling else
%nonassoc THEN
%left ELSE


%nonassoc ASSIGN
%left OR
%left AND
%nonassoc EQ NEQ LT LE GT GE
%left PLUS MINUS
%left TIMES DIVIDE
%left UMINUS

%%

program:	exp

exp:
  lvalue /* ID */
| NIL
/* Sequencing (exp,exp{,exp ...}) */
| LPAREN exp_sequence RPAREN
| LPAREN RPAREN
| INT
| STRING
/* Negation */
| MINUS exp %prec UMINUS
/* Function call */
| ID LPAREN RPAREN
| ID LPAREN exp_list RPAREN
/* Arithmetic */
| exp PLUS exp
| exp MINUS exp
| exp TIMES exp
| exp DIVIDE exp
/* Comparison */
| exp EQ exp
| exp NEQ exp
| exp GT exp
| exp LT exp
| exp GE exp
| exp LE exp
/* Boolean op */
| exp AND exp
| exp OR exp
/* Record */
| ID LBRACE record_list RBRACE
| ID LBRACE RBRACE
/* Array */
| ID LBRACK exp RBRACK OF exp // FIXIT ../testcases/queens.tig:16.26: syntax error
/* Assignment */
| lvalue ASSIGN exp
/* If */
| IF exp THEN exp
| IF exp THEN exp ELSE exp
/* While */
| WHILE exp DO exp
/* For */
| FOR ID ASSIGN exp TO exp DO exp
/* Break */
| BREAK
/* Let */
| LET decs IN expseq END
/* Parentheses */
| LPAREN exp RPAREN

expseq: /* 0 or more exps */
  /* empty */
| exp
| expseq SEMICOLON exp

record_list:
  ID EQ exp
| record_list COMMA ID EQ exp

exp_list: /* one or more exps */
  exp
| exp_list COMMA exp


exp_sequence: /* two or more exps */
  exp SEMICOLON exp
| exp_sequence SEMICOLON exp

decs:
  /* empty */
| decs dec

dec:
  tydec
| vardec
| fundec

tydec: TYPE ID EQ ty

ty:
  ID
| LBRACE tyfields RBRACE
| ARRAY OF ID

tyfields: 
/* empty */
| ID COLON ID
| tyfields COMMA ID COLON ID

vardec:
  VAR ID ASSIGN exp
| VAR ID COLON ID ASSIGN exp

fundec:
  FUNCTION ID LPAREN tyfields RPAREN EQ exp
| FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp

lvalue:
  ID
| lvalue DOT ID
| lvalue LBRACK exp RBRACK
| ID LBRACK exp RBRACK
