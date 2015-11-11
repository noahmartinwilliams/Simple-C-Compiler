#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator-globals.h"
#include "globals.h"
#include "handle-registers.h"
#include "generator-types.h"
#include "handle-funcs.h"


char* assign(FILE *fd, struct expr_t *e);
void setup_types()
{
	num_types++;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=malloc(sizeof(struct type_t));
	struct type_t *i=types[num_types-1];
	i->name=strdup("int");
	i->body=malloc(sizeof(struct tbody_t));
	i->body->size=int_size;
}

void setup_generator()
{
	setup_types();
	setup_registers();
}

void generate_binary_expression(FILE *fd, struct expr_t *e);

void generate_expression(FILE *fd, struct expr_t *e)
{
	if (e->kind==bin_op) {
		generate_binary_expression(fd, e);
	} else if (e->kind==var) {
		read_var(fd, e->attrs.var);
	} else if (e->kind==const_int) {
		assign_constant(fd, e);
	}
}

void generate_binary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	struct reg_t *ret=get_ret_register(e->type->body->size);
	struct reg_t *lhs=get_free_register(fd, e->type->body->size);
	if (!strcmp(e->attrs.bin_op, "+")) {
		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);
		generate_expression(fd, e->right);
		add(fd, lhs, ret);
	} else if (!strcmp(e->attrs.bin_op, "=")) {
		generate_expression(fd, e->right);
		assign_reg(fd, ret, lhs);
		char *tmp=assign(fd, e->left);
		/* TODO: figure out a good way to abstract away the direct use
		 * of the mov command here. Printint opcodes is for handle-registers.c */
		fprintf(fd, "\tmovl %s, %s\n", lhs->name, tmp);
		free(tmp);
	} else if (!strcmp(e->attrs.bin_op, "-")) {
		generate_expression(fd, e->right);
		assign_reg(fd, ret, lhs);
		generate_expression(fd, e->left);
		sub(fd, lhs, ret);
	}
	free_register(fd, lhs);
	depth--;
}

void generate_statement(FILE *fd, struct statem_t *s)
{
	if (s->kind==expr) {
		generate_expression(fd, s->attrs.expr);
	} else if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++) {
			generate_statement(fd, s->attrs.list.statements[x]);
		}
	} else if (s->kind==ret) {
		generate_expression(fd, s->attrs.expr);
		if (!in_main)
			fprintf(fd, "\tret\n");
		else
			fprintf(fd, "\tmovq %%rax, %%rdi\n\tmovq $60, %%rax\n\tsyscall\n");
	} else if (s->kind==declare) {
		return;
	}
}

off_t get_var_offset(struct statem_t *s, off_t o)
{
	if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++)
			o+=get_var_offset(s->attrs.list.statements[x], o);
	} else if (s->kind==declare) {
		o+=s->attrs.var->type->body->size;
		s->attrs.var->offset=-o;
	} else {
		o=0;
	}

	return o;
}

void generate_function(FILE *fd, struct func_t *f)
{
	if (!strcmp(f->name, "main")) {
		in_main=true;
		fprintf(fd, ".global _start\n.type _start, @function\n_start:\n");
	} else {
		fprintf(fd, ".globl %s\n.type %s, @function\n%s:\n", f->name, f->name, f->name);
	}
	fprintf(fd, "\tmovq %%rsp, %%rbp\n");
	off_t o=get_var_offset(f->statement_list, 0);
	expand_stack_space(fd, o);

	generate_statement(fd, f->statement_list);
	if (strcmp(f->name, "main")) {
		fprintf(fd, "\tmovq $0, %%rax\n\tret\n");
	} else {
		fprintf(fd, "\tmovq %%rax, %%rdi\n\tmovq $60, %%rax\n\tsyscall\n");
	}
}

char* assign(FILE *fd, struct expr_t *dest)
{
	char *ret=NULL;
	if (dest->kind==var) {
		if (dest->attrs.var->scope!=0) {
			asprintf(&ret, "%ld(%%rbp)", dest->attrs.var->offset);
		}
	}
	return ret;
}
