int main()
{
	int x;
	x=0;
	int y=++x;
	if (x==1 && y==1) {
		int *z;
		z=&x;
		y=++*z;
		if (y==2 && *z==2)
			return 0;
		else
			return 2;
	} else
		return 1;
}
