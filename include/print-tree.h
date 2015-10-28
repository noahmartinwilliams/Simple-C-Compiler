#ifndef __PRINT_TREE_H__
#define __PRINT_TREE_H__
#include <stddef.h>
#include <stdlib.h>


extern void print_tree(char* (*printer) (char*, void*), void *tree, char *str, off_t a, off_t b);
#endif
