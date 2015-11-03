#include "types.h"
#include <stdlib.h>
#include "globals.h"
#include <string.h>
#include <stdio.h>
#include "handle-registers.h"
#include "generator-types.h"


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

