int main()
{
	int x=1;
	int y=x ? 1 : 0;
	if (y==1) {
		x=0;
		y= x ? 1 : 0;
		if (y==0)
			return 0;
		else
			return 2;
	} else
		return 1;
}
