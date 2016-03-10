#include <stdio.h>
#include "types.h"
#include "globals.h"
#include "generator/backend-exported.h"

char* generate_global_string(FILE *fd, char *str)
{
	return make_global_string(fd, str);
}

void setup_generator()
{
	setup_backend();
	setup_types();
}
