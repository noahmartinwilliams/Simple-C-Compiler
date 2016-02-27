int f(int a)
{
	return a-1;
}

int main()
{
	#ifndef DEBUG
	int (*ptr) (int a);
	ptr=f;
	return ptr(1);
	#else
	int (*ptr) (int a);
	ptr=f;
	ptr(1, 2);
	return 0;
	#endif
}
