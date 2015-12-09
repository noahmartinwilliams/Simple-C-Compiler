#include "types.h"
#include "generator-types.h"
#include <stdio.h>
#include "generator-globals.h"

void shift_left(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	if (src->size==word_size) {
		/* I hate Intel. -.- Apparently the shift instruction
		can only either shift by one (which requires no argument)
		or it can shift by the contents of cl. 

		Why the hell do these things have these completely arbitrary
		idiotic rules? */

		fprintf(fd, "\tpushq %%rcx\n");
		fprintf(fd, "\tmovl %s, %%ecx\n", get_reg_name(dest, dest->size));
		fprintf(fd, "\tshl %%cl, %s\n", get_reg_name(src, src->size));
		fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(src, src->size));
		fprintf(fd, "\tpopq %%rcx\n");
	}
}
