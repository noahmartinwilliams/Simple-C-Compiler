#ifndef __TYPES_H__
#define __TYPES_H__
#include <stdbool.h>
#include <stdlib.h>

typedef void (*__printer_function_t) (char*, void*); /* For type conversion for
the print_tree function so I don't get annoying warnings because I never pass it
a function pointer that has void* as the second argument type. */

enum type_kind { _normal, _enum, _struct, _union };
struct tbody_t {
	size_t size;
	int refcount;
	int base_pointer_depth;
	bool is_func_pointer;
	enum type_kind kind;

	enum { _FLOAT, _INT } core_type;
	union {
		struct {
			struct var_t **vars;
			int num_vars;
			size_t alignment;
		} vars;

		struct {
			struct type_t *return_type;
			struct type_t **arguments;
			bool has_var_args;
			int num_arguments;
		} func_ptr;
	} attrs;
};

struct type_t {
	bool native_type;
	int refcount;
	char *name;
	int pointer_depth;
	struct tbody_t *body;
	bool is_signed;
};

struct expr_t {
	enum { bin_op=0x01, pre_un_op=0x02, post_un_op=0x03, question=0x04, const_int=0x05, const_float=0x06, var=0x07, funccall=0x08, arg=0x09, const_str=0x0A, const_size_t=0x0B, func_val=0x0C, func_ptr_call=0x0D, convert=0x0E } kind;
	struct expr_t *left, *right;
	struct type_t *type;
	bool has_gotos;
	union {
		struct var_t *var;
		long int cint_val;
		char* cfloat;
		char *bin_op;
		char *un_op;
		char *cstr_val;
		struct expr_t *argument;
		struct func_t *function;
	} attrs;
};

struct var_t {
	char *name;
	off_t offset;
	int scope_depth;
	bool hidden;
	bool is_register;
	struct reg_t *reg;
	struct type_t *type;
	int refcount;
};

struct const_t {
	char *name;
	int scope;
	bool is_hidden;
	struct expr_t *e;
};

struct statem_t {
	enum { expr=0x01, block=0x02, declare=0x03, _while=0x04, ret=0x05, _if=0x06, _break=0x07, _continue=0x08, label=0x09, _goto=0x0A, _for=0x0B, do_while=0x0C, _switch=0x0D, _case=0x0E, _default=0x0F } kind;
	bool has_gotos;
	struct statem_t *left;
	struct expr_t *expr; 
	struct statem_t *right;
	union {
		char *label_name;
		struct var_t *var;

		struct {
			struct expr_t *initial, *update;
		} _for;
	} attrs;
};

struct func_t {
	char *name;
	enum { _extern, _static, _inline } attributes;
	struct statem_t *statement_list;
	struct type_t *ret_type;
	struct var_t **arguments;
	int num_arguments;
	bool has_var_args;
	bool do_inline;

	int num_calls;
};

#endif
