void f()
{
	enum a { b, c } d;
	d=b;
}

int main()
{
	int a=0;
	f();
	return a;
}
