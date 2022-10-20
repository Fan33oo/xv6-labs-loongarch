// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "loongarch.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);
int steal();


struct run {
  struct run *next;
};

typedef struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

kmem kmem_cpu[NCPU];

void
kinit()
{
  int i;
  for (i = 0; i < NCPU; ++i) {
    initlock(&kmem_cpu[i].lock, "kmem");
  }
  freerange((void*)RAMBASE, (void*)RAMSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (uint64)pa < RAMBASE || (uint64)pa >= RAMSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  push_off();
  int cid = cpuid();
  acquire(&kmem_cpu[cid].lock);
  r->next = kmem_cpu[cid].freelist;
  kmem_cpu[cid].freelist = r;
  release(&kmem_cpu[cid].lock);
  pop_off();
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  push_off();
  int cid = cpuid();
  acquire(&kmem_cpu[cid].lock);
  r = kmem_cpu[cid].freelist;
  if(r) {
    kmem_cpu[cid].freelist = r->next;
  }
  else {
    if (steal()) {
      r = kmem_cpu[cid].freelist;
      kmem_cpu[cid].freelist = r->next;
    }
  }
  release(&kmem_cpu[cid].lock);
  pop_off();
  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

// call after push_off()
int
steal()
{
  int cpu;
  int cid = cpuid();
  int count = 0;
  struct run *r;
  for (cpu = 0; cpu < NCPU; ++cpu) {
    if (cpu == cid)
      continue;
    acquire(&kmem_cpu[cpu].lock);
    while (count < 50) {
      if (kmem_cpu[cpu].freelist) {
        r = kmem_cpu[cpu].freelist;
        kmem_cpu[cpu].freelist = r->next;
        r->next = kmem_cpu[cid].freelist;
        kmem_cpu[cid].freelist = r;
        count++;
      }
      else {
        break;
      }
    }
    release(&kmem_cpu[cpu].lock);
  }

  return count;
}