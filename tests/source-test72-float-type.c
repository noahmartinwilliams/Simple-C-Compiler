extern int printf(char *msg, ...);
extern void exit(int status);
void subtraction()
{
	float f=1.0;
	f=f-1.0;

	if (f < 0.1) {
		f=1.0;
		if (0.5 < f) {
			return; 
		} else {
			printf("subtraction error: 0.5<f failed\n");
			printf("subtraction error: (0.5<f)==%d\n", 0.5<f);
			printf("subtraction error: f = %f\n", f);
			printf("subtraction error: f should be 1.0\n");
			exit(2);
		}
	} else {
		printf("subtraction error: (1.0-1.0) < 0.1 failed\n");
		printf("subtraction error: (1.0-1.0) = %f\n", f);
		printf("subtraction error: ((1.0-1.0) < 0.1) == %d\n", f < 0.1);
		exit(1);
	}
}

void addition()
{
	float f=0.0;
	f=f+1.0;
	if (f < 0.9) { 
		printf("addition error: 0.0+1.0 < 0.9\n");
		printf("addition error: 0.0+1.0= %f\n", f);
		exit(4);
	}
	return;
}

void greater_than()
{
	float f=2.0;
	if ( f > 1.0) {
		if (0.0 > 2.1) {
			printf("greater_than error: 0.0 > 2.1\n");
			printf("greater_than error: (0.0 > 2.1) == %d\n", (0.0 > 2.1));
			exit(9);
		} else
			return;
	} else {
		printf("f set to 2.0\n");
		printf("greater_than error: (f > 1.0)==%d\n", f>1.0);
		printf("f = %f\n", f);
		exit (5);
	}
}

void greater_than_or_equal_to()
{
	float f=2.0;
	if (1.0 >= f) {
		printf("greater_than_or_equal_to error: 1.0 >= 2.0\n");
		exit(6);
	} else
		return;
}

void multiplication()
{
	float f=2.0;
	f*=2.0;
	if ( (f-4.0) >= 0.1) {
		printf("multiplication error: ((2.0*2.0)-4.0) >= 0.1\n");
		printf("multiplication error: 2.0*2.0 = %f\n", f);
		exit(7);
	} else
		return;
}

void division()
{
	float f=2.0;
	f=f/2.0;
	if (f>1.1 || f<0.9) {
		printf("division error: (2.0/2.0)!=1.0\n");
		printf("division error: (2.0/2.0)==%f\n", f);
		printf("((2.0/2.0) > 2.1) == %d\n", f>1.1);
		printf("((2.0/2.0) < 2.1) == %d\n", f<1.1);
		exit(8);
	}
}

float _global;
void global()
{
	_global=1.0;
	_global+=1.0;
	if (_global < 1.9 || _global > 2.1) {
		printf("global error: global supposed to be 2, is actually: %f\n", _global);
		exit(10);
	}
	return;
}

void convert()
{
	float f=1.0;
	int i=2;
	i=(int) f;
	if (i!=1 && i!=0) {
		printf("convert error: i is not correct: %d\n", i);
		exit(11);
	}
	return;
}

int main()
{
	subtraction();
	addition();
	greater_than();
	greater_than_or_equal_to();
	multiplication();
	division();
	global();
	convert();
	return 0;
}
