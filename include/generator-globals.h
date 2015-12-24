#ifndef __GENERATOR_GLOBALS_H__
#define __GENERATOR_GLOBALS_H__
#include "stdbool.h"
#include "stddef.h"

extern int depth;
extern bool in_main;
extern size_t word_size;
extern size_t int_size;

extern int unique_num;
extern struct stack_t *loop_stack;
extern size_t pointer_size;
extern size_t char_size;
extern struct reg_t **regs;
extern int num_regs;
extern bool multiple_functions;
extern size_t current_stack_offset;
#endif
