%{
#include "comp.tab.h"
#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include "handle-types.h"
void count(char *s);

#ifdef HAIKU_EASTER_EGG
extern void yyerror(char *s);
static inline void haiku_easter_egg(char *s)
{
	int x;
	for (x=0; x<strlen(s); x++) {
		switch (s[x]) {
		case 'a':
		case 'e':
		case 'i':
		case 'o':
		case 'u':
		number_of_syllables++;
		}
	}
}
#else
#define haiku_easter_egg(X )
#endif
%}


%%
\"(\\\\|[^"\\])*\"	{ count(yytext); yylval.str=strdup(yytext); return STR_LITERAL; }
'.'	{ count(yytext); yylval.chr=yytext[1]; return CHAR_LITERAL; }
"extern"	{ count(yytext); return EXTERN; }
"return"	{ count(yytext); return RET; }
"if"	{ count(yytext); return IF; }
"else"	{ count(yytext); return ELSE; }
"while"	{ count(yytext); return WHILE; }
"break"	{ count(yytext); return BREAK; }
"continue"	{ count(yytext); return CONTINUE; }
"struct"	{ count(yytext); return STRUCT; }
[0-9]+	{ count(yytext); yylval.l=atoi(yytext); return CONST_INT; }
[+-]=	{ count(yytext); yylval.str=strdup(yytext); return ASSIGN_OP; }
"&"	{ count(yytext); return '&'; }
"|"	{ count(yytext); return '|'; }
"<<"	{ count(yytext); return SHIFT_LEFT; }
">>"	{ count(yytext); return SHIFT_RIGHT; }
"!="	{ count(yytext); return NE_TEST; }
"<="	{ count(yytext); return LE_TEST; }
">="	{ count(yytext); return GE_TEST; }
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
[_a-zA-Z][_a-zA-Z0-9]*	{ count(yytext); haiku_easter_egg(yytext); if (get_type_by_name(yytext)==NULL) { yylval.str=strdup(yytext); return IDENTIFIER ; } else { yylval.type=get_type_by_name(yytext); return TYPE; } }
%%

void count(char *s)
{
	if (current_line_str==NULL) {
		current_line_str=realloc(current_line_str, sizeof(char)*(strlen(s)+1));
		memset(current_line_str, 0, strlen(s));
	} else
		current_line_str=realloc(current_line_str, sizeof(char)*(strlen(current_line_str)+strlen(s)+1));

	strcat(current_line_str, s);
	for (; *s; s++) {
		if (*s=='\n') {
			current_line++;
			current_char=0;
#ifdef HAIKU_EASTER_EGG
			if (current_haiku_line==1 && number_of_syllables!=5) {
				goto error;
			}

			if (current_haiku_line==2 && number_of_syllables!=7) {
				goto error;
			}

			if (current_haiku_line==3 && number_of_syllables!=5) {
				goto error;
			} else {
				current_haiku_line=0;
				number_of_syllables++;
			}

			current_haiku_line++;

			continue;
			error:
			yyerror("Warning: Identifiers better written as a haiku");
#endif
			free(current_line_str);
			current_line_str=NULL;
		}
		current_char++;
	}
}

int yywrap()
{
	return 1;
}
