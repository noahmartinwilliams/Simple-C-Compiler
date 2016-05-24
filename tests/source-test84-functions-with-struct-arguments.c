int a()
{
	return 1;
}

struct c_t {
	int i;
	int *i2;
	char c;
};

int func(struct c_t d)
{
	d.i=0;
	d.i2=0;
	d.c='a';
	return 1;
}

int main()
{
	int b=a();
	if (b==1) {
		struct c_t f;
		f.i=2;
		f.i2=1;
		f.c='b';
		b=func(f);
		if (b!=1)
			return 2;
		return 0;
	} else
		return 1;
}
