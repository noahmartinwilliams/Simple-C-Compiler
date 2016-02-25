enum b_t { foo, bar };
enum c_t { fooo=1, baar };
void f()
{
	enum a { b, c } d;
	d=b;
}

int main()
{
	int a=0;
	enum b_t zag;
	enum c_t biz;
	biz=fooo;
	if (((int) biz) != 1)
		return 1;
	zag=foo;
	f();
	return a;
}
