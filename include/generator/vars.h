#ifndef __INCLUDE_GENERATOR_VARS_H__
#define __INCLUDE_GENERATOR_VARS_H__
#include <stdio.h>
#include "types.h"

extern char* prepare_var_assignment(FILE *fd, struct expr_t *dest);
extern void generate_global_vars(FILE *fd, struct statem_t *s);
extern off_t get_var_offset(struct statem_t *s, off_t current_off);

#endif
