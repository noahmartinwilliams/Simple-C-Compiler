#include "types.h"
#include <stdlib.h>
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include "handle-registers.h"
#include "generator-types.h"
#include "handle-funcs.h"


bool in_main=false;
size_t word_size=4;
size_t int_size=4;

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
		struct reg_t *ret=get_ret_register(e->type->body->size);
		read_var(fd, e->attrs.var);
	} else if (e->kind==const_int) {
		struct reg_t *ret=get_ret_register(e->type->body->size);
		assign_constant(fd, e);
	}
}

void generate_binary_expression(FILE *fd, struct expr_t *e)
{
	if (!strcmp(e->attrs.bin_op, "+")) {
		struct reg_t *ret=get_ret_register(e->type->body->size);
		struct reg_t *lhs=get_free_register(e->type->body->size);
		generate_expression(fd, e->left);
		assign(fd, ret, lhs);
		generate_expression(fd, e->right);
		add(fd, lhs, ret);
		lhs->in_use=false;
	}
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
	}
}

void generate_function(FILE *fd, struct func_t *f)
{
	if (!strcmp(f->name, "main")) {
		in_main=true;
		fprintf(fd, ".global _start\n.type _start, @function\n_start:\n");
	} else {
		fprintf(fd, ".globl %s\n.type %s, @function\n%s:\n", f->name, f->name, f->name);
	}
	/* TODO: test this more */
	generate_statement(fd, f->statement_list);
	if (strcmp(f->name, "main")) {
		fprintf(fd, "\tmovq $0, %%rax\n\tret\n");
	} else {
		fprintf(fd, "\tmovq %%rax, %%rdi\n\tmovq $60, %%rax\n\tsyscall\n");
	}
}
