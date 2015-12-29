int main()
{
	struct a_t {
		int a;
		char c;
	} b;

	b.a=1;
	b.c='a';

	if (b.a==1)
		return 0;
	else
		return 1;
}
