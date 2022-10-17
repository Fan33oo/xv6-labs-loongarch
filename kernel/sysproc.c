#include "types.h"
#include "loongarch.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

#define MAP_FAILED ((char *) -1)

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_mmap(void)
{
  int length, prot, flags, fd;
  if (argint(1, &length) < 0 || argint(2, &prot) < 0
   || argint(3, &flags) < 0 || argint(4, &fd) < 0) {
    return (uint64)MAP_FAILED;
  }
  struct proc *p = myproc();
  struct VMA v;
  int i;
  int found = 0;
  uint64 addr = (uint64)MAXVA;
  addr = PGROUNDUP(addr - length);
  for (i = 0; i < 16; i++) {
    v = p->vma[i];
    if (v.used == 1){
      if (v.address <= addr) {
        addr = PGROUNDUP(v.address - length);
      }
    }
    else if (!found){
      found = i + 1;
    }
  }
  if (found) {
    found -= 1;
    p->vma[found].used = 1;
    p->vma[found].address = addr;
    p->vma[found].prot = prot;
    p->vma[found].flags = flags;
    p->vma[found].f = p->ofile[fd];
    filedup(p->vma[found].f);
    return addr;
  }
  else {
    return (uint64)MAP_FAILED;
  }
}

uint64
sys_munmap(void)
{
  return -1;
}