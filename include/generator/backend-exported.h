#ifndef __GENERATOR_BACKEND_EXPORTED_H__
#define __GENERATOR_BACKEND_EXPORTED_H__
#include "types.h"
#include "generator/generator-types.h"
#include <stdio.h>
extern void (*xor)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*or)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*and)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*test_or)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*assign_dereference)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*test_and)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*shift_left)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*shift_right)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*int_num)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*int_mul)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*int_div)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*int_sub)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*int_add)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*compare_registers)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*int_num)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*assign_reg)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void (*place_comment)(FILE *fd, char *str);
extern void (*load_global_string)(FILE *fd, char *str);
extern void (*test_invert)(FILE *fd, struct reg_t *a);
extern void (*invert)(FILE *fd, struct reg_t *a);
extern void (*int_neg)(FILE *fd, struct reg_t *a);
extern void (*int_inc)(FILE *fd, struct reg_t *a);
extern void (*int_dec)(FILE *fd, struct reg_t *a);
extern void (*call_function_pointer)(FILE *fd, struct reg_t *a);
extern void (*free_register)(FILE *fd, struct reg_t *a);
extern void (*start_func_ptr_call)(FILE *fd, struct reg_t *a);
extern void (*call)(FILE *fd, struct func_t *f);
extern void (*start_call)(FILE *fd, struct func_t *f);
extern void (*make_function)(FILE *fd, struct func_t *f);
extern void (*compare_register_to_int)(FILE *fd, struct reg_t *a, int i);
extern void (*jmp_ge)(FILE *fd, char *str);
extern void (*place_label)(FILE *fd, char *str);
extern void (*jmp_gt)(FILE *fd, char *str);
extern void (*jmp_lt)(FILE *fd, char *str);
extern void (*jmp_neq)(FILE *fd, char *str);
extern void (*jmp_eq)(FILE *fd, char *str);
extern void (*jmp)(FILE *fd, char *str);
extern void (*jmp_le)(FILE *fd, char *str);
extern void (*backend_make_global_var)(FILE *fd, struct var_t *v);
extern void (*read_var)(FILE *fd, struct var_t *v);
extern void (*load_function_ptr)(FILE *fd, struct func_t *f, struct reg_t *r);
extern void (*add_argument)(FILE *fd, struct reg_t *reg, struct type_t *t );
extern void (*return_from_call)(FILE *fd);
extern char* (*get_reg_name)(struct reg_t *reg, size_t size);
extern struct reg_t* (*get_ret_register)(size_t s);
extern struct reg_t* (*get_free_register)(FILE *fd, size_t s, int depth);
extern char* (*reg_name)(struct reg_t *a);
extern void (*set_register_size)(struct reg_t *r, size_t s);
extern void (*get_address)(FILE *fd, struct expr_t *var);
extern void (*dereference)(FILE *fd, struct reg_t *reg, size_t size);
extern void (*assign_var)(FILE *fd, struct reg_t *src, struct var_t *dest);
extern char* (*make_global_string)(FILE *fd, char *str);
extern void (*assign_constant_int)(FILE *fd, int e);
extern void (*assign_constant)(FILE *fd, struct expr_t *e);
extern void (*assign_constant_float)(FILE *fd, struct expr_t *e);
extern void (*float_sub)(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void (*float_add)(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void (*compare_float_registers)(FILE *fd, struct reg_t *a, struct reg_t *b);
extern char* (*generate_global_float)(FILE *fd, char *num);
#endif
