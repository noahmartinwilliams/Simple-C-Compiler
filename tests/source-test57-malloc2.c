#include "stddef.h"
extern void* malloc(size_t size);
extern void free(void* i);

int main()
{
	int **i=malloc(sizeof(int*));
	*i=malloc(sizeof(int));
	**i=1;

	if (**i==1) {
		free(*i);
		free(i);
		return 0;
	} else {
		free(*i);
		free(i);
		return 1;
	}
}
