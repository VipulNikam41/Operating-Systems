#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define SAMPLES 20

#define HIGH 20
#define FREQ 80

typedef struct pte 
{
  int id;
  int present;
  struct pte *next;
  struct pte *prev;  
} pte;


void init_sequence(int *sequence, int refs, int pages)  
{

  int high = (int)(pages*((float)HIGH/100));
  if(high < 2)
  {
    high = 2;
  }
  
  /* dummy value */
  int prev = pages; 
  
  for(int i = 0; i<refs; i++) 
  {
    if(rand()%100 < FREQ) 
    {    
      int rnd;
      do 
      {
      	/* we want to prevent the same page being picked again */
      	rnd = rand()%high;
      } while (rnd == prev);
      prev = rnd;
      sequence[i] = rnd;          
    } 
    else 
    {
      int rnd;
      do 
      {
	       rnd = high + rand()%(pages - high);
      } while (rnd == prev);
      prev = rnd;
      sequence[i] = rnd;
    }
  }
}

void clear_page_table(pte *page_table, int pages) 
{
    for(int i=0; i < pages; i++) 
    {
      page_table[i].id = i;
      page_table[i].present = 0;
      page_table[i].next = NULL;
      page_table[i].prev = NULL;            
    }
}

int simulate(int *sequence, pte *page_table, int refs, int frames, int pages) {

  int hits = 0;
  int allocated = 0;

  pte *first = NULL;
  pte *last = NULL;  
  
  
  for(int i = 0; i < refs; i++) 
  {
    int next = sequence[i];
    pte *entry = &page_table[next];

    if(entry->present == 1) 
    {
      hits++;
      if(entry->next != NULL) 
      {
      	/* unlink the entry and place it last */

      	if(first == entry) 
        {
      	  first = entry->next;
      	} 
        else 
        {
      	  entry->prev->next = entry->next;
      	}
      	entry->next->prev = entry->prev;

      	entry->prev = last;
      	entry->next = NULL;

      	last->next = entry;
      	last = entry;
      }
    } 
    else 
    {
      if(allocated < frames) 
      {
      	allocated++;
      	entry->present = 1;
      	entry->prev = last;
      	entry->next = NULL;

      	if(last != NULL) 
        {
      	  last->next = entry;
      	}
      	if(first == NULL) 
        {
      	  first = entry;
      	}
      	last = entry;
      } 
      else 
      {
      	pte *evict;

      	assert(first != NULL);

      	evict = first;
      	first = evict->next;

      	evict->present = 0;

      	entry->present = 1;
      	entry->prev = last;
      	entry->next = NULL;

      	if(last != entry) 
        {
      	  last->next = entry;
      	}
      	if(first == NULL) 
        {
      	  first = entry;
      	}
      	last = entry;
      }
    }
  }
  return hits;
}


int main(int argc, char *argv[]) 
{
  /* the number of references, should be a command line argument */
  int refs = 100000;

  /* the number of pages of a process, should be a command line argument */
  int pages = 100;
  
  int *sequence = (int*)malloc(refs*sizeof(int)); 


  init_sequence(sequence, refs, pages);

  pte *page_table = (pte *)malloc(pages*sizeof(pte));

  printf("# This is a benchmark of LRU replacement policy\n");
  printf("# %d page references\n", refs);    
  printf("# %d pages \n", pages);  
  printf("#\n#\n#frames\tratio\n");    
  
  /*  frames is the size of the memory in frames */
  int frames;

  int incr = pages/SAMPLES;
  
  for(frames = incr; frames <= pages; frames += incr) 
  {
    clear_page_table(page_table, pages);

    int hits =  simulate(sequence, page_table, refs, frames, pages);

    float ratio = (float)hits/refs;

    printf("%d\t%.2f\n", frames, ratio);
  }
  return 0;
}
