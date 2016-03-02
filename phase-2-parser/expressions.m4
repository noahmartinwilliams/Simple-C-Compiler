call_arg_list: noncomma_expression {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=arg;
	e->left=e->right=NULL;
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
	e->right=e->left=NULL;
	e->attrs.argument=$3;
	tmp->right=e;
	$$=$1;
};

expression: noncomma_expression | noncomma_expression ',' noncomma_expression {
	$$=make_bin_op(",", $1, $3);
};
noncomma_expression: CONST_INT {
	$$=create_const_int_expr($1, NULL);
} | CONST_INT '.' CONST_INT {
	char *num;
	asprintf(&num, "%ld.%ld", $1, $3);
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=const_float;
	e->attrs.cfloat=generate_global_float(output, num);
	e->left=e->right=NULL;
	e->type=get_type_by_name("float", _normal);
	e->type->refcount++;
	free(num);
	$$=e;
} | assignable_expr | binary_expr | '(' expression ')' {
	$$=$2;
} | prefix_expr | IDENTIFIER '(' ')' {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->kind=funccall;
	e->left=e->right=NULL;
	e->attrs.function=get_func_by_name($1);
	e->attrs.function->num_calls++;
	parser_handle_inline_func(e->attrs.function->num_calls, e->attrs.function);
	e->type=e->attrs.function->ret_type;
	e->type->refcount++;
	found_inline_in_function=found_inline_in_function || e->attrs.function->attributes&_inline;
	free($1);
	$$=e;
} | IDENTIFIER '(' call_arg_list ')' {
	struct var_t *v=get_var_by_name($1);
	struct expr_t *e=malloc(sizeof(struct expr_t));
	if (v!=NULL && v->type->body->is_func_pointer) {
		e->kind=func_ptr_call;
		e->right=$3;
		e->left=NULL;
		e->type=v->type->body->attrs.func_ptr.return_type;
		e->type->refcount++;
		e->attrs.var=v;
		v->refcount++;
		e->has_gotos=$3->has_gotos;
		int x;
		struct expr_t *tmp=$3;
		for (x=0; tmp->right!=NULL; tmp=tmp->right) {
			x++;
		}
		if (!v->type->body->attrs.func_ptr.has_var_args) {
			if (x > v->type->body->attrs.func_ptr.num_arguments) {
				yyerror("Too many arguments to function pointer.");
				exit(1);
			} else if (x < v->type->body->attrs.func_ptr.num_arguments) {
				yyerror("Too few arguments to function pointer.");
				exit(1);
			}
		}
	} else {
		e->kind=funccall;
		e->right=$3;
		e->left=NULL;
		e->attrs.function=get_func_by_name($1);
		e->attrs.function->num_calls++;
		parser_handle_inline_func(e->attrs.function->num_calls, e->attrs.function);
		e->type=e->attrs.function->ret_type;
		e->type->refcount++;
	}
	free($1);
	found_inline_in_function=found_inline_in_function || e->attrs.function->attributes&_inline;
	$$=e;
} | STR_LITERAL {
	struct expr_t *e=malloc(sizeof(struct expr_t));
	e->type=increase_type_depth(get_type_by_name("char", _normal), 1);
	e->kind=const_str;
	e->right=NULL;
	e->left=NULL;
	e->attrs.cstr_val=generate_global_string(output, $1);
	free($1);
	$$=e;
} | postfix_expr | SIZEOF '(' type_with_stars ')' {
	$$=create_const_int_expr(get_type_size($3), get_type_by_name("long", _normal));
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
} | ALIGNOF '(' type ')' {
	$$=create_const_int_expr(get_alignof($3), NULL);
	free_type($3);
} | '(' type ')' noncomma_expression {
	$$=malloc(sizeof(struct expr_t));
	$$->kind=convert;
	$$->type=$2;
	$2->refcount++;
	$$->right=$4;
	$$->left=NULL;
	$$->has_gotos=$4->has_gotos;
};

postfix_expr: assignable_expr INC_OP {
	$$=make_prefix_or_postfix_op("++", $1, $1->type, false);
} | assignable_expr DEC_OP {
	$$=make_prefix_or_postfix_op("--", $1, $1->type, false);
};

prefix_expr: '&' assignable_expr {
	$$=make_prefix_or_postfix_op("&", $2, increase_type_depth($2->type, 1), true);
} | CHAR_LITERAL {
	$$=create_const_int_expr((long int) $1, get_type_by_name("char", _normal));
} | '!' noncomma_expression {
	$$=make_prefix_or_postfix_op("!", $2, $2->type, true);
} | INC_OP assignable_expr {
	$$=make_prefix_or_postfix_op("++", $2, $2->type, true);
} | '~' noncomma_expression {
	$$=make_prefix_or_postfix_op("~", $2, $2->type, true);
} | '-' noncomma_expression {
	$$=make_prefix_or_postfix_op("-", $2, $2->type, true);
};

assignable_expr: IDENTIFIER {
	if (is_constant($1)!=NULL) {
		$$=is_constant($1);
	} else {
		struct var_t *v=get_var_by_name($1);
		if (v==NULL) {
			struct func_t *f=get_func_by_name($1);
			if (f==NULL) {
				yyerror("Unkown var");
				exit(1);
			}
			free($1);
			struct expr_t *e=malloc(sizeof(struct expr_t));
			e->kind=func_val;
			e->type=increase_type_depth(get_type_by_name("void", _normal), 1);
			/*TODO: fix this to be more sophisticated later on. */

			e->left=e->right=NULL;
			e->has_gotos=false;
			e->attrs.function=f;
			$$=e;
		} else {
			free($1);
			struct expr_t *e=malloc(sizeof(struct expr_t));
			e->left=e->right=NULL;
			e->kind=var;
			e->attrs.var=v;
			e->type=v->type;
			e->type->refcount++;
			v->refcount++;
			$$=e;
		}
	}
} | '*' assignable_expr {
	$$=make_prefix_or_postfix_op("*", $2, decrease_type_depth($2->type, 1), true);
} | assignable_expr '.' IDENTIFIER {
	struct type_t *type=$1->type;
	struct tbody_t *body=type->body;
	if (body->kind==_union) {
		struct expr_t *e=malloc(sizeof(struct expr_t));
		memcpy(e, $1, sizeof(struct expr_t));
		e->type=get_struct_or_union_attr_type(type, $3);
		if (e->type==NULL)
			yyerror("Identifier is not a valid attribute.");
		free($3);
		$$=e;
	} else if (body->kind==_struct) {
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
		constant->type=get_type_by_name("int", _normal);
		constant->type->refcount++;

		deref->right=addition;
		deref->attrs.un_op=strdup("*");
		ref->attrs.un_op=strdup("&");
		deref->type=get_var_member(type, $3)->type; /* source of the problems. */
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
	offset->type=increase_type_depth(get_type_by_name("int", _normal), 1);
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
	if (is_constant_kind($1)) {
		/* This can happen due to the const keyword. */
		yyerror("error: can not assign to constant.");
		exit(1);
	}
	$$=make_bin_op("=", $1, $3);
} | noncomma_expression '<' noncomma_expression {
	$$=make_bin_op("<", $1, $3);
} | noncomma_expression '>' noncomma_expression {
	$$=make_bin_op(">", $1, $3);
} | noncomma_expression NE_TEST noncomma_expression {
	$$=make_bin_op("!=", $1, $3);
} | assignable_expr ASSIGN_OP noncomma_expression {
	if (is_constant_kind($1))
		yyerror("can't assign to constant.");
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
} | noncomma_expression '%' noncomma_expression {
	$$=make_bin_op("%", $1, $3);
};
