#include <stdlib.h>
#include "types.h"
#include <stdbool.h>

int current_line=0;
int current_char=0;
char *current_file=NULL;
int yydebug=0;

bool evaluate_constants=true;

struct type_t **types=NULL;
int num_types=0;
