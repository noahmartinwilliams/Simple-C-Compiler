call_arg_list: noncomma_expression {
	struct expr_t *e=arg_expr();
	if ($1->type->body->kind==_struct) {
		struct expr_t *current=e;
		int x;
		for (x=0; x<$1->type->body->attrs.vars.num_vars; x++) {
			current->attrs.argument=struct_dot_expr($1, $1->type->body->attrs.vars.vars[x]->name);
			current->type=current->attrs.argument->type;
			current->type->refcount++;
			if (x!=$1->type->body->attrs.vars.num_vars-1) {
				current->right=malloc(sizeof(struct expr_t));
				current->right->kind=arg;
				current=current->right;
			}
		}
	} else {
		e->type=$1->type;
		e->type->refcount++;
		e->attrs.argument=$1;
	}
	$$=e;
} | call_arg_list ',' noncomma_expression {
	struct expr_t *e=arg_expr();
	struct expr_t *tmp=$1;
	for (; tmp->right!=NULL; tmp=tmp->right) {
	}
	e->type=$3->type;
	e->type->refcount++;
	e->attrs.argument=$3;
	tmp->right=e;
	$$=$1;
};

expression: noncomma_expression | noncomma_expression ',' noncomma_expression {
	$$=bin_expr(",", $1, $3, NULL);
};
noncomma_expression:  CONST_INT {
	$$=const_int_expr($1, NULL);
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
	calls_function=true;
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
	calls_function=true;
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
		int x;
		struct expr_t *tmp=$3;
		for (x=0; tmp->right!=NULL; tmp=tmp->right)
			x++;
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
	e->left=e->right=NULL;
	e->attrs.cstr_val=generate_global_string(output, $1);
	free($1);
	$$=e;
} | postfix_expr | SIZEOF '(' type_with_stars ')' {
	$$=const_int_expr(get_type_size($3), get_type_by_name("long", _normal));
} | noncomma_expression '?' noncomma_expression ':' noncomma_expression {
	$$=bin_expr("?", $1, bin_expr(":", $3, $5, NULL), NULL);
} | ALIGNOF '(' type ')' {
	$$=const_int_expr(get_alignof($3), NULL);
	free_type($3);
} | '(' type ')' noncomma_expression {
	$$=convert_expr($4, $2);
};

postfix_expr: assignable_expr INC_OP {
	$$=postfix_expr("++", $1, $1->type);
} | assignable_expr DEC_OP {
	$$=postfix_expr("--", $1, $1->type);
};

prefix_expr: '&' assignable_expr {
	$$=prefix_expr("&", $2, increase_type_depth($2->type, 1));
} | CHAR_LITERAL {
	$$=const_int_expr((long int) $1, get_type_by_name("char", _normal));
} | '!' noncomma_expression {
	$$=prefix_expr("!", $2, $2->type);
} | INC_OP assignable_expr {
	$$=prefix_expr("++", $2, $2->type);
} | '~' noncomma_expression {
	$$=prefix_expr("~", $2, $2->type);
} | '-' noncomma_expression {
	$$=prefix_expr("-", $2, $2->type);
};

assignable_expr: IDENTIFIER {
	if (is_constant($1)!=NULL)
		$$=is_constant($1);
	else {
		struct var_t *v=get_var_by_name($1);
		if (v==NULL) {
			struct func_t *f=get_func_by_name($1);
			if (f==NULL) {
				char *msg;
				asprintf(&msg, "Unknown var %s", $1);
				yyerror(msg);
				exit(1);
			}
			free($1);
			struct expr_t *e=malloc(sizeof(struct expr_t));
			e->kind=func_val;
			e->type=increase_type_depth(get_type_by_name("void", _normal), 1);
			/*TODO: fix this to be more sophisticated later on. */

			e->left=e->right=NULL;
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
	$$=prefix_expr("*", $2, decrease_type_depth($2->type, 1));
} | assignable_expr '.' IDENTIFIER {
	struct type_t *type=$1->type;
	struct tbody_t *body=type->body;
	if (body->kind==_union) {
		$$=$1;
		free_type($$->type);
		$$->type=get_struct_or_union_attr_type(type, $3);
		free($3);
	} else if (body->kind==_struct) {
		/* a.b ---> *(&a+offsetof(typeof(a), b)) */
		/* TODO: ensure that a.b.c works properly */

		struct type_t *t=increase_type_depth(type, 1);
		type->refcount++;
		$$=struct_dot_expr($1, $3);
		free($3);
	}
} | assignable_expr POINTER_OP IDENTIFIER {
	/* a->b ---> *(a+offsetof(typeof(a), b)) */
	$$=prefix_expr("*", 
		bin_expr("+", 
			$1, 
			const_int_expr(
				get_offset_of_member($1->type, $3), 
				increase_type_depth(get_type_by_name("int", _normal), 1)
			),
			NULL
			), 
		get_struct_or_union_attr_type($1->type, $3)
		);


	free($3);
} | assignable_expr '[' expression ']' {
	$$=prefix_expr("*",  
		bin_expr("+", 
			prefix_expr("&", 
				$1, 
				increase_type_depth($1->type, 1)
				),
			bin_expr("*", 
				get_array_lower_size($1), 
				convert_expr($3, $1->type),
				NULL
				),
			NULL
			), 
			$1->type
			);
};

binary_expr:  noncomma_expression '*' noncomma_expression {
	$$=bin_expr("*", $1, $3, NULL);
} | noncomma_expression '/' noncomma_expression {
	$$=bin_expr("/", $1, $3, NULL);
} | noncomma_expression '+' noncomma_expression {
	$$=bin_expr("+", $1, $3, NULL);
} | noncomma_expression '-' noncomma_expression {
	$$=bin_expr("-", $1, $3, NULL);
} | assignable_expr '=' noncomma_expression {
	if (is_constant_kind($1)) {
		/* This can happen due to the const keyword. */
		yyerror("error: can not assign to constant.");
		exit(1);
	}
	$$=bin_expr("=", $1, $3, NULL);
} | noncomma_expression '<' noncomma_expression {
	$$=bin_expr("<", $1, $3, NULL);
} | noncomma_expression '>' noncomma_expression {
	$$=bin_expr(">", $1, $3, NULL);
} | noncomma_expression NE_TEST noncomma_expression {
	$$=bin_expr("!=", $1, $3, NULL);
} | assignable_expr ASSIGN_OP noncomma_expression {
	if (is_constant_kind($1))
		yyerror("can't assign to constant.");
	$$=bin_expr($2, $1, $3, NULL);
} | noncomma_expression GE_TEST noncomma_expression {
	$$=bin_expr(">=", $1, $3, NULL);
} | noncomma_expression LE_TEST noncomma_expression {
	$$=bin_expr("<=", $1, $3, NULL);
} | noncomma_expression SHIFT_LEFT noncomma_expression {
	$$=bin_expr("<<", $1, $3, NULL);
} | noncomma_expression SHIFT_RIGHT noncomma_expression {
	$$=bin_expr(">>", $1, $3, NULL);
} | noncomma_expression '|' noncomma_expression {
	$$=bin_expr("|", $1, $3, NULL);
} | noncomma_expression '&' noncomma_expression {
	$$=bin_expr("&", $1, $3, NULL);
} | noncomma_expression '^' noncomma_expression {
	$$=bin_expr("^", $1, $3, NULL);
} | noncomma_expression TEST_OR noncomma_expression {
	$$=bin_expr("||", $1, $3, NULL);
} | noncomma_expression TEST_AND noncomma_expression {
	$$=bin_expr("&&", $1, $3, NULL);
} | noncomma_expression EQ_TEST noncomma_expression {
	$$=bin_expr("==", $1, $3, NULL);
} | noncomma_expression '%' noncomma_expression {
	$$=bin_expr("%", $1, $3, NULL);
};
