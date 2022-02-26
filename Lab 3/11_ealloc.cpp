#include "ealloc.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

char* p1 = NULL;
char* p2 = NULL;
char* p3 = NULL;
char* p4 = NULL;

static const int arr_size = PAGESIZE/MINALLOC;

int start1[arr_size], start2[arr_size], start3[arr_size], start4[arr_size];
int end1[arr_size], end2[arr_size], end3[arr_size], end4[arr_size];

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

void cleanup() 
{
	int i = 0;
	while(i < arr_size)
	{
		start1[i] = start2[i] = start3[i] = start4[i] = 0;
		end1[i] = end2[i] = end3[i] = end4[i] = 0;

		i++;
	}
}

void init_alloc()
{
	cleanup();
	return;
}

char *alloc(int size) 
{
	int sdM = size % MINALLOC;

	if (sdM != 0)
	{
		return NULL;
	}

	int size_units = size / MINALLOC;
	
	if (p1 == NULL) 
	{
		p1 = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
		if (p1 == (void *) -1)
			return NULL;
	}

	int state, probable_start, free_count, i;

	state = probable_start = free_count = i = 0;

	while(i < arr_size) 
	{
		state = increment(state, start1[i]);

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
			start1[probable_start] = end1[i] = 1;

			return p1 + probable_start*MINALLOC;
		}

		state = decrement(state,end1[i]);

		i++;
	}

	if (p2 == NULL) 
	{
		p2 = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		if (p2 == (void *) -1)
			return NULL;
	}

	state = probable_start = free_count = i = 0; 
	while(i < arr_size) 
	{
		state = increment(state, start2[i]);

		if (state == 0)
		{
			free_count = increment(free_count);
		}
		else
		{
			probable_start = increment(i);
			free_count = 0;
		}

		if (free_count == size_units)
		{
			start2[probable_start] = end2[i] = 1;
			return p2 + probable_start*MINALLOC;
		}
		state = decrement(state,end2[i]);

		i++;
	}

	if (p3 == NULL)
	{
		p3 = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
		if (p3 == (void *) -1)
			return NULL;
	}
	state = probable_start = free_count = 0;

	i = 0;
	while(i < arr_size) 
	{
		state = increment(state, start3[i]);

		if (state == 0) 
		{
			free_count = increment(free_count);
		}
		else
		{
			probable_start = increment(i);
			free_count = 0;
		}

		if (free_count == size_units) 
		{
			start3[probable_start] = end3[i] = 1;

			return p3 + probable_start*MINALLOC;
		}

		state = decrement(state,end3[i]);

		i++;
	}

	if (p4 == NULL)
	{
		p4 = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
		if (p4 == (void *) -1)
			return NULL;
	}

	state = probable_start = free_count = i = 0;
	while(i < arr_size) 
	{
		state = increment(state, start4[i]);

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
			start4[probable_start] = end4[i] = 1;

			return p4 + probable_start*MINALLOC;
		}
		state = decrement(state,end4[i]);

		i++;
	}
	return NULL;

}

void dealloc(char * addr)
{
	int flag = 0;
	
	if (p1 == NULL) 
		return;
	char * temp = p1;

	for (int i = 0; i < arr_size; i++,temp += MINALLOC) 
	{
		if (temp == addr) 
		{
			start1[i] = 0;
			flag = 1;
		}

		if (flag == 1 && end1[i] == 1)
		{
			end1[i] = 0;
			return;
		}
	}

	if (p2 == NULL) 
		return;

	temp = p2;
	flag = 0;
	for (int i = 0; i < arr_size; i++,temp += MINALLOC) 
	{
		if (temp == addr) 
		{
			start2[i] = 0;
			flag = 1;
		}
		if (flag == 1 && end2[i] == 1)
		{
			end2[i] = 0;
			return;
		}
	}

	if (p3 == NULL) 
		return;

	temp = p3;
	flag = 0;
	for (int i = 0; i < arr_size; i++,temp += MINALLOC)
	{
		if (temp == addr)
		{
			start3[i] = 0;
			flag = 1;
		}

		if (flag == 1 && end3[i] == 1)
		{
			end3[i] = 0;
			return;
		}

	}

	if (p4 == NULL) 
		return;

	temp = p4;
	flag = 0;
	for (int i = 0; i < arr_size; i++,temp += MINALLOC) 
	{
		if (temp == addr) 
		{
			start4[i] = 0;
			flag = 1;
		}
		if (flag == 1 && end4[i] == 1)
		{
			end4[i] = 0;
			return;
		}
	}
	return;
}