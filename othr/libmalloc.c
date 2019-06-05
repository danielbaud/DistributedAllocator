#define _GNU_SOURCE
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include "libmalloc.h"

// Static variable to keep track of the mapped memory
static struct blk *map = NULL;


__attribute__((__visibility__("hidden")))
struct blk *getmap(size_t pagesize, size_t size, unsigned page)
{
  while (size > pagesize)
    pagesize += pagesize;
  struct blk *block = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, 
                           MAP_ANONYMOUS | MAP_SHARED | MAP_FILE, -1, 0);
  block->size = size;
  block->state = ALLOCATED;
  block->page = page;
  void *temp = block;
  char *t = temp;
  t += block->size;
  temp = t;
  block->next = temp;
  block->next->page = page;
  block->next->next = NULL;
  block->next->size = pagesize - block->size;
  block->next->state = FREE;
  return block;
}


__attribute__((__visibility__("default")))
void *malloc(size_t size)
{
  size_t pagesize = sysconf(_SC_PAGESIZE);
  
  while (size % sizeof (size_t))
    size++;
  size += sizeof (struct blk);
  struct blk *p = map;
  struct blk *par = p;
  while (p && p->size > 0 &&(p->size < size || p->state == ALLOCATED))
  {
    par = p;
    p = p->next;
  }
  
  if (!p)
  {
    if (!map)
    {
      map = getmap(pagesize, size, 0);
      return map + 1;
    }
    else
    {
      par->next = getmap(pagesize, size, par->page + 1);
      return par->next + 1;
    }
  }
  p->state = ALLOCATED;
  if (p->next)
  {
    void *temp = p;
    char *man = temp;
    man += size + sizeof (struct blk);
    temp = man;
    struct blk *ne = p->next;
    p->next = temp;
    p->next->next = ne;
    p->next->size = p->size - size - sizeof (struct blk);
    p->next->page = p->page;
    p->next->state = FREE;
    p->size -= p->next->size;
  }
  return p + 1;
}


__attribute__((__visibility__("default")))
void *calloc(size_t number, size_t size)
{
  //size_t pagesize = sysconf(_SC_PAGESIZE);
  size_t si = number * size;
  if (!si)
    return NULL;
  void *ptr = malloc(si);
  return memset(ptr, 0, si);
}


__attribute__((__visibility__("default")))
void free(void *ptr)
{
  //size_t pagesize = sysconf(_SC_PAGESIZE);
  if (!ptr)
    return;
  struct blk *p = map;
  struct blk *par = p;
  while (p + 1 != ptr)
  {
    par = p;
    p = p->next;
  }
  p->state = FREE;
  if (p->next && p->next == FREE && p->page == p->next->page)
  {
    p->size += p->next->size;
    p->next = p->next->next;
  }
  if (par->state == FREE)
  {
    par->size += par->next->size;
    par->next = par->next->next;
  }
}


__attribute__((__visibility__("default")))
void *realloc(void *ptr, size_t size)
{
  if (!ptr)
    return malloc(size);
  
  if (!size)
  {
    free(ptr);
    return NULL;
  }
  
  struct blk *p = map;
  while (p + 1 != ptr)
    p = p->next;
  
  size_t s = p->size;
  
  free(ptr);
  
  void *ret = malloc(size);
  size_t min = size > s ? s : size;
  
  return memcpy(ret, ptr, min);
}
