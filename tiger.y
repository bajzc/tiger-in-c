%{
#define YY_NO_UNISTD_H
#define YY_NO_STDIO_H
#define YY_NO_STDLIB_H
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "errormsg.h"
#include "parse.h"

int yylex(void); /* function prototype */

void yyerror(char *s);

A_exp absyn_root = NULL;
%}

%locations

%union {
	int pos;
	int ival;
	string sval;
	A_exp exp;
	A_expList exp_list;
	A_ty ty;
	A_fundec fundec;
	A_fundecList fundecs;
	A_fieldList tyfields;
	A_decList decs;
	A_dec dec;
	A_var var;
	A_efieldList record_list;
	A_nametyList tydecs;
	A_namety tydec;
	}

%token <sval> ID STRING
%token <ival> INT
%type <exp> program exp
%type <exp_list> expseq exp_list exp_sequence
%type <ty> ty
%type <fundec> fundec
%type <fundecs> fundecs
%type <tyfields> tyfields
%type <decs> decs
%type <dec> dec vardec
%type <var> lvalue
%type <record_list> record_list
%type <tydecs> tydecs
%type <tydec> tydec

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

program:
  exp
  { $$ = absyn_root = $1; }

exp:
/* aka ID */
  lvalue
  { $$ = A_VarExp(@$, $1); }
| NIL
  { $$ = A_NilExp(@$); }
/* Sequencing (exp;exp{;exp ...}) */
| LPAREN exp_sequence RPAREN
  { $$ = A_SeqExp(@1, $2); }
| LPAREN RPAREN
  { $$ = NULL; }
| INT
  { $$ = A_IntExp(@$, $1); }
| STRING
  { $$ = A_StringExp(@$, $1); }
/* Negation */
| MINUS exp %prec UMINUS
  { $$ = A_OpExp(@$, A_minusOp, A_IntExp(@$, 0), $2); }
/* Function call */
| ID LPAREN RPAREN
  { $$ = A_CallExp(@$, S_Symbol($1), NULL); }
| ID LPAREN exp_list RPAREN
  { $$ = A_CallExp(@$, S_Symbol($1), $3); }
/* Arithmetic */
| exp PLUS exp
  { $$ = A_OpExp(@$, A_plusOp, $1, $3); }
| exp MINUS exp
  { $$ = A_OpExp(@$, A_minusOp, $1, $3); }
| exp TIMES exp
  { $$ = A_OpExp(@$, A_timesOp, $1, $3); }
| exp DIVIDE exp
  { $$ = A_OpExp(@$, A_divideOp, $1, $3); }
/* Comparison */
| exp EQ exp
  { $$ = A_OpExp(@$, A_eqOp, $1, $3); }
| exp NEQ exp
  { $$ = A_OpExp(@$, A_neqOp, $1, $3); }
| exp GT exp
  { $$ = A_OpExp(@$, A_gtOp, $1, $3); }
| exp LT exp
  { $$ = A_OpExp(@$, A_ltOp, $1, $3); }
| exp GE exp
  { $$ = A_OpExp(@$, A_geOp, $1, $3); }
| exp LE exp
  { $$ = A_OpExp(@$, A_leOp, $1, $3); }
/* Boolean op */
| exp AND exp
  { $$ = A_IfExp(@$, $1, $3, A_IntExp(@3, 0)); }
| exp OR exp
  { $$ = A_IfExp(@$, $1, A_IntExp(@2, 1), $3); }
/* Record FIX */
| ID LBRACE record_list RBRACE
  { $$ = A_RecordExp(@$, S_Symbol($1), $3); }
| ID LBRACE RBRACE
  { $$ = A_RecordExp(@$, S_Symbol($1), NULL);}
/* Array */
| ID LBRACK exp RBRACK OF exp
  { $$ = A_ArrayExp(@$, S_Symbol($1), $3, $6); }
/* Assignment */
| lvalue ASSIGN exp
  { $$ = A_AssignExp(@$, $1, $3); }
/* If */
| IF exp THEN exp
  { $$ = A_IfExp(@$, $2, $4, NULL); }
| IF exp THEN exp ELSE exp
  { $$ = A_IfExp(@$, $2, $4, $6); }
/* While */
| WHILE exp DO exp
  { $$ = A_WhileExp(@$, $2, $4); }
/* For */
| FOR ID ASSIGN exp TO exp DO exp
  { $$ = A_ForExp(@$, S_Symbol($2), $4, $6, $8); }
/* Break */
| BREAK
  { $$ = A_BreakExp(@$); }
/* Let */
| LET decs IN expseq END
  { $$ = A_LetExp(@$, $2, A_SeqExp(@4, $4)); }
/* Parentheses */
| LPAREN exp RPAREN
  { $$ = $2; }

expseq: /* 0 or more exps */
  /* empty */
  { $$ = NULL; }
| exp
  { $$ = A_ExpList($1, NULL); }
| exp SEMICOLON expseq
  { $$ = A_ExpList($1, $3); }

record_list:
  ID EQ exp
  { $$ = A_EfieldList(A_Efield(S_Symbol($1), $3), NULL); }
| ID EQ exp COMMA record_list
  { $$ = A_EfieldList(A_Efield(S_Symbol($1), $3), $5); }

exp_list: /* one or more exps */
  exp
  { $$ = A_ExpList($1, NULL); }
| exp COMMA exp_list
  { $$ = A_ExpList($1, $3); }

exp_sequence: /* two or more exps */
  exp SEMICOLON exp
  { $$ = A_ExpList($1, A_ExpList($3, NULL)); }
| exp SEMICOLON exp_sequence
  { $$ = A_ExpList($1, $3); }

decs:
  /* empty */
  { $$ = NULL; }
| dec decs
  { $$ = A_DecList($1, $2); }

dec:
  tydecs
  { $$ = A_TypeDec(@$, $1); }
| vardec
  { $$ = $1; }
| fundecs
  { $$ = A_FunctionDec(@$, $1); }

tydecs:
  tydec
  { $$ = A_NametyList($1, NULL); }
| tydec tydecs
  { $$ = A_NametyList($1, $2); }

tydec: 
  TYPE ID EQ ty
  { $$ = A_Namety(S_Symbol($2), $4); }

ty:
  ID
  { $$ = A_NameTy(@$, S_Symbol($1)); }
| LBRACE tyfields RBRACE
  { $$ = A_RecordTy(@$, $2); }
| ARRAY OF ID
  { $$ = A_ArrayTy(@$, S_Symbol($3)); }

tyfields: 
/* empty */
  { $$ = NULL; }
| ID COLON ID
  { $$ = A_FieldList(A_Field(@1, S_Symbol($1), S_Symbol($3)), NULL); }
| ID COLON ID COMMA tyfields 
  { $$ = A_FieldList(A_Field(@1, S_Symbol($1), S_Symbol($3)), $5); }

vardec:
  VAR ID ASSIGN exp
  { $$ = A_VarDec(@$, S_Symbol($2), NULL, $4); }
| VAR ID COLON ID ASSIGN exp
  { $$ = A_VarDec(@$, S_Symbol($2), S_Symbol($4), $6); }

fundecs:
  fundec
  { $$ = A_FundecList($1, NULL); }
| fundec fundecs
  { $$ = A_FundecList($1, $2); }

fundec:
  FUNCTION ID LPAREN tyfields RPAREN EQ exp
  { $$ = A_Fundec(@$, S_Symbol($2), $4, NULL, $7); }
| FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp
  { $$ = A_Fundec(@$, S_Symbol($2), $4, S_Symbol($7), $9); }

lvalue:
  ID
  { $$ = A_SimpleVar(@$, S_Symbol($1)); }
| lvalue DOT ID
  { $$ = A_FieldVar(@$, $1, S_Symbol($3)); }
| lvalue LBRACK exp RBRACK
  { $$ = A_SubscriptVar(@$, $1, $3); }
| ID LBRACK exp RBRACK
  { $$ = A_SubscriptVar(@$, A_SimpleVar(@1, S_Symbol($1)), $3); }
%%
void yyerror(char *s)
{
  EM_error(yylloc, "%s", s);

}
