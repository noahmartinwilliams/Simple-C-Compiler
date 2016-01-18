int f(int a)
{
	return a-1;
}

int main()
{
	int (*ptr) (int a);
	ptr=f;
	return ptr(1);
}
