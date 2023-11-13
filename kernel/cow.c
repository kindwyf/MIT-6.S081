#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "memlayout.h"

struct {
  uint8 count;
  struct spinlock lock;
} cows[(PHYSTOP - KERNBASE) >> 12];

void increcount(uint64 pa){
  if(pa < KERNBASE)
    return ;
  uint64 index = (pa - KERNBASE) >> 12;
  acquire(&cows[index].lock);
  cows[index].count++;
  release(&cows[index].lock);
}

void decrecount(uint64 pa){
  if(pa < KERNBASE)
    return ;
  uint64 index = (pa - KERNBASE) >> 12;
  acquire(&cows[index].lock);
  cows[index].count--;
  release(&cows[index].lock);
}

uint8 getcount(uint64 pa){
  if(pa < KERNBASE)
    return 0;
  uint64 index = (pa - KERNBASE) >> 12;
  return cows[index].count;
}

