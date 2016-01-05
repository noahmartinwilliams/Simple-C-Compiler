#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/generator-globals.h"
#include "generator/generator-types.h"
#include "generator/generator.h"
#include "generator/backend/registers.h"
#include "globals.h"
#include "types.h"

void backend_make_global_var(FILE *fd, struct var_t *v)
{
	fprintf(fd, "\t.comm %s, %d, %d\n", v->name, get_type_size(v->type), get_type_size(v->type));

}

static void inc_by_int(FILE *fd, int i, char *dest, size_t size)
{
	if (size==char_size)
		fprintf(fd, "\taddb $%d, %s\n", i, dest);
	else if (size==word_size)
		fprintf(fd, "\taddl $%d, %s\n", i, dest);
	else if (size==pointer_size)
		fprintf(fd, "\taddq $%d, %s\n", i, dest);
}

void get_address(FILE *fd, struct expr_t *_var)
{
	place_comment(fd, "& (");
	depth++;
	struct reg_t *ret=get_ret_register(_var->type->body->size);
	if (_var->kind==var) {
		fprintf(fd, "\tmovq %%rbp, %s\n", get_reg_name(ret, pointer_size));
		inc_by_int(fd, _var->attrs.var->offset, get_reg_name(ret, pointer_size), pointer_size);
	}
	depth--;
	place_comment(fd, "(");
}

void assign_var(FILE *fd, struct reg_t *src, struct var_t *dest)
{
	if (dest==NULL) {
		fprintf(stderr, "Internal error: a NULL variable pointer was passed to assign_var\n");
		exit(1);
	}
	if (dest->scope_depth!=0) {
		if (get_type_size(dest->type)==word_size)
			fprintf(fd, "\tmovl %s, %ld(%%rbp)\n", reg_name(src), dest->offset);
		else if (get_type_size(dest->type)==pointer_size)
			fprintf(fd, "\tmovq %s, %ld(%%rbp)\n", reg_name(src), dest->offset);
	} else {
		if (get_type_size(dest->type)==word_size)
			fprintf(fd, "\tmovl %s, %s(%%rip)\n", reg_name(src), dest->name);
	}
} 

void read_var(FILE *fd, struct var_t *v)
{
	if (v->scope_depth!=0) {
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