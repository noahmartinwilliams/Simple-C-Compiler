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
		fprintf(fd, "\tmovl %s, %%ecx\n", reg_name(dest));
		fprintf(fd, "\tshl %%cl, %s\n", reg_name(src));
		fprintf(fd, "\tmovl %s, %%eax\n", reg_name(src));
		fprintf(fd, "\tpopq %%rcx\n");
	}
}

void shift_right(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	if (src->size==word_size) {
		fprintf(fd, "\tpushq %%rcx\n");
		fprintf(fd, "\tmovl %s, %%ecx\n", reg_name(dest));
		fprintf(fd, "\tshr %%cl, %s\n", reg_name(src));
		fprintf(fd, "\tmovl %s, %%eax\n", reg_name(src));
		fprintf(fd, "\tpopq %%rcx\n");
	}
}

void or(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==word_size) {
		fprintf(fd, "\torl %s, %s\n", reg_name(a), reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(b));
	}
}

void and(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==word_size) {
		fprintf(fd, "\tandl %s, %s\n", reg_name(a), reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(b));
	}
}

void xor(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==word_size) {
		fprintf(fd, "\txorl %s, %s\n", reg_name(a), reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(b));
	}
}

void test_or(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	unique_num++;
	if (a->size==word_size) {
		/* Good GOD, this is a lot of code. -.- */
		/*TODO: figure out how to shorten this */
		fprintf(fd, "\tcmpl $0, %s\n", reg_name(a));
		fprintf(fd, "\tjne test$or$%d$true\n", unique_num);
		fprintf(fd, "\tcmpl $0, %s\n", reg_name(b));
		fprintf(fd, "\tjne test$or$%d$true\n", unique_num);
		fprintf(fd, "\tmovl $0, %s\n", reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(b));
		fprintf(fd, "\tjmp test$or$%d$false\n", unique_num);
		fprintf(fd, "\ttest$or$%d$true:\n", unique_num);
		fprintf(fd, "\tmovl $1, %s\n", reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(b));
		fprintf(fd, "\ttest$or$%d$false:\n", unique_num);
	}
}

void test_and(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	unique_num++;
	if (a->size==word_size) {
		fprintf(fd, "\tcmpl $0, %s\n", reg_name(a));
		fprintf(fd, "\tje test$and$%d$false\n", unique_num);
		fprintf(fd, "\tcmpl $0, %s\n", reg_name(b));
		fprintf(fd, "\tje test$and$%d$false\n", unique_num);

		fprintf(fd, "\tmovl $1, %s\n", reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovl $1, %%eax\n");

		fprintf(fd, "\tjmp test$and$%d$true\n", unique_num);

		fprintf(fd, "\ttest$and$%d$false:\n", unique_num);

		fprintf(fd, "\tmovl $0, %s\n", reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovl $0, %%eax\n");

		fprintf(fd, "\ttest$and$%d$true:\n", unique_num);

	}
}
