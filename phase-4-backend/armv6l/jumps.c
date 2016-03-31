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
	fprintf(fd, "\tbeq %s\n", name);
}

void jmp_neq(FILE *fd, char *name)
{
	fprintf(fd, "\tbne %s\n", name);
}

void jmp_lt(FILE *fd, char *name)
{
	fprintf(fd, "\tblt %s\n", name);
}

void jmp_gt(FILE *fd, char *name)
{
	fprintf(fd, "\tbgt %s\n", name);
}

void jmp_le(FILE *fd, char *name)
{
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
	//last_comparison_float=(!a->is_signed) || (!b->is_signed);
	fprintf(fd, "\tcmp %s, %s\n", reg_name(b), reg_name(a));
}

void compare_register_to_int(FILE *fd, struct reg_t *a, int i)
{
	last_comparison_float=(!a->is_signed);
	fprintf(fd, "\tcmp %s, #%d\n", reg_name(a), i);
}
