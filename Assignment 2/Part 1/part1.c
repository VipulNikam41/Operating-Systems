#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 

int o = 0, id = 0, a =1;
char str[] = "Hello World";

void forkPrint(int i, int id, int length)
{
	if(i <= length)
	{
		id = fork();
	}
	if(i <= length && id > o)
	{
		printf("%c %d\n", str[i], id);
		sleep(rand()%4 + a);
		forkPrint(i+a, id,length);
	}
	return;  
}

int main()
{
	setbuf(stdout, NULL);
	forkPrint(o, id, 10);
	return 0; 
}