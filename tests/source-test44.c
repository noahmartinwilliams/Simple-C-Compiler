extern void puts(char *str);
int main()
{
	int x;
	x=0;
	do {
		puts("here");
	} while (x!=0) ;

	x=99;
	do {
		puts("beer");
		x--;
	} while (x>0);

	return 0;
}
