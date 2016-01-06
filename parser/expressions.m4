call_arg_list: noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=arg;
	e->left=NULL;
	e->right=NULL;
	e->type=$1->type;
	e->type->refcount++;
	e->attrs.argument=$1;
	$$=e;
} | call_arg_list ',' noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	struct expr_t *tmp=$1;
	for (; tmp->right!=NULL; tmp=tmp->right) {
	}
	e->kind=arg;
	e->type=$3->type;
	e->type->refcount++;
	e->right=NULL;
	e->left=NULL;
	e->attrs.argument=$3;
	tmp->right=e;
	$$=$1;
};

expression: noncomma_expression ;
noncomma_expression: CONST_INT {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->type=get_type_by_name("int");
	e->type->refcount++;
	e->kind=const_int;
	e->left=NULL;
	e->right=NULL;
	e->attrs.cint_val=$1;
	$$=e;
} | assignable_expr | binary_expr | '(' expression ')' {
	$$=$2;
} | prefix_expr | IDENTIFIER '(' ')' {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=funccall;
	e->left=NULL;
	e->right=NULL;
	e->attrs.function=get_func_by_name($1);
	e->attrs.function->num_calls++;
	parser_handle_inline_func(e->attrs.function->num_calls, e->attrs.function);
	e->type=e->attrs.function->ret_type;
	e->type->refcount++;
	free($1);
	$$=e;
} | IDENTIFIER '(' call_arg_list ')' {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=funccall;
	e->right=$3;
	e->left=NULL;
	e->attrs.function=get_func_by_name($1);
	e->attrs.function->num_calls++;
	parser_handle_inline_func(e->attrs.function->num_calls, e->attrs.function);
	e->type=e->attrs.function->ret_type;
	e->type->refcount++;
	free($1);
	$$=e;
} | STR_LITERAL {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->type=increase_type_depth(get_type_by_name("char"), 1);
	e->kind=const_str;
	e->right=NULL;
	e->left=NULL;
	e->attrs.cstr_val=generate_global_string(output, $1);
	free($1);
	$$=e;
} | postfix_expr | SIZEOF '(' type_with_stars ')' {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=const_size_t;
	e->attrs.cint_val=get_type_size($3);
	e->type=get_type_by_name("size_t");
	e->type->refcount++;
	e->left=e->right=NULL;
	$$=e;
} | noncomma_expression '?' noncomma_expression ':' noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=bin_op;
	e->attrs.bin_op=strdup("?");
	e->type=$3->type;
	e->type->refcount++;

	e->left=$1;
	struct expr_t *e2=e->right=malloc(sizeof(struct expr_t));
	
	e2->kind=bin_op;
	e2->attrs.bin_op=strdup(":");
	e2->left=$3;
	e2->right=$5;
	e2->type=e->type;
	e->type->refcount++;

	$$=e;
};

postfix_expr: assignable_expr INC_OP {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=post_un_op;
	e->left=$1;
	e->right=NULL;
	e->attrs.un_op=strdup("++");
	e->type=$1->type;
	$1->type->refcount++;
	$$=e;
} | assignable_expr DEC_OP {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=post_un_op;
	e->left=$1;
	e->right=NULL;
	e->attrs.un_op=strdup("--");
	e->type=$1->type;
	$1->type->refcount++;
	$$=e;
};

prefix_expr: '&' assignable_expr {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=pre_un_op;
	e->attrs.un_op=strdup("&");
	e->right=$2;
	e->left=NULL;
	e->type=increase_type_depth($2->type, 1);
	$$=e;
} | CHAR_LITERAL {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=const_int;
	e->type=get_type_by_name("char");
	e->type->refcount++;
	e->left=NULL;
	e->right=NULL;
	e->attrs.cint_val=(long int) $1;
	$$=e;
} | '!' noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=pre_un_op;
	e->attrs.un_op=strdup("!");
	e->right=$2;
	e->left=NULL;
	e->type=$2->type;
	e->type->refcount++;
	$$=e;
} | INC_OP assignable_expr {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=pre_un_op;
	e->attrs.un_op=strdup("++");
	e->right=$2;
	e->left=NULL;
	e->type=$2->type;
	e->type->refcount++;
	$$=e;
} | '~' noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=pre_un_op;
	e->attrs.un_op=strdup("~");
	e->right=$2;
	e->left=NULL;
	e->type=$2->type;
	e->type->refcount++;
	$$=e;
} | '-' noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=pre_un_op;
	e->type=$2->type;
	e->type->refcount++;
	e->left=NULL;
	e->right=$2;
	e->attrs.un_op=strdup("-");
	$$=e;
};

assignable_expr: IDENTIFIER {
	struct var_t *v=get_var_by_name($1);
	free($1);
	if (v==NULL) {
		yyerror("Unkown var");
		exit(1);
	}
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->left=NULL;
	e->right=NULL;
	e->kind=var;
	e->attrs.var=v;
	e->type=v->type;
	e->type->refcount++;
	v->refcount++;
	$$=e;
} | '*' assignable_expr {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=pre_un_op;
	e->attrs.un_op=strdup("*");
	e->right=$2;
	e->left=NULL;
	e->type=decrease_type_depth($2->type, 1);
	$$=e;
} | assignable_expr '.' IDENTIFIER {
	struct type_t *type=$1->type;
	struct tbody_t *body=type->body;
	if (body->is_union) {
		struct expr_t *e=malloc(sizeof(struct expr_t));
		memcpy(e, $1, sizeof(struct expr_t));
		e->type=get_struct_or_union_attr_type(type, $3);
		free($3);
		$$=e;
	} else if (body->is_struct) {
		/* a.b ---> *(&a+offsetof(typeof(a), b)) */
		/* TODO: ensure that a.b.c works properly */
		struct expr_t *deref=malloc(sizeof(struct expr_t));
		struct expr_t *addition=malloc(sizeof(struct expr_t));
		struct expr_t *ref=malloc(sizeof(struct expr_t));
		struct expr_t *constant=malloc(sizeof(struct expr_t));

		deref->kind=ref->kind=pre_un_op;
		addition->kind=bin_op;
		constant->kind=const_int;
		constant->left=constant->right=deref->left=ref->left=NULL;
		constant->type=get_type_by_name("int");
		constant->type->refcount++;

		deref->right=addition;
		deref->attrs.un_op=strdup("*");
		ref->attrs.un_op=strdup("&");
		deref->type=get_var_member(type, $3)->type;
		deref->type->refcount++;

		constant->attrs.cint_val=get_offset_of_member(type, $3);

		ref->right=$1;
		ref->type=increase_type_depth(type, 1);

		addition->attrs.bin_op=strdup("+");
		addition->left=ref;
		addition->right=constant;
		addition->type=ref->type;
		ref->type->refcount++;

		type->refcount++;
		$$=deref;
		free($3);
	}
} | assignable_expr POINTER_OP IDENTIFIER {
	/* a->b ---> *(a+offsetof(typeof(a), b)) */
	struct expr_t *pointer=malloc(sizeof(struct expr_t));
	struct expr_t *var=$1;
	struct expr_t *addition=malloc(sizeof(struct expr_t));
	struct expr_t *offset=malloc(sizeof(struct expr_t));

	offset->kind=const_int;
	offset->type=increase_type_depth(get_type_by_name("int"), 1);
	addition->kind=bin_op;
	addition->type=var->type;
	var->type->refcount++;
	pointer->kind=pre_un_op;
	pointer->type=get_struct_or_union_attr_type($1->type, $3);

	pointer->left=offset->left=offset->right=NULL;

	addition->attrs.bin_op=strdup("+");
	offset->attrs.cint_val=get_offset_of_member($1->type, $3);
	pointer->attrs.un_op=strdup("*");
	
	pointer->right=addition;

	addition->left=var;
	addition->right=offset;

	free($3);

	$$=pointer;
};

binary_expr:  noncomma_expression '*' noncomma_expression {
	$$=make_bin_op("*", $1, $3);
} | noncomma_expression '/' noncomma_expression {
	$$=make_bin_op("/", $1, $3);
} | noncomma_expression '+' noncomma_expression {
	$$=make_bin_op("+", $1, $3);
} | noncomma_expression '-' noncomma_expression {
	$$=make_bin_op("-", $1, $3);
} | assignable_expr '=' noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	struct expr_t *a=$1, *b=$3;
	parser_type_cmp(&a, &b);
	e->type=b->type;
	e->type->refcount++;
	e->kind=bin_op;
	e->left=a;
	e->right=b;
	e->attrs.bin_op=strdup("=");
	$$=e;
} | noncomma_expression '<' noncomma_expression {
	$$=make_bin_op("<", $1, $3);
} | noncomma_expression '>' noncomma_expression {
	$$=make_bin_op(">", $1, $3);
} | noncomma_expression NE_TEST noncomma_expression {
	$$=make_bin_op("!=", $1, $3);
} | assignable_expr ASSIGN_OP noncomma_expression {
	$$=make_bin_op($2, $1, $3);
} | noncomma_expression GE_TEST noncomma_expression {
	$$=make_bin_op(">=", $1, $3);
} | noncomma_expression LE_TEST noncomma_expression {
	$$=make_bin_op("<=", $1, $3);
} | noncomma_expression SHIFT_LEFT noncomma_expression {
	$$=make_bin_op("<<", $1, $3);
} | noncomma_expression SHIFT_RIGHT noncomma_expression {
	$$=make_bin_op(">>", $1, $3);
} | noncomma_expression '|' noncomma_expression {
	$$=make_bin_op("|", $1, $3);
} | noncomma_expression '&' noncomma_expression {
	$$=make_bin_op("&", $1, $3);
} | noncomma_expression '^' noncomma_expression {
	$$=make_bin_op("^", $1, $3);
} | noncomma_expression TEST_OR noncomma_expression {
	$$=make_bin_op("||", $1, $3);
} | noncomma_expression TEST_AND noncomma_expression {
	$$=make_bin_op("&&", $1, $3);
} | noncomma_expression EQ_TEST noncomma_expression {
	$$=make_bin_op("==", $1, $3);
} ;
