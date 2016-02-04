#ifndef __GENERATOR_GLOBALS_H__
#define __GENERATOR_GLOBALS_H__
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "generator/types.h"
#ifndef IN_BACKEND
#include "generator/backend-exported.h"
#endif

extern int depth;
extern size_t word_size;
extern size_t int_size;

extern int unique_num;
extern struct stack_t *loop_stack;
extern size_t pointer_size;
extern size_t char_size;
extern size_t long_size;
extern bool multiple_functions;
extern size_t current_stack_offset;
extern size_t float_size;

extern bool in_main;

#endif
