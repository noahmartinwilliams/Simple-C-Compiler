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

void test3()
{
	int x=0;
	x^=1;
	if (x!=1)
		exit(5);
	
	x^=1;
	if (x!=0)
		exit (6);
}

void test4()
{
	int x=5;
	x%=4;
	if (x!=1)
		exit(7);
}

void test5()
{
	int x=0;
	x+=1;
	if (x!=1)
		exit(8);
}

int main()
{
	test1();
	test2();
	test3();
	test4();
	test5();
	return 0;
}
