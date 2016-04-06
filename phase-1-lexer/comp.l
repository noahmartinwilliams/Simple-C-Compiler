%{
#include "comp.tab.h"
#include "globals.h"
#include <stdlib.h>
#include <string.h>
#include "parser/types.h"
#include "parser/consts.h"
#include "lexer/bases.h"
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

void handle_preproc_token(char *str)
{
	/* TODO: get this working with the haiku easter egg (or don't). */
	int x=1;
	free(current_line_str);
	current_line_str=NULL;
	free(current_file);
	current_file=NULL;
	while ((str[x]==' ' || str[x]=='\t') && str[x]!='\0')
		x++;

	#define preproc_error \
	if (str[x]=='\0') {\
		fprintf(stderr, "Internal preprocessing error on line %d\n", __LINE__);\
		exit(1);\
	}
	preproc_error
	int y=x;
	while (str[y]!=' ' && str[y]!='\t' && str[y]!='\0')
		y++;

	preproc_error
	
	char c=str[y];
	str[y]='\0';

	current_line=atoi(str+x);

	str[y]='\0';

	while (str[x]!='"' )
		x++;
	
	preproc_error

	y=++x;
	while (str[y]!='"' )
		y++;

	preproc_error
	
	str[y]='\0';

	current_file=strdup(str+x);
	str[y]='"';
	current_char=0;
	#undef preproc_error
}

%}

%x in_comment
%%

"/*"	{ count(yytext); BEGIN(in_comment); }
\"(\\\\|[^"\\]|\\n)*\"	{ count(yytext); yylval.str=strdup(yytext); return STR_LITERAL; }
'.'	{ count(yytext); yylval.chr=yytext[1]; return CHAR_LITERAL; }
'\\\\'	{ count(yytext); yylval.chr='\\'; return CHAR_LITERAL; }
'\\n'	{ count(yytext); yylval.chr='\n'; return CHAR_LITERAL; }
'\\0'	{ count(yytext); yylval.chr='\0'; return CHAR_LITERAL; }
\/\/.*$	{ count(yytext); }
^\#[[:space:]]*[0-9]+[[:space:]]*\".*\".*$	{ handle_preproc_token(yytext); count(yytext); }
0[0-9]+	{ count(yytext); yylval.l=oct2i(yytext); return CONST_INT; }
0x[0-9a-fA-F]+	{ count(yytext); yylval.l=hex2i(yytext); return CONST_INT; }
"short"	{ count(yytext); return SHORT; }
"."	{ count(yytext); return '.'; }
"->"	{ count(yytext); return POINTER_OP; }
"..."	{ count(yytext); return MULTI_ARGS; }
"signed"	{ count(yytext); return SIGNED; }
"unsigned"	{ count(yytext); return UNSIGNED; }
"long"	{ count(yytext); return LONG; }
"enum"	{ count(yytext); return ENUM; }
"const"	{ count(yytext); return CONST; }
"__alignof__"	{ count(yytext); return ALIGNOF; }
"register"	{ count(yytext); return REGISTER; }
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
"["	{ count(yytext); return '['; }
"]"	{ count(yytext); return ']'; }
[ \t\n]	{ count(yytext); }
[_a-zA-Z][_a-zA-Z0-9]*	{ count(yytext); 
	haiku_easter_egg(yytext); 
	if (get_type_by_name(yytext, _normal)==NULL ) { 
		yylval.str=strdup(yytext); 
		return IDENTIFIER ; 
	} else { 
		yylval.type=get_type_by_name(yytext, _normal); 
		return TYPE;
	} }
<in_comment>[^*\n]*	{ count(yytext); }
<in_comment>"*"+[^*/\n]*	{ count(yytext); }
<in_comment>\n	{ count(yytext); }
<in_comment>"*"+"/"	{ count(yytext); BEGIN(INITIAL); }
.	{ count(yytext); yyerror("Unknown token."); }
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
