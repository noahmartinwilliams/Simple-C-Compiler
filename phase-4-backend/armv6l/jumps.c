#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/globals.h"
#include "generator/types.h"
#include "generator/generator.h"
#include "globals.h"
#include "types.h"

extern bool last_comparison_float;
void jmp(FILE *fd, char *name)
{
	fprintf(fd, "\tb %s\n", name);
}

void jmp_eq(FILE *fd, char *name)
{
	fprintf(fd, "\tbe %s\n", name);
}

void jmp_neq(FILE *fd, char *name)
{
	fprintf(fd, "\tbne %s\n", name);
}

void jmp_lt(FILE *fd, char *name)
{
	if (last_comparison_float)
		fprintf(fd, "\tbb %s\n", name);
	else
		fprintf(fd, "\tbl %s\n", name);
}

void jmp_gt(FILE *fd, char *name)
{
	if (last_comparison_float)
		fprintf(fd, "\tba %s\n", name);
	else
		fprintf(fd, "\tbg %s\n", name);
}

void jmp_le(FILE *fd, char *name)
{
	if (last_comparison_float)
		fprintf(fd, "\tbe %s\n", name);
	else
		fprintf(fd, "\tble %s\n", name);
}

void place_label(FILE *fd, char *name)
{
	fprintf(fd, "\t%s:\n", name);
}

void jmp_ge(FILE *fd, char *name)
{
		fprintf(fd, "\tbge %s\n", name);
}

void compare_registers(FILE *fd, struct reg_t *a, struct reg_t *b)
{	
	last_comparison_float=(!a->is_signed) || (!b->is_signed);
	fprintf(fd, "\tcmp %s, %s\n", reg_name(b), reg_name(a));
}

void compare_register_to_int(FILE *fd, struct reg_t *a, int i)
{
	last_comparison_float=(!a->is_signed);
	if (a->size==char_size)
		fprintf(fd, "\tcmpb $%d, %s\n", i, reg_name(a));

	else if (a->size==word_size)
		fprintf(fd, "\tcmpl $%d, %s\n", i, reg_name(a));
	
	else if (a->size==pointer_size)
		fprintf(fd, "\tcmpq $%d, %s\n", i, reg_name(a));
}
