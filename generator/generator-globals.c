#include <stdlib.h>
#include "types.h"
#include "stack.h"

struct reg_t **regs=NULL;
int num_regs=0;
int unique_num=0;
int depth=0;
bool in_main=false;
size_t word_size=4;
size_t int_size=4;
size_t pointer_size=8;
size_t char_size=1;
struct stack_t *loop_stack=NULL;

bool multiple_functions=false;
size_t current_stack_offset=0;
