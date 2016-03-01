extern void exit(int status);
void _long()
{
	if (sizeof(long int) <= sizeof(int))
		exit(1);
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

int main()
{
	_long();
	_unsigned();
	_signed();
}
