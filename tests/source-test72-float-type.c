extern int printf(char *msg, ...);
extern void exit(int status);
void subtraction()
{
	float f=1.0;

	if ((f-1.0) < 0.1) {
		if (f>0.5) {
			if (0.5 > f) {
				printf("error: 0.5 > 1.0\n");
				exit(3);
			} else
				return; 
		} else {
			printf("error: 1.0>0.5 failed\n");
			exit(2);
		}
	} else {
		printf("error (1.0-1.0) < 0.1 failed\n");
		exit(1);
	}
}

void addition()
{
	float f=0.0;
	f=f+1.0;
	if (f < 0.9) {
		printf("0.0+1.0 < 0.9\n");
		printf("f: %f\n", f);
		exit(4);
	}
	return;
}

int main()
{
	subtraction();
	addition();
	return 0;
}
