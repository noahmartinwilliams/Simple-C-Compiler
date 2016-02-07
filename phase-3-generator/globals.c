#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "stack.h"
#include "generator/types.h"

struct stack_t *loop_stack=NULL;
int unique_num=0;
int depth=0;
bool in_main=false;
size_t word_size;
size_t int_size;
size_t pointer_size;
size_t long_size;
size_t char_size;
size_t float_size;
size_t byte_size;

bool multiple_functions=false;

