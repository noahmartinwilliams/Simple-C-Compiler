#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities/types.h"
#include "generator/types.h"
#include "generator/generator.h"
#include "generator/globals.h"
#include "backend/agnostic-arithmetic.h"
#include "backend/int-arithmetic.h"
#include "backend/variables.h"
#include "backend/registers.h"
#include "backend/calls.h"
#include "backend/jumps.h"
#include "globals.h"
#include "types.h"
#include "backend/globals.h"

static inline void size_error(char *message, size_t size)
{
	fprintf(stderr, "Internal Error: unknown size: %ld passed to %s\n", size, message);
	exit(1);
}

void assign_constant(FILE *fd, struct expr_t *e)
{
	if (get_type_size(e->type)==char_size)
		fprintf(fd, "\tmovb $%ld, %%al\n", e->attrs.cint_val);
	else if (get_type_size(e->type)==short_size)
		fprintf(fd, "\tmovw $%ld, %%ax\n", e->attrs.cint_val);
	else if (get_type_size(e->type)==word_size)
		fprintf(fd, "\tmovl $%ld, %%eax\n", e->attrs.cint_val);
	else if (get_type_size(e->type)==pointer_size)
		fprintf(fd, "\tmovq $%ld, %%rax\n", e->attrs.cint_val);
	else 
		size_error("assign_constant", get_type_size(e->type));
}

void assign_constant_int(FILE *fd, long int e)
{
	fprintf(fd, "\tmovq $%ld, %%rax\n", e);
}

