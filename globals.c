#include <stdlib.h>
#include "types.h"
#include <stdbool.h>

bool print_trees=false;
int yydebug=0;

char *current_line_str=NULL;
int current_line=0;
int current_char=0;
char *current_file=NULL;
char *current_function=NULL;

bool evaluate_constants=true;

struct type_t **types=NULL;
int num_types=0;

struct var_t **vars=NULL;
int num_vars=0;

struct func_t **funcs=NULL;
int num_funcs=0;

int scope_depth=0;

#ifdef HAIKU_EASTER_EGG
int number_of_syllables=0;
int current_haiku_line=1;
#endif 
