%{
#define YY_NO_UNISTD_H
#include "util.h"
#include "symbol.h"
#include "absyn.h"
#include "tiger.tab.h"
#include "errormsg.h"

// char to num, no check
#define C2N(c) ((int)c-48)

#define COMMENT_IN                                                             \
  do {                                                                         \
    if (comment_nest == 0)                                                     \
      BEGIN(COMMENT);                                                          \
    comment_nest += 1;                                                         \
  } while (0)

#define COMMENT_OUT                                                            \
  do {                                                                         \
    comment_nest -= 1;                                                         \
    if (comment_nest < 0)                                                      \
      EM_error(yylloc, "comment doesn't match");                               \
    else if (comment_nest == 0)                                                \
      BEGIN(INITIAL);                                                          \
  } while (0)

/* location tracking */
#define YY_USER_ACTION                                                         \
  {                                                                            \
    yylloc.first_line = yylloc.last_line = yylineno;                           \
    yylloc.first_column = columnPos; yylloc.last_column = columnPos+yyleng-1;  \
    columnPos += yyleng;                                                       \
  }

int columnPos = 1;
int comment_nest = 0;

char string_buf[1024];
char *str_ptr;

int yywrap(void)
{
 columnPos=1;
 return 1;
}

%}
%x COMMENT STR

/* location tracking */
%option yylineno
%%
" "	 {}
\n	 {columnPos = 1;}
\t       {}
"/*"     {COMMENT_IN;}
\"  	 {str_ptr = string_buf; BEGIN(STR);}
<STR>{ /* https://www.cs.virginia.edu/~cr4bd/flex-manual/Start-Conditions.html */
\" 	 {*str_ptr = '\0'; yylval.sval = String(string_buf);
	  BEGIN(INITIAL); return STRING;}
\\[0-9]{3} {
            char x = (char)(C2N(yytext[1])*100+C2N(yytext[2])*10+C2N(yytext[3]));
			*str_ptr++ = x;}
\\n 	 {*str_ptr++ = '\n';}
\\t 	 {*str_ptr++ = '\t';}
\\\^.    {
        switch(yytext[2]){
		case '@':
		  *str_ptr++ = 0x00; // NULL
		  break;
		case 'C':
		case 'c':
		  *str_ptr++ = 0x03; // EXT
		  break;
		case 'G':
		  *str_ptr++ = 0x07; // bell
		  break;
		case 'H':
		  *str_ptr++ = 0x08; // backspace
		  break;
		case 'I':
		  *str_ptr++ = 0x09; // tab
		  break;
		case 'J':
		  *str_ptr++ = 0x0A; // line feed
		  break;
		case 'K':
		  *str_ptr++ = 0x0B; // vertical tab
		  break;
		case 'L':
		  *str_ptr++ = 0x0C;
		  break;
		case 'M':
		  *str_ptr++ = 0x0D;
		  break;
		case 'Z':
		  *str_ptr++ = 0x1A;
		  break;
		case '[':
		  *str_ptr++ = 0x1B;
		  break;
		default:
		  assert(0);
		}}
\\\\     {*str_ptr++ = '\\';}
\\\"     {*str_ptr++ = '\"';}
\\(\t|\n|" ")+\\ {}
[^\\\n\"]+        {
            char *yptr = yytext;
            while ( *yptr )
                    *str_ptr++ = *yptr++;
            }
}
<COMMENT>{
"/*"     {COMMENT_IN;}
\n 	 {columnPos = 1;}
"*/"	 {COMMENT_OUT;}
. 	 {continue;}
}
","	 {return COMMA;}
":"      {return COLON;}
";"      {return SEMICOLON;}
"("      {return LPAREN;}
")"	 {return RPAREN;}
"["      {return LBRACK;}
"]"	 {return RBRACK;}
"{"	 {return LBRACE;}
"}"	 {return RBRACE;}
"."	 {return DOT;}
"+"	 {return PLUS;}
"-" 	 {return MINUS;}
"*"	 {return TIMES;}
"/"   	 {return DIVIDE;}
":="  	 {return ASSIGN;}
"="      {return EQ;}
"<>"     {return NEQ;}
"<"  	 {return LT;}
"<="  	 {return LE;}
">"  	 {return GT;}
">="  	 {return GE;}
"&"  	 {return AND;}
"|"  	 {return OR;}
while	 {return WHILE;}
for  	 {return FOR;}
to 	 {return TO;}
break    {return BREAK;}
let      {return LET;}
in       {return IN;}
end      {return END;}
function {return FUNCTION;}
var      {return VAR;}
type     {return TYPE;}
array    {return ARRAY;}
if       {return IF;}
then     {return THEN;}
else     {return ELSE;}
do       {return DO;}
of       {return OF;}
nil      {return NIL;}

[a-zA-Z][a-zA-Z0-9_]* {yylval.sval=String(yytext); return ID;}
[0-9]+	 {yylval.ival=atoi(yytext); return INT;}

.	 {EM_error(yylloc, "illegal token");}
