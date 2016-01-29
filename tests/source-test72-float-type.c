extern int printf(char *msg, ...);
int main()
{
	float f=1.0;

	if ((f-1.0) < 0.1) {
		if (f>0.5) {
			if (0.5 > f) {
				printf("error: 0.5 > 1.0\n");
				return 3;
			} else
				return 0;
		} else {
			printf("error: 1.0>0.5 failed\n");
			return 2;
		}
	} else {
		printf("error (1.0-1.0) < 0.1 failed\n");
		return 1;
	}
}
