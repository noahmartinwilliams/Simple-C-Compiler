enum b_t { foo, bar };
void f()
{
	enum a { b, c } d;
	d=b;
}

int main()
{
	int a=0;
	enum b_t zag;
	zag=foo;
	f();
	return a;
}
