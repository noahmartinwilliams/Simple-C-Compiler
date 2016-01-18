extern int b();

int a()
{
	return 2;
}

int main()
{
	int x=a();

	int y=b();
	if (y==3) {
		if (x==2)
			return 0;
		else
			return 1;
	} else
		return 2;
}
