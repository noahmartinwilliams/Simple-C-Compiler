#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator-globals.h"
#include "globals.h"
#include "backend.h"
#include "generator-types.h"
#include "handle-funcs.h"

/* TODO: make all of this more efficient */

char* prepare_var_assignment(FILE *fd, struct expr_t *dest);
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
	struct reg_t *rhs=get_free_register(fd, e->type->body->size);
	if (!strcmp(e->attrs.bin_op, "+")) {
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);
		fprintf(fd, "\t#)\n\t#+\n\t#(\n");
		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);
		fprintf(fd, "\t#)\n");
		int_add(fd, lhs, rhs);
		fprintf(fd, "\t#)\n");
	} else if (!strcmp(e->attrs.bin_op, "=")) {
		fprintf(fd, "\t#Note: lhs, and rhs of assignment is swapped\n");
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->right);
		if (e->left->kind!=var)
			assign_reg(fd, ret, lhs);
		char *tmp=prepare_var_assignment(fd, e->left);
		fprintf(fd, "\t#)\n\t#=\n\t#(\n");
		/* TODO: figure out a good way to abstract away the direct use
		 * of the mov command here. Printint opcodes is for handle-registers.c */
		if (e->left->kind!=var)
			fprintf(fd, "\tmovl %s, %s\n", get_reg_name(lhs), tmp);
		else 
			fprintf(fd, "\tmovl %s, %s\n", get_reg_name(ret), tmp);
		fprintf(fd, "\t#)\n\t#)\n");
		free(tmp);
	} else if (!strcmp(e->attrs.bin_op, "-")) {
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);
		fprintf(fd, "\t#)\n\t#-\n\t#(\n");
		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);
		fprintf(fd, "\t#)\n");
		int_sub(fd, lhs, rhs);
		fprintf(fd, "\t#)\n");
	} else if (!strcmp(e->attrs.bin_op, "/")) {
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);
		fprintf(fd, "\t#)\n\t#/\n\t#(\n");
		generate_expression(fd, e->right);
		assign_reg(fd, ret, rhs);
		fprintf(fd, "\t#)\n");
		int_div(fd, lhs, rhs);
		fprintf(fd, "\t#)\n");

	} else if (!strcmp(e->attrs.bin_op, "*")) {
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);
		fprintf(fd, "\t#)\n\t#*\n\t#(\n");
		generate_expression(fd, e->right);
		fprintf(fd, "\t#)\n");
		int_mul(fd, ret, lhs);
		fprintf(fd, "\t#)\n");

	} else if (!strcmp(e->attrs.bin_op, "==")) {
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->left);
		assign_reg(fd, ret, lhs);
		fprintf(fd, "\t#)\n\t#==\n\t#(\n");
		generate_expression(fd, e->right);
		fprintf(fd, "\t#)\n");
		compare_registers(fd, ret, lhs);

		unique_num++;
		char *are_eq, *are_not_eq;
		asprintf(&are_eq, "is$equal$%d", unique_num);
		asprintf(&are_not_eq, "is$not$equal$%d", unique_num);

		jmp_eq(fd, are_eq);
		assign_constant_int(fd, 0);
		jmp(fd, are_not_eq);
		place_label(fd, are_eq);
		assign_constant_int(fd, 1);
		place_label(fd, are_not_eq);

		free(are_eq);
		free(are_not_eq);
	}
	free_register(fd, rhs);
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
	} else if (s->kind==_if) {
		fprintf(fd, "\t#if (\n");
		generate_expression(fd, s->attrs._if.condition);
		struct reg_t *ret=get_ret_register(word_size);
		compare_register_to_int(fd, ret, 0);

		unique_num++;
		char *unique_name_else, *unique_name_true;
		asprintf(&unique_name_else, "if$not$true$%d", unique_num);
		asprintf(&unique_name_true, "if$true$%d", unique_num);
		fprintf(fd, "\t#)\n");

		jmp_eq(fd, unique_name_else);
		fprintf(fd, "\t#{\n");
		generate_statement(fd, s->attrs._if.block);
		jmp(fd, unique_name_true);
		if (s->attrs._if.else_block==NULL) {
			fprintf(fd, "\t#}\n");
			place_label(fd, unique_name_else);
		} else {
			place_label(fd, unique_name_else);
			fprintf(fd, "\t#} else {\n");
			generate_statement(fd, s->attrs._if.else_block);
		}
		place_label(fd, unique_name_true);
		free(unique_name_else);
		free(unique_name_true);
	}
}

off_t get_var_offset(struct statem_t *s, off_t current_off)
{
	off_t o=0;
	if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++)
			o+=get_var_offset(s->attrs.list.statements[x], current_off+o);
	} else if (s->kind==declare) {
		o=s->attrs.var->type->body->size;
		s->attrs.var->offset=-(o+current_off);
	} else if (s->kind==_if) {
		o+=get_var_offset(s->attrs._if.block, current_off+o);
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

char* prepare_var_assignment(FILE *fd, struct expr_t *dest)
{
	char *ret=NULL;
	if (dest->kind==var) {
		if (dest->attrs.var->scope!=0) {
			asprintf(&ret, "%ld(%%rbp)", dest->attrs.var->offset);
		}
	}
	return ret;
}
