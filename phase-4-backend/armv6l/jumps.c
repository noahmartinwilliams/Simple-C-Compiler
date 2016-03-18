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
	fprintf(fd, "\tjmp %s\n", name);
}

void jmp_eq(FILE *fd, char *name)
{
	fprintf(fd, "\tje %s\n", name);
}

void jmp_neq(FILE *fd, char *name)
{
	fprintf(fd, "\tjne %s\n", name);
}

void jmp_lt(FILE *fd, char *name)
{
	if (last_comparison_float)
		fprintf(fd, "\tjb %s\n", name);
	else
		fprintf(fd, "\tjl %s\n", name);
}

void jmp_gt(FILE *fd, char *name)
{
	if (last_comparison_float)
		fprintf(fd, "\tja %s\n", name);
	else
		fprintf(fd, "\tjg %s\n", name);
}

void jmp_le(FILE *fd, char *name)
{
	if (last_comparison_float)
		fprintf(fd, "\tjbe %s\n", name);
	else
		fprintf(fd, "\tjle %s\n", name);
}

void place_label(FILE *fd, char *name)
{
	fprintf(fd, "\t%s:\n", name);
}

void jmp_ge(FILE *fd, char *name)
{
	if (last_comparison_float)
		fprintf(fd, "\tjae %s\n", name);
	else
		fprintf(fd, "\tjge %s\n", name);
}

void compare_registers(FILE *fd, struct reg_t *a, struct reg_t *b)
{	
	last_comparison_float=(!a->is_signed) || (!b->is_signed);
	if (a->size==char_size)
		fprintf(fd, "\tcmpb %s, %s\n", reg_name(a), reg_name(b));

	else if (a->size==word_size)
		fprintf(fd, "\tcmpl %s, %s\n", reg_name(a), reg_name(b));

	else if (a->size==pointer_size)
		fprintf(fd, "\tcmpq %s, %s\n", reg_name(a), reg_name(b));
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
