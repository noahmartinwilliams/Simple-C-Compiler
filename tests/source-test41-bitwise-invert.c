int main()
{
	int x=0;
	int y=0;
	x=~x;
	for (y=0; y<32; y++) {
		if (!(x&(1<<y)))
			return 1;
	}

	return 0;
}
