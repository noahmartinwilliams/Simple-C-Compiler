int main()
{
	int x=0;
	label:
		x=x+1;
		if (x<100)
			goto label;
	if (x==100)
		return 0;
	else
		return 1;
}
