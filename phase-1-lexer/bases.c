#include <string.h>

int hex2i(char *str)
{
	int i=0;
	int x;
	for (x=0; str[x]!='\0'; x++) {
		i*=16;
		switch(str[x]) {
		case '0':
			i+=0;
			break;
		case '1':
			i+=1;
			break;
		case '2':
			i+=2;
			break;
		case '3':
			i+=3;
			break;
		case '4':
			i+=4;
			break;
		case '5':
			i+=5;
			break;
		case '6':
			i+=6;
			break;
		case '7':
			i+=7;
			break;
		case '8':
			i+=8;
			break;
		case '9':
			i+=9;
			break;
		case 'a':
		case 'A':
			i+=10;
			break;
		case 'b':
		case 'B':
			i+=11;
			break;
		case 'c':
		case 'C':
			i+=12;
			break;
		case 'd':
		case 'D':
			i+=13;
			break;
		case 'e':
		case 'E':
			i+=14;
			break;
		case 'f':
		case 'F':
			i+=15;
			break;
		}
	}

	return i;
}

int oct2i(char *str)
{
	int ret=0;
	int mul=1;
	int len=strlen(str);
	int x=len-1;
	for (; x>=0; x--) {
		switch(str[x]) {
		case '0':
			ret+=mul*0;
			break;
		case '1':
			ret+=mul*1;
			break;
		case '2':
			ret+=mul*1;
			break;
		case '3':
			ret+=mul*1;
			break;
		case '4':
			ret+=mul*1;
			break;
		case '5':
			ret+=mul*1;
			break;
		case '6':
			ret+=mul*1;
			break;
		case '7':
			ret+=mul*1;
			break;
		}
		mul*=8;
	}
	return ret;
}
