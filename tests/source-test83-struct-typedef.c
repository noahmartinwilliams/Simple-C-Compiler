typedef struct {
	int b;
	char c;
} a_t;

int main()
{
	a_t a;
	a.b=1;
	a.c='\\';
	if (a.c!='\\')
		return 1;

	if (a.b!=1)
		return 2;

	return 0;
}
