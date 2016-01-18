int main()
{
	int x=1, y=1;
	if (x && y) {
		x=0;
		if (x && y)
			return 2;
		x=1;
		y=0;

		if (x && y)
			return 3;

		return 0;
	} else
		return 1;
}
