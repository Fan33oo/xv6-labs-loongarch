#include "types.h"
#include "param.h"
#include "loongarch.h"
#include "defs.h"
#include "memlayout.h"

// entry.S needs one stack per CPU.
__attribute__ ((aligned (16))) char stack0[4096 * NCPU];

volatile static int started = 0;

// entry.S jumps here on stack0.
void
main()
{
   if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6-loongarch kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    vminit();        // create kernel page table
    procinit();      // process table
    trapinit();      // trap vectors
    apic_init();     // set up LS7A1000 interrupt controller
    extioi_init();   // extended I/O interrupt controller
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    ramdiskinit();   // emulated hard disk
    userinit();      // first user process
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
  }
    scheduler(); 
}

