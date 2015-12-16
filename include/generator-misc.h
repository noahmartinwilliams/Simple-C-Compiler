#ifndef __GENERATOR_MISC_H__
#define __GENERATOR_MISC_H__
#include "types.h"

extern char* generate_global_string(FILE *fd, char *str);
extern char* prepare_var_assignment(FILE *fd, struct expr_t *dest);
extern void get_address(FILE *fd, struct expr_t *_var);
extern void setup_types();
extern void setup_generator();
extern void generate_statement(FILE *fd, struct statem_t *s);
extern off_t get_var_offset(struct statem_t *s, off_t current_off);
extern void generate_function(FILE *fd, struct func_t *f);
extern void generate_global_vars(FILE *fd, struct statem_t *s);

#endif
