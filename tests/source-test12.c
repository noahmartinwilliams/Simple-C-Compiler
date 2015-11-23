int main()
{
	int x=0, y;
	{
		int x=2;
		y=x;
	}
	{
		int x=3;
		y=x;
	}

	return y;
}
