statement: expression ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=expr;
	s->attrs.expr=$1;
	s->has_gotos=false;
	/* TODO: fix this up. */
	$$=s;
} | '{' statement_list '}' {
	$$=$2;
} | var_declaration | WHILE '(' expression ')' statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_while;
	s->attrs._while.condition=$3;
	s->attrs._while.block=$5;
	s->has_gotos=$5->has_gotos;
	$$=s;
} | RETURN expression ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=ret;
	s->attrs.expr=$2;
	s->has_gotos=false;
	/*TODO: fix this up. */
	$$=s;
} | IF '(' expression ')' statement ELSE statement {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_if;
	s->attrs._if.condition=$3;
	s->attrs._if.block=$5;
	s->attrs._if.else_block=$7;
	s->has_gotos=$5->has_gotos || $7->has_gotos;
	$$=s;
} | IF '(' expression ')' statement %prec IFX{
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_if;
	s->has_gotos=$5->has_gotos;
	s->attrs._if.condition=$3;
	s->attrs._if.block=$5;
	s->attrs._if.else_block=NULL;
	$$=s;
} | BREAK ';' { 
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_break;
	s->has_gotos=false;
	$$=s;
} | CONTINUE ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_continue;
	s->has_gotos=false;
	$$=s;
} | IDENTIFIER ':' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=label;
	s->attrs.label_name=strdup($1);
	s->has_gotos=true;
	free($1);
	$$=s;
} | GOTO IDENTIFIER ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=_goto;
	s->attrs.label_name=strdup($2);
	s->has_gotos=true;
	free($2);
	$$=s;
}  | for_loop | DO statement WHILE '(' expression ')' ';' {
	struct statem_t *s=malloc(sizeof(struct statem_t));
	s->kind=do_while;
	s->has_gotos=$2->has_gotos;
	s->attrs.do_while.condition=$5;
	s->attrs.do_while.block=$2;
	$$=s;
};
