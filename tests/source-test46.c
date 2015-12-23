int main()
{
	union a_t {
		int i;
		char c;
	} b;

	b.i=66;

	if (b.c!='A') {
		b.i--;
		if (b.c=='A')
			return 0;
		else
			return 2;
	} else
		return 1;
}
