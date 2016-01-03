extern int printf(char *msg, ...);

int main()
{
	int x=0;
	for (; x<10; x++) {
		printf("%d", x);
	}
	printf("\n");
	return 0;
}
