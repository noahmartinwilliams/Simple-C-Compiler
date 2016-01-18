int main()
{
	int x=0;
	while (x<100) {
		if (x==50)
			break;
		else
			x=x+1;
	}
	if (x==50)
		return 0;
	else
		return 1;
}
