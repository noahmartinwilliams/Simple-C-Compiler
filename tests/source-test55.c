extern int printf(char *msg, ...);
int main()
{
	int x;
	for (x=0; x<10;) {
		printf("%d", x);
		x=x+1;
	}

	printf("\n");
	return 0;
}
