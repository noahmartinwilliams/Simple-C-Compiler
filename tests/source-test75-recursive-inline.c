inline int a(int c);
inline int b(int c)
{
	return a(c);
}
inline int a(int c)
{
	return b(c);
}

int main()
{
	return a(1);
}
