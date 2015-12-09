#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator-globals.h"
#include "generator-types.h"
#include "generator.h"
#include "globals.h"
#include "types.h"

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
	fprintf(fd, "\tjl %s\n", name);
}

void jmp_gt(FILE *fd, char *name)
{
	fprintf(fd, "\tjg %s\n", name);
}

void jmp_le(FILE *fd, char *name)
{
	fprintf(fd, "\tje %s\n", name);
	fprintf(fd, "\tjl %s\n", name);
}

void place_label(FILE *fd, char *name)
{
	fprintf(fd, "\t%s:\n", name);
}

void jmp_ge(FILE *fd, char *name)
{
	fprintf(fd, "\tje %s\n", name);
	fprintf(fd, "\tjg %s\n", name);
}
