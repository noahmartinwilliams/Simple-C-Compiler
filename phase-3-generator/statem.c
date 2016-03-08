#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "generator/globals.h"
#include "generator/generator-expr.h"
#include "generator/misc.h"
#include "globals.h"
#include "generator/types.h"
#include "parser/types.h"
#include "parser/funcs.h"
#include "parser/exprs.h"
#include "stack.h"
#include "optimizations.h"
#include "optimization-globals.h"
#include "generator/backend-exported.h"
void generate_statement(FILE *fd, struct statem_t *s);
#include "statems/if-statem.h"
#include "statems/while-statem.h"
#include "statems/do-while-statem.h"
#include "statems/for-statem.h"
#include "statems/switch-statem.h"

void generate_statement(FILE *fd, struct statem_t *s)
{
	if (s==NULL)
		return; 

	depth++;
	struct reg_t *retu=get_ret_register(word_size, false);
	struct expr_t *cond;
	struct statem_t *_block;
	if (s->kind==expr) {
		place_comment(fd, "(");
		generate_expression(fd, s->expr);
		place_comment(fd, ");");
	} else if (s->kind==block) {
		generate_statement(fd, s->left);
		generate_statement(fd, s->right);
	} else if (s->kind==ret) {
		place_comment(fd, "return");
		place_comment(fd, "(");
		generate_expression(fd, s->expr);
		return_from_call(fd);
		place_comment(fd, ");");
	} else if (s->kind==declare) {
		if (s->expr!=NULL) {
			struct reg_t *retu=get_ret_register(word_size, s->expr->type->body->core_type==_FLOAT);
			generate_expression(fd, s->expr);
			assign_var(fd, retu, s->attrs.var);
		}
		return;
	} else if (s->kind==_if)
		generate_if_statement(fd, s, retu, cond, _block);
	else if (s->kind==_while)
		generate_while_loop(fd, s, retu, cond, _block);
	else if (s->kind==_break) {
		int *n=pop(loop_stack);
		char *jump_to;
		asprintf(&jump_to, "loop$end$%d", *n);
		place_comment(fd, "break;");
		jmp(fd, jump_to);
		free(jump_to);
		push(loop_stack, n);
	} else if (s->kind==_continue) {
		int *n=pop(loop_stack);
		char *jump_to;
		asprintf(&jump_to, "loop$start$%d", *n);
		place_comment(fd, "continue;");
		jmp(fd, jump_to);
		free(jump_to);
		push(loop_stack, n);
	} else if (s->kind==_goto) {
		place_comment(fd, "goto ");
		place_comment(fd, s->attrs.label_name);
		place_comment(fd, ";");

		jmp(fd, s->attrs.label_name);
	} else if (s->kind==label) {
		place_comment(fd, s->attrs.label_name);

		place_label(fd, s->attrs.label_name);
	} else if (s->kind==_for)
		generate_for_loop(fd, s, retu, cond, _block);
	else if (s->kind==do_while)
		generate_do_while_loop(fd, s, retu, cond, _block);
	else if (s->kind==_switch)
		generate_switch_statement(fd, s, retu, cond, _block);
	depth--;
}
