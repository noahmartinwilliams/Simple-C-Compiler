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

void shift_right(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	if (src->size==word_size) {
		fprintf(fd, "\tpushq %%rcx\n");
		fprintf(fd, "\tmovl %s, %%ecx\n", get_reg_name(dest, dest->size));
		fprintf(fd, "\tshr %%cl, %s\n", get_reg_name(src, src->size));
		fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(src, src->size));
		fprintf(fd, "\tpopq %%rcx\n");
	}
}

void or(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==word_size) {
		fprintf(fd, "\torl %s, %s\n", get_reg_name(a, a->size), get_reg_name(b, b->size));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(b, b->size));
	}
}

void and(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==word_size) {
		fprintf(fd, "\tandl %s, %s\n", get_reg_name(a, a->size), get_reg_name(b, b->size));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(b, b->size));
	}
}

void xor(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==word_size) {
		fprintf(fd, "\txorl %s, %s\n", get_reg_name(a, a->size), get_reg_name(b, b->size));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(b, b->size));
	}
}

void test_or(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	unique_num++;
	if (a->size==word_size) {
		/* Good GOD, this is a lot of code. -.- */
		/*TODO: figure out how to shorten this */
		fprintf(fd, "\tcmpl $0, %s\n", get_reg_name(a, a->size));
		fprintf(fd, "\tjne test$or$%d$true\n", unique_num);
		fprintf(fd, "\tcmpl $0, %s\n", get_reg_name(b, b->size));
		fprintf(fd, "\tjne test$or$%d$true\n", unique_num);
		fprintf(fd, "\tmovl $0, %s\n", get_reg_name(b, b->size));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(b, b->size));
		fprintf(fd, "\tjmp test$or$%d$false\n", unique_num);
		fprintf(fd, "\ttest$or$%d$true:\n", unique_num);
		fprintf(fd, "\tmovl $1, %s\n", get_reg_name(b, b->size));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", get_reg_name(b, b->size));
		fprintf(fd, "\ttest$or$%d$false:\n", unique_num);
	}
}
