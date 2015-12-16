int main()
{
	int i=1, x=1;

	int y=i^x;
	if (y!=0)
		return 1;
	
	x=0;
	y=i^x;
	if (y!=1)
		return 2;

	return 0;
}
