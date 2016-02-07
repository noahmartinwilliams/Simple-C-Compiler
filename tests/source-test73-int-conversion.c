extern int printf(char *msg, ...);
int main()
{
	int i=97;
	char c='a';
	if (i!=((int) c)) {
		printf("((int) 'a') = %d\n", (int) c);
		return 1;
	} else
		return 0;
}
