const int x=1;

int main()
{
	if (x==1) {
		#if DEBUG == 1
			x=2;
		#endif
		const int y=0xaf;
		if (y==175) {
			const int z=010;
			if (z==8)
				return 0;
			else
				return 3;
		} else
			return 2;
	} else
		return 1;
}
