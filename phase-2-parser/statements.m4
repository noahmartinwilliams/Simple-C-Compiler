statement_list: statement { 
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=list;
	$$->left=$1;
	$$->right=NULL;
} | statement_list statement {
	$$=$1;
	struct statem_t *s=$1;
	for (; s->right!=NULL; s=s->right) {}
	s->right=malloc(sizeof(struct statem_t));
	init_statem(s->right);
	s=s->right;
	s->kind=list;
	s->right=NULL;
	s->left=$2;
};


maybe_empty_expr: %empty {
	$$=NULL;
} | expression {
	$$=$1;
};

statement: expression ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=expr;
	$$->expr=$1;
	$$->has_gotos=$1->has_gotos;
} | '{' statement_list '}' {
	$$=$2;
} | var_declaration | WHILE '(' expression ')' statement {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_while;
	$$->expr=$3;
	$$->right=$5;
	$$->has_gotos=$5->has_gotos;
	$$->left=NULL;
} | RETURN expression ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=ret;
	$$->expr=$2;
	$$->has_gotos=false;
	/*TODO: Fix this to properly detect gotos from the epxression statements in expression. */
} | IF '(' expression ')' statement ELSE statement {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_if;
	$$->left=$5;
	$$->right=$7;
	$$->expr=$3;
	$$->has_gotos=$5->has_gotos || $7->has_gotos;
} | IF '(' expression ')' statement %prec IFX{
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_if;
	$$->left=$5;
	$$->has_gotos=$5->has_gotos;
	$$->expr=$3;
} | BREAK ';' { 
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_break;
	$$->has_gotos=false;
} | CONTINUE ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_continue;
	$$->has_gotos=false;
} | IDENTIFIER ':' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=label;
	$$->attrs.label_name=strdup($1);
	$$->has_gotos=true;
	free($1);
} | GOTO IDENTIFIER ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_goto;
	$$->attrs.label_name=strdup($2);
	$$->has_gotos=true;
	free($2);
} | DO statement WHILE '(' expression ')' ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=do_while;
	$$->has_gotos=$2->has_gotos || $5->has_gotos;
	$$->expr=$5;
	$$->right=$2;
} | SWITCH '(' expression ')' '{' switch_list '}' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_switch;
	$$->has_gotos=$6->has_gotos;
	$$->right=$6;
	$$->expr=$3;
} | RETURN ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=ret;
	$$->expr=NULL;
	$$->has_gotos=false;
} | FOR '(' maybe_empty_expr ';' maybe_empty_expr ';' maybe_empty_expr ')' statement {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_for;
	$$->attrs._for.initial=$3;
	if ($5==NULL) {
		struct expr_t *e=malloc(sizeof(struct expr_t));
		e->kind=const_int;
		e->attrs.cint_val=1;
		e->left=e->right=NULL;
		e->has_gotos=false;
		e->type=get_type_by_name("int", _normal);
		$$->expr=e;
	} else
		$$->expr=$5;
	$$->attrs._for.update=$7;
	$$->right=$9;
	$$->has_gotos=$9->has_gotos;
};

switch_list: switch_element {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=list;
	$$->left=$1;
	$$->right=NULL;
} | switch_list switch_element {
	struct statem_t *s=$1;
	for (; s->right!=NULL; s=s->right) {}
	s->right=malloc(sizeof(struct statem_t));
	init_statem(s->right);
	s->right->kind=list;
	s->right->left=$2;
	s->right->right=NULL;
	$$=$1;
} ;

switch_element: CASE expression ':' statement_list {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_case;
	$$->left=NULL;
	$$->right=$4;
	$$->expr=$2;
} | DEFAULT ':' statement_list {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_default;
	$$->right=$3;
	$$->left=NULL;
	$$->expr=NULL;
} | CASE expression ':' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_case;
	$$->expr=$2;
	$$->left=$$->right=NULL;
}
