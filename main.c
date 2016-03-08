#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "generator/generator.h"
#include "globals.h"
#include "parser/exprs.h"
#include "parser/funcs.h"
#include "parser/consts.h"
#include "parser/statems.h"
#include "parser/types.h"
#include "parser/vars.h"
#ifdef DEBUG
#include "printer.h"
#include "print-tree.h"
extern int yy_flex_debug;
#endif
#include "types.h"

extern struct type_t *current_type;
extern void yyparse();
extern FILE* yyin;
extern FILE *output;
char *backend_name=NULL;
int main(int argc, char *argv[])
{
	if (argc<3) {
		fprintf(stderr, "Usage: %s output_file.s input_file.c \n", argv[0]);
		exit(1);
	}
	#ifdef DEBUG
	yy_flex_debug=0;
	#endif
	backend_name=strdup(argv[3]);
	current_file=strdup(argv[2]);
	yyin=fopen(argv[2], "r");
	output=fopen(argv[1], "w+");
	setup_generator();
	yyparse();
	free(current_function);
	free_type(current_type);
	free_all_funcs();
	cleanup_backend();
	free_all_types();
	free_all_vars();
	free_all_constants();
	free(current_file);
	fclose(output);
	return 0;
}
