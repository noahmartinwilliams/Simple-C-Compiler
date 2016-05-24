#define IN_BACKEND
#include "generator/globals.h"
#include "types.h"
#include "generator/types.h"
#include <stdio.h>

extern int unique_num;
void load_global_string(FILE *fd, char *str)
{
	fprintf(fd, "\tmov r0, #%s\n", str);
}

char* make_global_string(FILE *fd, char *str)
{
	unique_num++;
	fprintf(fd, "\t\t.section .rodata\n");
	fprintf(fd, ".LC%d:\n", unique_num);
	fprintf(fd, "\t.string %s\n", str);
	char *ret;
	asprintf(&ret, ".LC%d", unique_num);
	return ret;
}
