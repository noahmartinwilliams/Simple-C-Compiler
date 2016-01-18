int main()
{
	int x=0;
	switch (x) {
	case 0:
		x++;
		switch(x) {
		case 1:
			return 0;
		case 0:
			return 1;
		}
	case 1:
		return 2;
	}
}
