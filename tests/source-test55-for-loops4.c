extern int printf(char *msg, ...);
int main()
{
	int x=0;
	for (; x<10;) {
		printf("%d", x);
		x=x+1;
	}
	printf("\n");
	return 0;
}
