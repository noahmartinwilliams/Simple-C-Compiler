#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator-globals.h"
#include "globals.h"
#include "backend.h"
#include "generator-types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "stack.h"

char* prepare_var_assignment(FILE *fd, struct expr_t *dest);

void get_address(FILE *fd, struct expr_t *_var)
{
	fprintf(fd, "\t#&\n\t#(\n");
	depth++;
	struct reg_t *ret=get_ret_register(_var->type->body->size);
	if (_var->kind==var) {
		fprintf(fd, "\tmovq %%rbp, %s\n", get_reg_name(ret, pointer_size));
		inc_by_int(fd, _var->attrs.var->offset, get_reg_name(ret, pointer_size), pointer_size);
	}
	depth--;
	fprintf(fd, "\t#)\n");
}
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
void generate_pre_unary_expression(FILE *fd, struct expr_t *e);

void generate_expression(FILE *fd, struct expr_t *e)
{
	if (e->kind==bin_op) {
		generate_binary_expression(fd, e);
	} else if (e->kind==var) {
		read_var(fd, e->attrs.var);
	} else if (e->kind==const_int) {
		assign_constant(fd, e);
	} else if (e->kind==pre_un_op) {
		generate_pre_unary_expression(fd, e);
	}
}

void generate_pre_unary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	struct reg_t *ret=get_ret_register(pointer_size);
	if (!strcmp(e->attrs.un_op, "&")) {
		get_address(fd, e->right);
	} else if (!strcmp(e->attrs.un_op, "*")) {
		fprintf(fd, "\t#(\n\t#*\n\t#(\n");
		generate_expression(fd, e->right);
		fprintf(fd, "\t#)\n");
		dereference(fd, ret, pointer_size);
		fprintf(fd, "\t#)\n");
	}
	depth--;
}

static void generate_comparison_expression(FILE *fd, struct expr_t *e, void (*comparitor)(FILE*, char*), char *true_string, char *false_string, struct reg_t *lhs)
{
	struct reg_t *ret=get_ret_register(e->type->body->size);

	fprintf(fd, "\t#(\n\t#(\n");
	generate_expression(fd, e->left);
	assign_reg(fd, ret, lhs);

	fprintf(fd, "\t#)\n\t#%s\n\t#(\n", e->attrs.bin_op);

	generate_expression(fd, e->right);
	fprintf(fd, "\t#)\n");
	compare_registers(fd, ret, lhs);

	unique_num++;
	char *is_true, *is_false;
	asprintf(&is_true, true_string, unique_num);
	asprintf(&is_false, false_string, unique_num);

	comparitor(fd, is_true);
	assign_constant_int(fd, 0);
	jmp(fd, is_false);

	place_label(fd, is_true);
	assign_constant_int(fd, 1);
	place_label(fd, is_false);

	fprintf(fd, "\t#)\n");

	free(is_true);
	free(is_false);

}
void generate_binary_expression(FILE *fd, struct expr_t *e)
{
	depth++;
	struct reg_t *ret=get_ret_register(get_type_size(e->type));
	struct reg_t *lhs=get_free_register(fd, get_type_size(e->type));
	struct reg_t *rhs=get_free_register(fd, get_type_size(e->type));
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
		 * of the mov command here. Printing opcodes is for handle-registers.c */
		if (e->left->kind!=var) {
			if (lhs->size==word_size)
				fprintf(fd, "\tmovl %s, %s\n", get_reg_name(lhs, word_size), tmp);
			else if (lhs->size==pointer_size)
				fprintf(fd, "\tmovq %s, %s\n", get_reg_name(lhs, pointer_size), tmp);
		} else {
			if (lhs->size==word_size)
				fprintf(fd, "\tmovl %s, %s\n", get_reg_name(ret, word_size), tmp);
			else if (lhs->size==pointer_size)
				fprintf(fd, "\tmovq %s, %s\n", get_reg_name(ret, pointer_size), tmp);
		}
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
		generate_comparison_expression(fd, e, jmp_eq, "is$eq$%d", "is$not$eq$%d", lhs);
	} else if (!strcmp(e->attrs.bin_op, "<")) {
		generate_comparison_expression(fd, e, jmp_lt, "is$lt$%d", "is$not$lt$%d", lhs);
	} else if (!strcmp(e->attrs.bin_op, ">")) {
		generate_comparison_expression(fd, e, jmp_gt, "is$gt$%d", "is$not$gt$%d", lhs);
	} else if (!strcmp(e->attrs.bin_op, "!=")) {
		generate_comparison_expression(fd, e, jmp_neq, "is$ne$%d", "is$eq$%d", lhs);
	} else if (!strcmp(e->attrs.bin_op, "+=")) {
		fprintf(fd, "\t#Note: lhs, and rhs of assignment is swapped\n");
		fprintf(fd, "\t#(\n\t#(\n");
		generate_expression(fd, e->right);
		assign_reg(fd, ret, lhs);
		fprintf(fd, "\t#)\n\t#+=\n\t#(\n");
		char *var=prepare_var_assignment(fd, e->left);
		fprintf(fd, "\t#)\n");
		int_inc_by(fd, lhs, var);
		fprintf(fd, "\t#)\n");
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
		fprintf(fd, "\t#return\n\t#(\n");
		generate_expression(fd, s->attrs.expr);
		fprintf(fd, "\t#)\n");
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
	} else if (s->kind==_while) {
		struct reg_t *ret=get_ret_register(word_size);
		int *l=malloc(sizeof(int));
		unique_num++;
		*l=unique_num;
		push(loop_stack, l);

		char *loop_start, *loop_end;
		asprintf(&loop_start, "loop$start$%d", unique_num);
		asprintf(&loop_end, "loop$end$%d", unique_num);

		fprintf(fd, "\t#while (\n");
		place_label(fd, loop_start);
		generate_expression(fd, s->attrs._while.condition);

		compare_register_to_int(fd, ret, 0);
		jmp_eq(fd, loop_end);
		fprintf(fd, "\t#) {\n");
		generate_statement(fd, s->attrs._while.block);
		jmp(fd, loop_start);
		fprintf(fd, "\t#}\n");
		place_label(fd, loop_end);

		free(loop_start);
		free(loop_end);

	} else if (s->kind==_break) {
		int *n=pop(loop_stack);
		char *jump_to;
		asprintf(&jump_to, "loop$end$%d", *n);
		fprintf(fd, "\t#break; \n");
		jmp(fd, jump_to);
		free(jump_to);
		push(loop_stack, n);
	} else if (s->kind==_continue) {
		int *n=pop(loop_stack);
		char *jump_to;
		asprintf(&jump_to, "loop$start$%d", *n);
		fprintf(fd, "\t#continue; \n");
		jmp(fd, jump_to);
		free(jump_to);
		push(loop_stack, n);
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
		o=get_type_size(s->attrs.var->type);
		s->attrs.var->offset=-(o+current_off);
	} else if (s->kind==_if) {
		o+=get_var_offset(s->attrs._if.block, current_off+o);
	} else if (s->kind==_while) {
		o+=get_var_offset(s->attrs._while.block, current_off+o);
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

	while (loop_stack!=NULL) {
		struct stack_t *tmp=loop_stack->next;
		free(loop_stack->element);
		free(loop_stack);
		loop_stack=tmp;
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
