#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/generator-globals.h"
#include "generator/generator-expr.h"
#include "globals.h"
#include "generator/backend/backend.h"
#include "generator/generator-types.h"
#include "handle-types.h"
#include "handle-funcs.h"
#include "stack.h"
#include "generator/generator-statem.h"

char* generate_global_string(FILE *fd, char *str)
{
	return make_global_string(fd, str);
}

char* prepare_var_assignment(FILE *fd, struct expr_t *dest);

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

void setup_types()
{
	num_types=3;
	types=realloc(types, num_types*sizeof(struct type_t*));
	types[num_types-1]=malloc(sizeof(struct type_t));
	struct type_t *i=types[num_types-1];
	i->name=strdup("int");
	i->pointer_depth=0;
	i->body=malloc(sizeof(struct tbody_t));
	i->body->size=int_size;
	i->body->is_struct=false;
	i->body->refcount=1;
	i->body->is_union=false;
	i->refcount=1;
	i->native_type=true;

	types[num_types-2]=malloc(sizeof(struct type_t));
	i=types[num_types-2];
	i->name=strdup("char");
	i->pointer_depth=0;
	i->body=malloc(sizeof(struct tbody_t));
	i->body->size=char_size;
	i->body->is_struct=false;
	i->body->refcount=1;
	i->body->is_union=false;
	i->refcount=1;
	i->native_type=true;

	types[num_types-3]=malloc(sizeof(struct type_t));
	i=types[num_types-3];
	i->name=strdup("void");
	i->pointer_depth=0;
	i->body=malloc(sizeof(struct tbody_t));
	i->body->size=0;
	i->body->is_struct=false;
	i->body->refcount=1;
	i->body->is_union=false;
	i->refcount=1;
	i->native_type=true;
}

void setup_generator()
{
	setup_types();
	setup_registers();
}

void generate_function(FILE *fd, struct func_t *f)
{
	make_function(fd, f);
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

void generate_global_vars(FILE *fd, struct statem_t *s)
{
	if (s->kind==list) {
		int x;
		for (x=0; x<s->attrs.list.num; x++) {
			generate_global_vars(fd, s->attrs.list.statements[x]);
		}
	} else if (s->kind==declare) {
		backend_make_global_var(fd, s->attrs.var);
		s->attrs.var->scope=0;
	}
}
