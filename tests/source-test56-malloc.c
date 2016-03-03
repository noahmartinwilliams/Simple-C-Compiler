#include "stddef.h"
extern void* malloc(size_t s);
extern void free(void* f);
int main()
{
	int *s;
	s=malloc(sizeof(int));
	*s=1;
	if (*s==1) {
		free(s);
		return 0;
	} else {
		return 1;
	}
		
}
