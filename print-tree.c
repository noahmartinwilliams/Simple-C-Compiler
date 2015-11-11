#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

void print_tree(void (*printer) (char*, void*), void *tree, char *str, off_t a, off_t b)
{
	char *s=NULL;
	printf("%s|_", str);
	printer(str, tree);
	printf("\n");
	asprintf(&s, "%s |", str);
	if (*((void**) (tree+a))!=NULL) {
		print_tree(printer, *((void**) (tree+a)), s, a, b);
	}
	free(s);
	asprintf(&s, "%s ", str);
	if (*((void**) (tree+b))!=NULL) {
		print_tree(printer, *((void**) (tree+b)), s, a, b);
	}
	free(s);
}
