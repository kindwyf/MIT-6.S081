// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  char lockname[8];
} kmems[NCPU];

void
kinit()
{
  for(int i=0;i<NCPU;i++){
    snprintf(kmems[i].lockname, 8, "keme_%d", i);
    initlock(&kmems[i].lock, kmems[i].lockname);
  }
  freerange(end, (void*)PHYSTOP);
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

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  push_off();
  int cid = cpuid();
  pop_off();

  acquire(&kmems[cid].lock);
  r->next = kmems[cid].freelist;
  kmems[cid].freelist = r;
  release(&kmems[cid].lock);
}

// For steal free list (lab 8-1)
struct run* steal(int cid){
  int ncid = cid;
  struct run* head, *fast, *slow;
  push_off();
  // 传入的cid与当前运行的cpu不是一个
  if(cid != cpuid())
    panic("steal");
  pop_off();
  for(int i=1;i<NCPU;i++){
    if(++ncid == NCPU)
      ncid = 0;
    acquire(&kmems[ncid].lock);
    if(kmems[ncid].freelist){
      // 从当前cpu的空闲链表中偷一半
      // 采用快慢指针的方式
      head = slow = kmems[ncid].freelist;
      fast = slow->next;
      while(fast){
        fast = fast->next;
        if(fast){
          slow = fast;
          fast = slow->next;
        }
      }
      // 后一半留给找到的有空闲的cpu
      kmems[ncid].freelist = slow->next;
      // 前一半以head为头，作为返回值
      release(&kmems[ncid].lock);
      slow->next = 0;
      return head;
    }
    release(&kmems[ncid].lock);
  }
  return 0;
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
  pop_off();
  acquire(&kmems[cid].lock);
  r = kmems[cid].freelist;
  if(r)
    kmems[cid].freelist = r->next;
  if(!r&&(r = steal(cid))){
    kmems[cid].freelist = r->next;
  }
  release(&kmems[cid].lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
