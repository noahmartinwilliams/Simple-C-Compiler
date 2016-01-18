extern void exit(int status);

void test1()
{
	int x=0;
	x|=1;
	if (x!=1)
		exit(1);
	
	x=1;
	x|=0;
	if (x!=1)
		exit(2);
}

void test2()
{
	int x=0;
	x&=1;
	if (x!=0)
		exit(3);
	
	x=1;
	x&=1;

	if (x!=1)
		exit(4);
}

int main()
{
	test1();
	test2();
	return 0;
}
