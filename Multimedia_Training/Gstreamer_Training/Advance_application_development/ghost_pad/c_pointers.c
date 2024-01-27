#include<stdio.h>
int main()
{
	/*int i=10;
	void *ptr=&i;
	printf("The value:%d",(int*)ptr);*/
	/*char *p;
	p="c programming";
	printf("The str:%s\n",p);
	printf("The str:%c\n",*&*p);*/
	void *vp;
	char ch=74,*cp="JACK";
	int j=65;
	vp=&ch;
	printf("The ch:%c\n",*(char*)vp);
	vp=&j;
	printf("The j:%c\n",*(int*)vp);
	vp=cp;
	printf("The cp:%s",(short int*)vp+2);

}
