#include<stdio.h>
#define FORMAT "NV12"
int main()
{
	char str[10];
	scanf("%s",str);
	switch(str)
	{
		case FORMAT :
			printf("It is invoked\n");
			break;
		default:
			printf("It is not invoked\n");
			break;
	}
}



