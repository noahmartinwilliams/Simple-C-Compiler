#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "stack.h"
#include "generator/generator-types.h"

struct stack_t *loop_stack=NULL;
int unique_num=0;
int depth=0;
bool in_main=false;
size_t word_size=4;
size_t int_size=4;
size_t pointer_size=8;
size_t char_size=1;

bool multiple_functions=false;

