#include<stdio.h>
#define GRAPH_LENGTH 78
int main()
{
	int i;
	long int position=2846526000;
	long int duration=52250000000;
	i = (int)(GRAPH_LENGTH * (double)position / (double)(duration + 1));
	printf("i:%d\n",i);

}
