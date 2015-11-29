%{
#include "comp.tab.h"
#include "globals.h"
#include <stdlib.h>
void count(char *s);
%}

%%
"return"	{ count(yytext); return RET; }
"if"	{ count(yytext); return IF; }
"else"	{ count(yytext); return ELSE; }
"while"	{ count(yytext); return WHILE; }
"break"	{ count(yytext); return BREAK; }
"continue"	{ count(yytext); return CONTINUE; }
"struct"	{ count(yytext); return STRUCT; }
[0-9]+	{ count(yytext); yylval.l=atoi(yytext); return CONST_INT; }
[+-]=	{ count(yytext); yylval.str=strdup(yytext); return ASSIGN_OP; }
"!="	{ count(yytext); return NE_TEST; }
"=="	{ count(yytext); return EQ_TEST; }
">"	{ count(yytext); return '>'; }
"<"	{ count(yytext); return '<'; }
"="	{ count(yytext); return '='; }
"("	{ count(yytext); return '('; }
")"	{ count(yytext); return ')'; }
"+"	{ count(yytext); return '+'; }
"-"	{ count(yytext); return '-'; }
"/"	{ count(yytext); return '/'; }
"*"	{ count(yytext); return '*'; }
";"	{ count(yytext); return ';'; }
"{"	{ scope++; count(yytext); return '{'; }
"}"	{ hide_current_scope(); scope--; count(yytext); return '}'; }
","	{ count(yytext); return ','; }
"&"	{ count(yytext); return '&'; }
[ \t\n]	{ count(yytext); }
[_a-zA-Z][_a-zA-Z0-9]*	{ count(yytext); yylval.str=strdup(yytext); return IDENTIFIER ; }
%%

void count(char *s)
{
	for (; *s; s++) {
		if (*s=='\n') {
			current_line++;
			current_char=0;
		}
		current_char++;
	}
}

int yywrap()
{
	return 1;
}
