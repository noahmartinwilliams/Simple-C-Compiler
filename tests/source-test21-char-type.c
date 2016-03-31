int main()
{
	char c='a';
	char d='\\';
	if (d!='\\')
		return 1;

	char e='\n';
	if (e!='\n')
		return 2;

	char f='\0';
	if (f!='\0')
		return 3;
	return 0;
}
