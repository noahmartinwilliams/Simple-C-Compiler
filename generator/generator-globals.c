#include <stdlib.h>
#include "types.h"
#include "stack.h"

int unique_num=0;
int depth=0;
bool in_main=false;
size_t word_size=4;
size_t int_size=4;

struct stack_t *loop_stack=NULL;
