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

%x in_comment
%%

"/*"	{ count(yytext); BEGIN(in_comment); }
\"(\\\\|[^"\\]|\\n)*\"	{ count(yytext); yylval.str=strdup(yytext); return STR_LITERAL; }
'.'	{ count(yytext); yylval.chr=yytext[1]; return CHAR_LITERAL; }
\/\/.*$	{ count(yytext); }
"."	{ count(yytext); return '.'; }
"->"	{ count(yytext); return POINTER_OP; }
"..."	{ count(yytext); return MULTI_ARGS; }
"__alignof__"	{ count(yytext); return ALIGNOF; }
"default"	{ count(yytext); return DEFAULT; }
"switch"	{ count(yytext); return SWITCH; }
"case"	{ count(yytext); return CASE; }
"sizeof"	{ count(yytext); return SIZEOF; }
"inline"	{ count(yytext); return INLINE; }
"union"	{ count(yytext); return UNION; }
"static"	{ count(yytext); return STATIC; }
"typedef"	{ count(yytext); return TYPEDEF; }
"do"	{ count(yytext); return DO; }
"for"	{ count(yytext); return FOR; }
"goto"	{ count(yytext); return GOTO; }
"extern"	{ count(yytext); return EXTERN; }
"return"	{ count(yytext); return RETURN; }
"if"	{ count(yytext); return IF; }
"else"	{ count(yytext); return ELSE; }
"while"	{ count(yytext); return WHILE; }
"break"	{ count(yytext); return BREAK; }
"continue"	{ count(yytext); return CONTINUE; }
"struct"	{ count(yytext); return STRUCT; }
[0-9]+	{ count(yytext); yylval.l=atoi(yytext); return CONST_INT; }
[\/*%^&|+-]=	{ count(yytext); yylval.str=strdup(yytext); return ASSIGN_OP; }
"--"	{ count(yytext); return DEC_OP; }
"++"	{ count(yytext); return INC_OP; }
"!"	{ count(yytext); return '!'; }
"%"	{ count(yytext); return '%'; }
"||"	{ count(yytext); return TEST_OR; }
"&&"	{ count(yytext); return TEST_AND; }
"^"	{ count(yytext); return '^'; }
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
"{"	{ scope_depth++; count(yytext); return '{'; }
"}"	{ hide_current_scope(); scope_depth--; count(yytext); return '}'; }
","	{ count(yytext); return ','; }
":"	{ count(yytext); return ':'; }
"?"	{ count(yytext); return '?'; }
"&"	{ count(yytext); return '&'; }
"~"	{ count(yytext); return '~'; }
[ \t\n]	{ count(yytext); }
[_a-zA-Z][_a-zA-Z0-9]*	{ count(yytext); haiku_easter_egg(yytext); if (get_type_by_name(yytext)==NULL) { yylval.str=strdup(yytext); return IDENTIFIER ; } else { yylval.type=get_type_by_name(yytext); return TYPE; } }
<in_comment>[^*\n]*	{ count(yytext); }
<in_comment>"*"+[^*/\n]*	{ count(yytext); }
<in_comment>\n	{ count(yytext); }
<in_comment>"*"+"/"	{ count(yytext); BEGIN(INITIAL); }
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
