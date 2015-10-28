%{
#include "comp.tab.h"
#include "globals.h"
void count(char *s);
%}

%%
[0-9]+	{ count(yytext); yylval.l=atoi(yytext); return CONST_INT; }
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
