#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "generator/generator.h"
#include "globals.h"
#include "handle-exprs.h"
#include "handle-funcs.h"
#include "handle-statems.h"
#include "handle-types.h"
#include "handle-vars.h"
#ifdef DEBUG
#include "printer.h"
#include "print-tree.h"
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
	backend_name=strdup(argv[3]);
	current_file=argv[2];
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
	fclose(output);
	return 0;
}
