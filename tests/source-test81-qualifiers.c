extern void exit(int status);
extern int printf(char *msg, ...);
void _long()
{
	if (sizeof(long int) <= sizeof(int))
		exit(1);

	int x=-1;
	long int y=(long int) x;
	if (y>=0)
		exit(5);
}

void _unsigned()
{
	unsigned int x=~ ((unsigned int) 0);
	x=~((unsigned int) 0);
	if (x < 0)
		exit(2);
}

void _signed()
{
	signed int x=~ ((signed int) 0);
	x=~((signed int) 0);
	if (!(x < 0))
		exit(3);
}

void _short()
{
	if (sizeof(short int) >= sizeof(int))
		exit(4);

	return;
}

int main()
{
	_long();
	_unsigned();
	_signed();
	_short();
}
