#include "types.h"
#include "loongarch.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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


int
sys_pgaccess(void)
{
  int temp = 0;

  uint64 addr;
  int n;
  uint64 buf;
  if (argaddr(0, &addr) < 0)
    return -1;
  if (argint(1, &n) < 0)
    return -1;
  if (argaddr(2, &buf) < 0)
    return -1;
  
  if (n > 32)
    return -1; 
  
  pagetable_t pt = myproc()->pagetable;
  pte_t *pte;
  for (int i = 0; i < n; i++) {
    pte = walk(pt, addr + i * PGSIZE, 0);
    if (*pte & PTE_A) {
      temp |= (1 << i);
      printf("%d\n", i);
      *pte &= ~PTE_A;
    }
  }
  copyout(pt, buf, (char *)&temp, sizeof(temp));

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
