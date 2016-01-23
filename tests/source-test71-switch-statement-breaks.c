int main()
{
	int x=0;
	switch (x) {
		case 0:
			x=1;
			break;
		case 1:
		case 2:
			return 2;
	}

	if (x!=1)
		return 1;

	switch (x) {
		case 1:
		case 2:
			return 0;
		default:
			return 3;
	}
}
