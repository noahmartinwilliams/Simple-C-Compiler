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

	if (x==100)
		return 0;
	else
		return 1;
}
