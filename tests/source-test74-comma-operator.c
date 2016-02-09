int main()
{
	int i;
	i=(1,2);
	if (i==1)
		return 1;
	else {
		if (i!=2)
			return 2;

		int x;
		i=(x=1, 2);
		if (i!=2)
			return 3;

		if (x!=1)
			return 4;
	}

	return 0;
}
