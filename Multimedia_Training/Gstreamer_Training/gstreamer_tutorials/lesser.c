#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{
	int fd=open("main.c",O_RDONLY | O_WRONLY);
	printf("fd:%d\n",fd);
}

