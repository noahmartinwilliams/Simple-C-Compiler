struct a_t {
	int i;
	char c;
};

int main()
{
	if (__alignof__(struct a_t)!=0) 
		return 0;
	else
		return 1;
}
