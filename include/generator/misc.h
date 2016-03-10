#ifndef __GENERATOR_MISC_H__
#define __GENERATOR_MISC_H__
#include "../types.h"

extern char* generate_global_string(FILE *fd, char *str);
extern void setup_types();
extern void setup_generator();
extern void generate_function(FILE *fd, struct func_t *f);

#endif
