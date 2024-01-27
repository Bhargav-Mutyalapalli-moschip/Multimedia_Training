#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*int main()
{
	char str[100]="pranav";
	char ptr[200]="RAM";
	char *sptr;
	sptr=str;
	printf("The str:%s    The ptr:%s\n",str,ptr);
	printf("/"RAM/"");
}*/
int main() {
    const char *text = "Hello, world!";

    printf("String with double quotes: \"%s\"\n", text);
    printf("\"String\"");

    return 0;
}

