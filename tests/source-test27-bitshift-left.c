int main()
{
	int x=2;
	x=x<<1;
	if (x!=4)
		return 1;
	
	x=2;
	x=x<<2;
	if (x!=8)
		return 2;
	
	x=2;
	x=(x<<3);
	if (x!=16)
		return 3;

	return 0;
}
