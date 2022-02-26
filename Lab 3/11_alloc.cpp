#include "alloc.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

char* p;
static const int arr_size = PAGESIZE/MINALLOC;
int starts[arr_size];
int ends[arr_size];

void loop()
{
	int i = 0; 
	while(i < arr_size)
	{
		starts[i] = ends[i] = 0;
		i++;
	}
}
int increment(int a, int plus = 1)
{
	a = a + plus;
	return a;
}
int decrement(int a, int min = 1)
{
	a = a - min;
	return a;
}

int init_alloc()
{
	p = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
	if (p == (void *) -1)
		return -1;

	loop();

	return 0;
}

int cleanup() 
{
	loop();

	return munmap(p, PAGESIZE);
}

char *alloc(int size) 
{
	int sdM = size % MINALLOC;
	int size_units = size / MINALLOC;

	int state, probable_start, free_count, i;

	if (sdM != 0)
		return NULL;

	state = probable_start = free_count = i = 0;
	
	while(i < arr_size) 
	{
		state = increment(state,starts[i]);

		if (state == 0) 
		{
			free_count = increment(free_count);
		}
		if (state != 0)
		{
			probable_start = increment(i);
			free_count = 0;
		}

		if (free_count == size_units) 
		{
			starts[probable_start] = ends[i] = 1;

			return p + probable_start*MINALLOC ;
		}

		state = decrement(state,ends[i]);

		i++;
	}
	return NULL;

}

void dealloc(char * addr)
{
	char * temp = p;

	int flag = 0;

	for (int i = 0; i < arr_size; i++,temp += MINALLOC) 
	{
		if (temp == addr)
		{
			starts[i] = 0;
			flag = 1;
		}

		if (flag == 1 && ends[i] == 1)
		{
			ends[i] = 0;
			return;
		}
	}

	return;
}
