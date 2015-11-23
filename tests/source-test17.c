int main()
{
	int x=0;
	while (x!=100) {
		if (x==99) {
			x=x+1;
			continue;
		}
		if (x==100) {
			return 1;
		}
		x=x+1;
	}

	return x;
}
