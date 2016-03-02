int main()
{
	struct a_t {
		int a;
		char c;
	} b;

	b.a=1;
	b.c='a';

	if (b.a==1) {
		struct {
			int i;
			char d;
		} f;
		f.i=9;
		f.d='a';
		if (f.i!=9)
			return 2;
		if (f.d!='a')
			return 3;
		return 0;
	} else
		return 1;
}
