#define IN_BACKEND
#include "types.h"
#include "generator/types.h"
#include <stdio.h>
#include "generator/globals.h"

void shift_left(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	fprintf(fd, "\tmov\t%s, %s, asl %s\n", reg_name(src), reg_name(src), reg_name(dest));
	fprintf(fd, "\tmov\tr0, %s\n", reg_name(src));
}

void shift_right(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	fprintf(fd, "\tmov\t%s, %s, asr %s\n", reg_name(src), reg_name(src), reg_name(dest));
	fprintf(fd, "\tmov\tr0, %s\n", reg_name(src));
}

void or(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	fprintf(fd, "\torr %s, %s, %s\n", reg_name(a), reg_name(a), reg_name(b));
	fprintf(fd, "\tmov r0, %s\n", reg_name(a));
}

void and(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	fprintf(fd, "\tand %s, %s, %s\n", reg_name(b), reg_name(b), reg_name(a));
	fprintf(fd, "\tmov r0, %s\n", reg_name(b));
}

void xor(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	fprintf(fd, "\teor %s, %s, %s\n", reg_name(b), reg_name(b), reg_name(a));
	fprintf(fd, "\tmov r0, %s\n", reg_name(b));
}

void test_or(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	unique_num++;
	char *yes, *skip;
	asprintf(&skip, "skip$%d", unique_num);
	asprintf(&yes, "yes$%d", unique_num);

	fprintf(fd, "\tcmp %s, #0\n", reg_name(a));
	jmp_neq(fd, yes);
	fprintf(fd, "\tcmp %s, #0\n", reg_name(b));
	jmp_neq(fd, yes);
	fprintf(fd, "\tmov r0, #0\n");
	jmp(fd, skip);
	place_label(fd, yes);
	fprintf(fd, "\tmov r0, #1\n");
	place_label(fd, skip);

	free(yes);
	free(skip);
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

	} else if (a->size==pointer_size) {
		fprintf(fd, "\tcmpq $0, %s\n", reg_name(a));
		fprintf(fd, "\tje test$and$%d$false\n", unique_num);
		fprintf(fd, "\tcmpq $0, %s\n", reg_name(b));
		fprintf(fd, "\tje test$and$%d$false\n", unique_num);

		fprintf(fd, "\tmovq $1, %s\n", reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovq $1, %%eax\n");

		fprintf(fd, "\tjmp test$and$%d$true\n", unique_num);

		fprintf(fd, "\ttest$and$%d$false:\n", unique_num);

		fprintf(fd, "\tmovq $0, %s\n", reg_name(b));
		if (b->use!=RET)
			fprintf(fd, "\tmovq $0, %%eax\n");

		fprintf(fd, "\ttest$and$%d$true:\n", unique_num);

	}
}

void test_invert(FILE *fd, struct reg_t *r)
{
	unique_num++;
	if (r->size==word_size) {
		fprintf(fd, "\tcmpl $0, %s\n", reg_name(r));
		fprintf(fd, "\tje invert$%d$false\n", unique_num);


		fprintf(fd, "\tmovl $0, %s\n", reg_name(r));
		if (r->use!=RET)
			fprintf(fd, "\tmovl $0, %%eax\n");

		fprintf(fd, "\tjmp invert$%d$true\n", unique_num);
		fprintf(fd, "\tinvert$%d$false:\n", unique_num);
		fprintf(fd, "\tmovl $1, %s\n", reg_name(r));
		if (r->use!=RET)
			fprintf(fd, "\tmovl $1, %%eax\n");
		fprintf(fd, "\tinvert$%d$true:\n", unique_num);
	}
}

void invert(FILE *fd, struct reg_t *r)
{
	if (r->size==word_size) {
		fprintf(fd, "\tnotl %s\n", reg_name(r));
		if (r->use!=RET)
			fprintf(fd, "\tmovl %s, %%eax\n", reg_name(r));
	} else if (r->size==long_size) {
		fprintf(fd, "\tnotq %s\n", reg_name(r));
		if (r->use!=RET)
			fprintf(fd, "\tmovq %s, %%rax\n", reg_name(r));
	} else {
		fprintf(stderr, "Internal Error: invalid register size: %d passed to invert\n", r->size);
		exit(1);
	}
}
