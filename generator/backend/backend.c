#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator-globals.h"
#include "generator-types.h"
#include "generator.h"
#include "globals.h"
#include "types.h"
#include "registers.h"


void backend_make_global_var(FILE *fd, struct var_t *v)
{
	fprintf(fd, "\t.comm %s, %d, %d\n", v->name, get_type_size(v->type), get_type_size(v->type));

}
void dereference(FILE *fd, struct reg_t *reg, size_t size)
{
	fprintf(fd, "\tmovq (%s), %%rax\n", get_reg_name(reg, reg->size));
}

void assign_dereference(FILE *fd, struct reg_t *assign_from, struct reg_t *assign_to)
{
	if (assign_from->size==word_size)
		fprintf(fd, "\tmovl %s, (%s)\n", get_reg_name(assign_from, assign_from->size), get_reg_name(assign_to, assign_to->size));
	else if (assign_from->size==pointer_size)
		fprintf(fd, "\tmovq %s, (%s)\n", get_reg_name(assign_from, assign_from->size), get_reg_name(assign_to, assign_to->size));
}



void assign_reg(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	if (dest->size==char_size)
		fprintf(fd, "\tmovb %s, %s\n", get_reg_name(src, dest->size), get_reg_name(dest, dest->size));

	else if (dest->size==word_size)
		fprintf(fd, "\tmovl %s, %s\n", get_reg_name(src, dest->size), get_reg_name(dest, dest->size));

	else if (dest->size==pointer_size)
		fprintf(fd, "\tmovq %s, %s\n", get_reg_name(src, dest->size), get_reg_name(dest, dest->size));
}

void assign_var(FILE *fd, struct reg_t *src, struct var_t *dest)
{
	if (!dest->is_global) {
		if (get_type_size(dest->type)==word_size)
			fprintf(fd, "\tmovl %s, %ld(%%rbp)\n", get_reg_name(src, src->size), dest->offset);
		else if (get_type_size(dest->type)==pointer_size)
			fprintf(fd, "\tmovq %s, %ld(%%rbp)\n", get_reg_name(src, src->size), dest->offset);
	} else {
		if (get_type_size(dest->type)==word_size)
			fprintf(fd, "\tmovl %s, %s(%%rip)\n", get_reg_name(src, src->size), dest->name);
	}
} 


void expand_stack_space(FILE *fd, off_t off)
{
	fprintf(fd, "\tsubq $%ld, %%rsp\n", off);
}

void read_var(FILE *fd, struct var_t *v)
{
	if (!v->is_global) {
		if (get_type_size(v->type)==char_size)
			fprintf(fd, "\tmovb %ld(%%rbp), %%al\n", v->offset);
		if (get_type_size(v->type)==word_size)
			fprintf(fd, "\tmovl %ld(%%rbp), %%eax\n", v->offset);
		if (get_type_size(v->type)==pointer_size)
			fprintf(fd, "\tmovq %ld(%%rbp), %%rax\n", v->offset);
	} else {
		if (get_type_size(v->type)==word_size) {
			fprintf(fd, "\tmovl %s(%%rip), %%eax\n", v->name);
		}
	}
}

void assign_constant(FILE *fd, struct expr_t *e)
{
	if (get_type_size(e->type)==word_size)
		fprintf(fd, "\tmovl $%ld, %%eax\n", e->attrs.cint_val);
}

void assign_constant_int(FILE *fd, int e)
{
	fprintf(fd, "\tmovl $%d, %%eax\n", e);
}

void compare_registers(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==char_size)
		fprintf(fd, "\tcmpb %s, %s\n", get_reg_name(a, a->size), get_reg_name(b, b->size));
	else if (a->size==word_size)
		fprintf(fd, "\tcmpl %s, %s\n", get_reg_name(a, a->size), get_reg_name(b, b->size));
}

void compare_register_to_int(FILE *fd, struct reg_t *a, int i)
{
	if (a->size==char_size)
		fprintf(fd, "\tcmpb $%d, %s\n", i, get_reg_name(a, a->size));
	else if (a->size==word_size)
		fprintf(fd, "\tcmpl $%d, %s\n", i, get_reg_name(a, a->size));
}


void start_call(FILE *fd, struct func_t *f)
{
	int x;
	for (x=0; x<num_regs; x++) {
		int y;
		size_t biggest_size=0;
		for (y=0; y<regs[x]->num_sizes; y++)
			if (regs[x]->sizes[y].size>biggest_size)
				biggest_size=regs[x]->sizes[y].size;
		if (regs[x]->in_use)
			fprintf(fd, "\tpush %s\n", get_reg_name(regs[x], biggest_size));
	}
	if (f->num_arguments==0 && f->ret_type->body->is_struct==false) {
		return;
	}
}

void add_argument(FILE *fd, struct reg_t *reg, struct type_t *t )
{
	/*TODO: Fix this to work better. */	
	if (!strcmp(t->name, "int"))
		if (reg->size==word_size)
			fprintf(fd, "\tmovl %s, %%edi\n", get_reg_name(reg, word_size));
}

void call(FILE *fd, struct func_t *f)
{
	fprintf(fd, "\tcall %s\n", f->name);
}
