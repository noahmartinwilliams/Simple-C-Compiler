#include "stddef.h"
extern void* calloc(int n, size_t size);
extern void free(void* ptr);
extern void exit(int status);
extern int printf(char *msg, ...);

void a()
{
	int *i=calloc(5, sizeof(int));
	i[3]=1;
	if (i[3]!=1) {
		free(i);
		exit(1);
	}
	free(i);
}

void b()
{
	int a[3];
	a[0]=1;
	if (a[0]!=1)
		exit(2);
}
int main()
{
	a();
	b();
	return 0;
}
