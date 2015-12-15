int main()
{
	int x=1;
	if (x&1==1) {
		if (x&0==1)
			return 1;
		else
			return 0;
	} else 
		return 2;
}
