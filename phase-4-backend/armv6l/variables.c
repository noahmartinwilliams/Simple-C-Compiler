#define IN_BACKEND
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator/types.h"
#include "generator/generator.h"
#include "parser/types.h"
#include "backend/registers.h"
#include "globals.h"
#include "backend/registers.h"
#include "types.h"

static inline void error(char *name, size_t size)
{
	fprintf(stderr, "Internal Error: size %ld passed to function %s, no handler found.\n", size, name);
}

void backend_make_global_var(FILE *fd, struct var_t *v)
{
	fprintf(fd, "\t.comm %s, %d, %d\n", v->name, get_type_size(v->type), get_type_size(v->type));

}

static void inc_by_int(FILE *fd, int i, char *dest, size_t size)
{
	fprintf(fd, "\tadd %s, %s, #%d\n", dest, dest, i);
}

void get_address(FILE *fd, struct expr_t *_var)
{
	struct reg_t *ret=get_ret_register(pointer_size, false);
	if (_var->kind==var) {
		fprintf(fd, "\tmov %s, fp\n", get_reg_name(ret, pointer_size));
		inc_by_int(fd, _var->attrs.var->offset, get_reg_name(ret, pointer_size), pointer_size);
	}
}

static inline void print_assign_var(FILE *fd, char *operator, struct reg_t *reg, struct var_t *var)
{
	fprintf(fd, "\t%s %s, var$%s$%d(%%rbp)\n", operator, reg_name(reg), var->name, var->scope_depth);
}

void assign_var(FILE *fd, struct reg_t *src, struct var_t *dest)
{
	if (dest->is_register) {
		fprintf(fd, "\tmov %s, %s\n", reg_name(src), reg_name(dest->reg));
		return;
	}
			
	if (src->use==FLOAT || src->use==FLOAT_RET) {
		char *name=reg_name(src);
		if (dest->scope_depth!=0) {
			fprintf(fd, "\tcvtsd2ss %s, %s\n", name, name);
			fprintf(fd, "\tmovss %s, %d(%%rbp)\n", name, dest->offset);
		} else {
			fprintf(fd, "cvtsd2ss %s, %s\n", name, name);
			fprintf(fd, "\tmovss %s, %s(%%rip)\n", name, dest->name);
		}
		return;
	}

	if (dest==NULL) {
		fprintf(stderr, "Internal error: a NULL variable pointer was passed to assign_var\n");
		exit(1);
	}

	size_t size=get_type_size(dest->type);
	if (dest->scope_depth!=0) {
		fprintf(fd, "\tstr\tr0, [fp, #%zd]\n", dest->offset);
	} else 
		if (size==word_size)
			fprintf(fd, "\tmovl %s, %s(%%rip)\n", reg_name(src), dest->name);
		else
			error("assign_var", size);
} 

static inline void print_read_var(FILE *fd, char *operator, char *reg, struct var_t *var)
{
	fprintf(fd, "\t%s var$%s$%d(%%rbp), %s\n", operator, var->name, var->scope_depth, reg);
}

void read_var(FILE *fd, struct var_t *v)
{
	get_ret_register(word_size, false)->is_signed=is_signed(v->type);
	if (v->is_register) {
			fprintf(fd, "\tmov %s, %%r0\n", reg_name(v->reg));
	}
	if (v->scope_depth!=0) {
		size_t size=get_type_size(v->type);
		fprintf(fd, "\tldr\tr0, [fp, #%zd]\n", v->offset);
	} else if (get_type_size(v->type)==word_size)
			fprintf(fd, "\tmov r0, [ sp, #%zd ]\n", v->offset);
}

void dereference(FILE *fd, struct reg_t *reg, size_t size)
{
	fprintf(fd, "\tldr r0, [%s]\n", reg_name(reg));
}

void assign_dereference(FILE *fd, struct reg_t *assign_from, struct reg_t *assign_to)
{

	if (assign_from->size==char_size)
		fprintf(fd, "\tmovb %s, (%s)\n", reg_name(assign_from), reg_name(assign_to));
	else if (assign_from->size==word_size)
		fprintf(fd, "\tmovl %s, (%s)\n", reg_name(assign_from), reg_name(assign_to));
	else if (assign_from->size==pointer_size)
		fprintf(fd, "\tmovq %s, (%s)\n", reg_name(assign_from), reg_name(assign_to));
	else
		error("assign_dereference", assign_from->size);
}
