int main()
{
	int a;
	a=1;
	int *i;
	i= &a;
	int **i2;
	i2=&i;
	if (**i2==1)
		return 0;
	else
		return 1;
}
