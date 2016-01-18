int main()
{
	int x=8;
	x=x>>1;
	if (x!=4)
		return 1;
	
	x=8;
	x=x>>2;
	if (x!=2)
		return 2;
	
	x=8;
	x=x>>3;
	if (x!=1)
		return 3;

	return 0;
}
