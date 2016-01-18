int f(int a)
{
	return a+1;
}

int main()
{
	int x=f(f(1));
	if (x==3) {
		x=f(f(x)+3);

		if (x==8)
			return 0;
		else
			return 2;
	} else
		return 1;
}
