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

static inline void size_error(char *message, size_t size)
{
	fprintf(stderr, "Internal Error: unknown size: %ld passed to %s\n", size, message);
	exit(1);
}

void compare_float_registers(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==word_size && b->size==word_size) {
		fprintf(fd, "\tsubq $8, %%rsp\n");
		fprintf(fd, "\tmovl %s, -%d(%%rbp)\n", reg_name(a), current_stack_offset+8);
		fprintf(fd, "\tmovl %s, -%d(%%rbp)\n", reg_name(b), current_stack_offset+4);
		fprintf(fd, "\tcvtss2sd -%d(%%rbp), %%xmm0\n", current_stack_offset+8);
		fprintf(fd, "\tcvtss2sd -%d(%%rbp), %%xmm1\n", current_stack_offset+4);

		fprintf(fd, "\tucomisd %%xmm0, %%xmm1\n");
		fprintf(fd, "\taddq $8, %%rsp\n");
		unique_num++;
		fprintf(fd, "\tja Float$Intern$lt$%d\n", unique_num);
		fprintf(fd, "\tjmp Float$Intern$End$%d\n", unique_num);
		fprintf(fd, "\tFloat$Intern$lt$%d:\n", unique_num);
		fprintf(fd, "\tmovl $1, %%eax\n");
		fprintf(fd, "\tcmpl $2, %%eax\n");
		fprintf(fd, "\tjmp Float$Intern$End$%d\n", unique_num);
		fprintf(fd, "\tFloat$Intern$End$%d:\n", unique_num);
		/*TODO: Fix this to be simpler. */
	} else
		size_error("compare_float_registers", a->size);
}

void assign_constant_float(FILE *fd, struct expr_t *e)
{
	if (get_type_size(e->type)==word_size) {
		fprintf(fd, "\tmovl $%d, %%eax\n", (float) e->attrs.cfloat_val);
	} else 
		size_error("assign_constant_float", get_type_size(e->type));
}

void float_sub(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	if (src->size==word_size && dest->size==word_size) {
		fprintf(fd, "\tsubq $8, %%rsp\n");
		fprintf(fd, "\tmovl %s, -%d(%%rbp)\n", reg_name(src), current_stack_offset+4);
		fprintf(fd, "\tmovl %s, -%d(%%rbp)\n", reg_name(dest), current_stack_offset+8);
		fprintf(fd, "\tcvtss2sd -%d(%%rbp), %%xmm0\n", current_stack_offset+4);
		fprintf(fd, "\tcvtss2sd -%d(%%rbp), %%xmm1\n", current_stack_offset+8);
		fprintf(fd, "\tsubsd %%xmm0, %%xmm1\n");


		fprintf(fd, "\taddq $8, %%rsp\n");
		fprintf(fd, "\tmovq %%xmm1, %%rax\n");
		if (dest->use!=RET)
			fprintf(fd, "\tmovl %%xmm1, %s\n", reg_name(dest));
	} else
		size_error("float_sub", src->size);
}
