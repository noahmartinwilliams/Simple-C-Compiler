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

void place_comment(FILE *fd, char *str)
{
	fprintf(fd, "\t#%s\n", str);
}

void dereference(FILE *fd, struct reg_t *reg, size_t size)
{
	fprintf(fd, "\tmovq (%s), %%rax\n", reg_name(reg));
}

void assign_dereference(FILE *fd, struct reg_t *assign_from, struct reg_t *assign_to)
{
	if (assign_from->size==word_size)
		fprintf(fd, "\tmovl %s, (%s)\n", reg_name(assign_from), reg_name(assign_to));
	else if (assign_from->size==pointer_size)
		fprintf(fd, "\tmovq %s, (%s)\n", reg_name(assign_from), reg_name(assign_to));
}



void assign_reg(FILE *fd, struct reg_t *src, struct reg_t *dest)
{
	if (dest->size==char_size)
		fprintf(fd, "\tmovb %s, %s\n", get_reg_name(src, dest->size), reg_name(dest));

	else if (dest->size==word_size)
		fprintf(fd, "\tmovl %s, %s\n", get_reg_name(src, dest->size), reg_name(dest));

	else if (dest->size==pointer_size)
		fprintf(fd, "\tmovq %s, %s\n", get_reg_name(src, dest->size), reg_name(dest));
}

void assign_var(FILE *fd, struct reg_t *src, struct var_t *dest)
{
	if (dest==NULL) {
		fprintf(stderr, "Internal error: a NULL variable pointer was passed to assign_var\n");
		exit(1);
	}
	if (dest->scope!=0) {
		if (get_type_size(dest->type)==word_size)
			fprintf(fd, "\tmovl %s, %ld(%%rbp)\n", reg_name(src), dest->offset);
		else if (get_type_size(dest->type)==pointer_size)
			fprintf(fd, "\tmovq %s, %ld(%%rbp)\n", reg_name(src), dest->offset);
	} else {
		if (get_type_size(dest->type)==word_size)
			fprintf(fd, "\tmovl %s, %s(%%rip)\n", reg_name(src), dest->name);
	}
} 


void expand_stack_space(FILE *fd, off_t off)
{
	if (off!=0)
		fprintf(fd, "\tsubq $%ld, %%rsp\n", off);
}

void read_var(FILE *fd, struct var_t *v)
{
	if (v->scope!=0) {
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
	if (get_type_size(e->type)==char_size)
		fprintf(fd, "\tmovb $%ld, %%al\n", e->attrs.cint_val);
	else if (get_type_size(e->type)==word_size)
		fprintf(fd, "\tmovl $%ld, %%eax\n", e->attrs.cint_val);
	else {
		fprintf(stderr, "Internal Error: unknown size: %ld passed to assign_constant\n", get_type_size(e->type));
		exit(1);
	}
}

void assign_constant_int(FILE *fd, int e)
{
	fprintf(fd, "\tmovl $%d, %%eax\n", e);
}

void compare_registers(FILE *fd, struct reg_t *a, struct reg_t *b)
{
	if (a->size==char_size)
		fprintf(fd, "\tcmpb %s, %s\n", reg_name(a), reg_name(b));

	else if (a->size==word_size)
		fprintf(fd, "\tcmpl %s, %s\n", reg_name(a), reg_name(b));

	else if (a->size==pointer_size)
		fprintf(fd, "\tcmpq %s, %s\n", reg_name(a), reg_name(b));
}

void compare_register_to_int(FILE *fd, struct reg_t *a, int i)
{
	if (a->size==char_size)
		fprintf(fd, "\tcmpb $%d, %s\n", i, reg_name(a));

	else if (a->size==word_size)
		fprintf(fd, "\tcmpl $%d, %s\n", i, reg_name(a));
	
	else if (a->size==pointer_size)
		fprintf(fd, "\tcmpq $%d, %s\n", i, reg_name(a));
}

void add_readonly_data(FILE *fd, struct expr_t *e)
{
	unique_num++;
	fprintf(fd, "\t.section\t.rodata\n");
	fprintf(fd, "\t.LC%d:\n", unique_num);
	/* TODO: figure this part out */
}
