// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

#define MAXPAGES PHYSTOP/PGSIZE

#define GETINDEX(page)  (page >> PGSHIFT)
#define GETREFS(ref)    (ref & (~(1<<31)))
#define CHECKW(ref)     ((ref & (1<<31)) > 0)
#define SETW(ref)       (ref |= (1<<31))

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.
                   
// References of pages the first bit
// store the write permission
uint refs[MAXPAGES]; 

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
  memset(refs,0,sizeof(refs));
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}


// Return 1 if it is the unique page
// The zero case is for the freerane
uint delref(uint64 pa){
  uint index = GETINDEX(pa);

  // If it is the last page or we are initializing 
  // with freerange (0)
  if(GETREFS(refs[index]) <= 1){
    refs[index] = 0;
    return 1;
  }
  --refs[index]; 
  return 0;
}


// Add a reference to a page
// if write == 1 this means that original 
// page have the write permission
void addref(uint64 pa, uint write){
  uint index = GETINDEX(pa);
  refs[index]++;
  if(write)
    SETW(refs[index]);
}

uint is_writtable(uint64 pa){
  return CHECKW(refs[GETINDEX(pa)]);
}

uint getrefs(uint64 pa){
  return GETREFS(refs[GETINDEX(pa)]);
}


// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  if(delref((uint64)pa)){ 

    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    refs[GETINDEX((uint64)r)] = 1; // First reference
  }

  return (void*)r;
}



