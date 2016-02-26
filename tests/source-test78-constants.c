const int x=1;

int main()
{
	if (x==1) {
		const int y=0xaf;
		if (y==175)
			return 0;
		else
			return 2;
	} else
		return 1;
}
