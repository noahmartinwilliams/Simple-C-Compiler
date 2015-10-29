#include <stdlib.h>
#include "types.h"
int current_line=0;
int current_char=0;
char *current_file=NULL;
int yydebug=0;

struct type_t **types=NULL;
int num_types=0;
