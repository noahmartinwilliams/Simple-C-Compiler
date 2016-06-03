#define IN_BACKEND
#include "generator/globals.h"
#include "types.h"
#include "generator/types.h"
#include <stdio.h>
#include <string.h>

extern int unique_num;
void load_global_string(FILE *fd, char *str)
{
	fprintf(fd, "\tldr r0, %s\n", str);
}

char* make_global_string(FILE *fd, char *str)
{
	unique_num++;
	fprintf(fd, "\t\t.section .rodata\n");
	fprintf(fd, ".LC%d:\n", unique_num);
	size_t len=strlen(str);
	str=realloc(str, len+3);
	str[len-1]='\\';
	str[len]='0';
	str[len+1]='"';
	str[len+2]='\0';
	fprintf(fd, "\t.string %s\n", str);
	unique_num++;
	fprintf(fd, "\t.text\n");
	fprintf(fd, ".L%d:\n", unique_num);
	fprintf(fd, "\t.word\t.LC%d\n", unique_num-1);
	char *ret;
	asprintf(&ret, ".L%d", unique_num);
	return ret;
}
