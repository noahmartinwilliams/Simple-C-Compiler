int main()
{
	int x=0, y;
	y=x++;
	if (x==1 && y==0)
		return 0;
	else if (x!=1 && y==0)
		return 1;
	else if (x==1 && y!=0)
		return 2;
}
