#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct node
{
	int a;
	char c;
};
int main()
{
	struct node *leak;
	leak=(struct node *)malloc(1*sizeof(struct node));
	leak->a=25;
	printf("the val:%d\n",leak->a);
}


