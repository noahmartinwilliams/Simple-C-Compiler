#ifndef __HANDLE_REGISTERS_H__
#define __HANDLE_REGISTERS_H__
#include <stdlib.h>

extern void free_all_registers();


extern void setup_registers();
extern struct reg_t* get_ret_register(size_t s);
extern void add(FILE *fd, struct reg_t *a, struct reg_t *b);
extern void assign(FILE *fd, struct reg_t *src, struct reg_t *dest);
extern void read_var(FILE *fd, struct var_t *v);
#endif
