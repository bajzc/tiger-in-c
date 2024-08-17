%{
#include <string.h>
#include <stdio.h>
#include "util.h"
#include "tiger.tab.h"
#include "errormsg.h"
#define YYDEBUG 1

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
      EM_error(EM_tokPos, "comment doesn't match");                            \
    else if (comment_nest == 0)                                                \
      BEGIN(INITIAL);                                                          \
  } while (0)

int charPos=1;

int comment_nest = 0;

char string_buf[1024];
char *str_ptr;

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

%}
%x COMMENT STR
%%
" "	 {adjust();}
\n	 {adjust(); EM_newline();}
\t       {adjust();}
"/*"     {adjust(); COMMENT_IN;}
\"  	 {adjust(); str_ptr = string_buf; BEGIN(STR);}
<STR>{ /* https://www.cs.virginia.edu/~cr4bd/flex-manual/Start-Conditions.html */
\" 	 {adjust(); *str_ptr = '\0'; yylval.sval = String(string_buf);
	  BEGIN(INITIAL); return STRING;}
\\[0-9]{3} {adjust();
            char x = (char)(C2N(yytext[1])*100+C2N(yytext[2])*10+C2N(yytext[3]));
			*str_ptr++ = x;}
\\n 	 {adjust(); *str_ptr++ = '\n';}
\\t 	 {adjust(); *str_ptr++ = '\t';}
\\\^.    {adjust();
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
\\\\     {adjust(); *str_ptr++ = '\\';}
\\\"     {adjust(); *str_ptr++ = '\"';}
\\(\t|\n|" ")+\\ {adjust();}
[^\\\n\"]+        {
            char *yptr = yytext;
            while ( *yptr )
                    *str_ptr++ = *yptr++;
            }
}
<COMMENT>{
"/*"     {adjust(); COMMENT_IN;}
\n 	 {adjust(); EM_newline();}
"*/"	 {adjust(); COMMENT_OUT;}
. 	 {adjust(); continue;}
}
","	 {adjust(); return COMMA;}
":"      {adjust(); return COLON;}
";"      {adjust(); return SEMICOLON;}
"("      {adjust(); return LPAREN;}
")"	 {adjust(); return RPAREN;}
"["      {adjust(); return LBRACK;}
"]"	 {adjust(); return RBRACK;}
"{"	 {adjust(); return LBRACE;}
"}"	 {adjust(); return RBRACE;}
"."	 {adjust(); return DOT;}
"+"	 {adjust(); return PLUS;}
"-" 	 {adjust(); return MINUS;}
"*"	 {adjust(); return TIMES;}
"/"   	 {adjust(); return DIVIDE;}
":="  	 {adjust(); return ASSIGN;}
"="      {adjust(); return EQ;}
"<>"     {adjust(); return NEQ;}
"<"  	 {adjust(); return LT;}
"<="  	 {adjust(); return LE;}
">"  	 {adjust(); return GT;}
">="  	 {adjust(); return GE;}
"&"  	 {adjust(); return AND;}
"|"  	 {adjust(); return OR;}
while	 {adjust(); return WHILE;}
for  	 {adjust(); return FOR;}
to 	     {adjust(); return TO;}
break    {adjust(); return BREAK;}
let      {adjust(); return LET;}
in       {adjust(); return IN;}
end      {adjust(); return END;}
function {adjust(); return FUNCTION;}
var      {adjust(); return VAR;}
type     {adjust(); return TYPE;}
array    {adjust(); return ARRAY;}
if       {adjust(); return IF;}
then     {adjust(); return THEN;}
else     {adjust(); return ELSE;}
do       {adjust(); return DO;}
of       {adjust(); return OF;}
nil      {adjust(); return NIL;}

[a-zA-Z][a-zA-Z0-9_]* {adjust(); yylval.sval=String(yytext); return ID;}
[0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}

.	 {adjust(); EM_error(EM_tokPos,"illegal token");}
