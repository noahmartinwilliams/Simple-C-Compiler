#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/generator-globals.h"
#include "generator/generator-types.h"
#include "generator/generator.h"
#include "globals.h"
#include "types.h"
#include "backend/backend.h"
#include "backend/registers.h"
#include "handle-types.h"

extern bool last_comparison_float;
static inline void size_error(char *message, size_t size)
{
	fprintf(stderr, "Internal Error: unknown size: %ld passed to %s\n", size, message);
	exit(1);
}

void compare_float_registers(FILE *fd, struct reg_t *b, struct reg_t *a)
{
	last_comparison_float=true;
	if (a->size==word_size && b->size==word_size) {
		fprintf(fd, "\tsubq $8, %%rsp\n");
		fprintf(fd, "\tmovl %s, -%d(%%rbp)\n", reg_name(a), current_stack_offset+4);
		fprintf(fd, "\tmovl %s, -%d(%%rbp)\n", reg_name(b), current_stack_offset+8);
		fprintf(fd, "\tflds -%d(%%rbp)\n", current_stack_offset+8);
		fprintf(fd, "\tflds -%d(%%rbp)\n", current_stack_offset+4);
		fprintf(fd, "\tfcomip\n");
		fprintf(fd, "\tfstp %%st(0)\n");
	} else
		size_error("compare_float_registers", a->size);
}

void assign_constant_float(FILE *fd, struct expr_t *e)
{
	if (get_type_size(e->type)==word_size) {
		fprintf(fd, "\tmovl %s(%%rip), %%eax\n", e->attrs.cfloat);
	} else 
		size_error("assign_constant_float", get_type_size(e->type));
}

static void arithmetic_intern(FILE *fd, char *name, struct reg_t *src, struct reg_t *dest)
{
	if (src->size==word_size && dest->size==word_size) {
		fprintf(fd, "\tsubq $8, %%rsp\n");
		fprintf(fd, "\tmovl %s, -%d(%%rbp)\n", reg_name(src), current_stack_offset+4);
		fprintf(fd, "\tmovl %s, -%d(%%rbp)\n", reg_name(dest), current_stack_offset+8);
		fprintf(fd, "\tmovss -%d(%%rbp), %%xmm0\n", current_stack_offset+4);
		fprintf(fd, "\tmovss -%d(%%rbp), %%xmm1\n", current_stack_offset+8);
		fprintf(fd, "\t%s %%xmm0, %%xmm1\n", name);
		fprintf(fd, "\tmovq %%xmm1, %%rax\n");
		if (dest->use!=RET)
			fprintf(fd, "\tmovl %%xmm1, %s\n", reg_name(dest));
		fprintf(fd, "\taddq $8, %%rsp\n");
	} else
		size_error(name, src->size);
}

void float_add(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	arithmetic_intern(fd, "addss", src, dest);
}

void float_sub(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	arithmetic_intern(fd, "subsd", src, dest);
}

char* generate_global_float(FILE *fd, char *num)
{
	fprintf(fd, ".section .rodata\n");
	unique_num++;
	fprintf(fd, "\t.LC%d:\n", unique_num);
	float f=atof(num);
	fprintf(fd, "\t.float %e\n", f);
	char *n=NULL;
	asprintf(&n, ".LC%d", unique_num);
	return n;
}
