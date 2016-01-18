extern int* malloc(size_t s);
extern void free(int* f);
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
