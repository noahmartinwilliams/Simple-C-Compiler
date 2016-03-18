#include <stdio.h>

int main()
{
	printf("sizeof(char): %zd\n", sizeof(char));
	printf("sizeof(short int): %zd\n", sizeof(short int));
	printf("sizeof(int): %zd\n", sizeof(int));
	printf("sizeof(long int): %zd\n", sizeof(long int));
	printf("sizeof(long long int): %zd\n", sizeof(long long int));
	printf("sizeof(double): %zd\n", sizeof(double));
	printf("sizeof(void*): %zd\n", sizeof(void*));
	printf("sizeof(float): %zd\n", sizeof(float));
}
