block: statement { 
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=block;
	$$->left=$1;
} | block statement {
	$$=$1;
	struct statem_t *s=$1;
	for (; s->right!=NULL; s=s->right) {}
	s->right=malloc(sizeof(struct statem_t));
	init_statem(s->right);
	s=s->right;
	s->kind=block;
	s->right=NULL;
	s->left=$2;
};


possibly_blank_expr: %empty {
	$$=NULL;
} | expression {
	$$=$1;
};

statement: expression ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=expr;
	$$->expr=$1;
	$$->has_gotos=false;
} | '{' block '}' {
	$$=$2;
} | var_declaration | WHILE '(' expression ')' statement {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_while;
	$$->expr=$3;
	$$->right=$5;
	$$->has_gotos=$5->has_gotos;
} | RETURN possibly_blank_expr ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=ret;
	$$->expr=$2;
	$$->has_gotos=false;
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
} | CONTINUE ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_continue;
} | IDENTIFIER ':' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=label;
	$$->attrs.label_name=$1;
	$$->has_gotos=true;
} | GOTO IDENTIFIER ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_goto;
	$$->attrs.label_name=$2;
	$$->has_gotos=true;
} | DO statement WHILE '(' expression ')' ';' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=do_while;
	$$->has_gotos=$2->has_gotos;
	$$->expr=$5;
	$$->right=$2;
} | SWITCH '(' expression ')' '{' switch_list '}' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_switch;
	$$->has_gotos=$6->has_gotos;
	$$->right=$6;
	$$->expr=$3;
} | FOR '(' possibly_blank_expr ';' possibly_blank_expr ';' possibly_blank_expr ')' statement {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_for;
	$$->attrs._for.initial=$3;
	if ($5==NULL) {
		struct expr_t *e=malloc(sizeof(struct expr_t));
		e->kind=const_int;
		e->attrs.cint_val=1;
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
	$$->kind=block;
	$$->left=$1;
	$$->right=NULL;
} | switch_list switch_element {
	struct statem_t *s=$1;
	for (; s->right!=NULL; s=s->right) {}
	s->right=malloc(sizeof(struct statem_t));
	init_statem(s->right);
	s->right->kind=block;
	s->right->left=$2;
	s->right->right=NULL;
	$$=$1;
} ;

switch_element: CASE expression ':' block {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_case;
	$$->right=$4;
	$$->expr=$2;
} | DEFAULT ':' block {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_default;
	$$->right=$3;
} | CASE expression ':' {
	$$=malloc(sizeof(struct statem_t));
	init_statem($$);
	$$->kind=_case;
	$$->expr=$2;
}
