#include "stddef.h"
extern void* calloc(int n, size_t size);
extern void free(void* ptr);

int main()
{
	int *i=calloc(5, sizeof(int));
	i[3]=1;
	if (i[3]!=1) {
		free(i);
		return 1;
	} else {
		free(i);
		return 0;
	}
}
