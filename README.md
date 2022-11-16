# xv6-labs-loongarch

This is a reference implementaion of xv6-labs-2021's labs on LoongArch. Inspired by MIT's xv6-riscv Edition, check https://pdos.csail.mit.edu/6.828/2021/tools.html for more details.

## Dependencies

Visit [qemu-loongarch-runenv](https://github.com/foxsen/qemu-loongarch-runenv) for more information about Loongarch's cross compilation tools, gdb and qemu.

## Run on qemu-system-loongarch64

```bash
make qemu
```
## Guide
The master branch is the basic version of xv6-loongarch that has been ported from an unknown version of xv6 to loongarch by others. After some minor modifications (e.g., allowing it to run programs under the root directory in other directories), it is now the basis of all labs. It can pass all its usertests. It differs from RISC-V version in these aspects:

- In terms of memory management, RISC-V kernel use a page table, which maps uart registers, mmio disk interfaces, PLIC, trampoline, and process kernel stacks in it. Because Loongarch has a direct mapping address translation mode, the kernel uses a direct mapping window instead of a page table. However, in the master branch, the kernel still has a page table, which maps the kernel stack of each process. From a later branch, for the sake of adding other functions, the method of the kernel using a page table to access memory has been completely modified.

- In the term of the disk io, xv6-riscv uses mmio, and the access code involves `virtio. h` and `virtio_ disk. c`. The loongarch version only simulates the disk io. In fact, the file system is written to the header file `ramdisk. h`, and then `ramdisk.c` access what is still in memory in essence. (Because the system is only used for teaching, the old version of xv6 seems to simulate the disk io in this way as well. The current mmio is only a way that can be used on a virtual machine, but with some minor modifications, xv6 can really run on a real machine.)

- There are also  differences in some architecture related parts, such as interrupts between the loongarch version and the RISC-V version. Please refer to the RISC-V manual and the Loongarch manual for details.

Practicing xv6-labs on xv6-loongarch can learn something about Loongarch architecture while being familiar with operating system principles. Like xv6-labs-2021, a branch is created for each lab, and the branch name is the same as xv6-labs-2021. Each branch has a python script of `grade-lab-*`, which can be run to check whether the exercise has been completed correctly. There will be some file differences between branches of different labs. Use `git diff` to view.

It is recommended that you read the official exercises and tips of xv6 first, and then check its implementation on Loongarch in the corresponding location. There is no difference in many architecture independent places. If you want to write your own code to complete the exercise, you can create a new branch and delete the completed code for the exercise. Later, a clean and unfinished repository may be created for teaching, but it is not available at present.

Due to different architectures, the official tips of xv6 may not be applicable to xv6-loongarch. Therefore, the following points need attention for each lab of xv6-loongarch.

### util
This lab is to make you familiar with the use of tools and application programming on xv6, including how to use system calls. This lab seems to have no architecture related aspects, and the code implementation should be exactly the same as that on RISC-V.

### syscall
Still architecture independent.

### pgtbl
#### Speed up system calls
Note the difference between xv6-loongarch and xv6 riscv-in `memlayout.h`, `USYSCALL` should be defined above `TRAPFRAME`.

#### Detecting which pages have been accessed
The solution for this exercise on xv6-loongarch is completely different from that of xv6-riscv. The TLB miss of RISC—V is completely in the charge of the hardware and does not need the concern of the operating system. When TLB miss happens, the hardware will automatically set the `PTE_A` bit. The system can judge whether the page has been accessed according to this bit. But the TLB refilling in Loongarch is completed by the operating system. In xv6-loongarch, it is `tlbrefill.S`. What is more troublesome is that the TLB refilling of Loongarch requires two parity pages to be completed together. This means that in order to achieve this seemingly simple function, the tlb refilling program will become more complex. The solution given here is very complicated. We no longer use the simple TLB filling instructions, but software walk the page table to read `PTE_ A` bit. According to `PTE_ A` judge whether to fill in a valid page table entry. If the page has not been accessed, fill in the invalid page table entry. After it is accessed, an interrupt will be triggered. In the interrupt handler, we set the `PTE_A` for it(see `trap.c`). Since then, the xv6-loongarch kernel has completely stopped using a "page table way" to access memory.

### trap
#### Backtrace
To implement this architecture related feature, you need to consult the Loongarch ABI manual.

### cow
There is one thing you should pay attention to when solving this lab on loongarch. `PTE_W` bit in page table entry does not work because it does not exist in the TLB table enrty. Use `PTE_ D` bit! If `PTE_D` bit is 0, an interrupt will be triggered when the page is written. After you notice this, other implementations should be similar to those on RISC-V. Note that you should be careful when solving this lab. In some cases, you need to use locks.

### thread
#### Uthread: switching between threads
Although this exercise is architecture related, you can learn from the code in `kernelvec.S`.

### net
This lab is not available on xv6-loongarch (have trouble adding pci devices to qemu-loongarch or qemu-loongarch itself may not support it), but the relevant codes involved are available and have been verified on xv6-riscv.

### lock
Because qemu-loongarch cannot laurnch multi-cores, passing the test script does not mean that you have completed the exercise correctly. This lab is not architecture related and can be done on xv6-riscv. The relevant code has been verified on xv6 riscv.

### fs
#### Large files
Because the disk io of xv6-loongarch is only simulated as mentioned above, this lab can only be performed on servers with large memory. We tried to modify such disk io, but qemu-loongarch may not support it temporarily (although there is an official Loongarch version of qemu, it's still not used here due to the incorrect bios).

### mmap
It is not architecture related and can be completed normally.

## To do
There are still many problems with xv6-labs-loongarch. The following are the unfinished things can be done in future.

- Replace with the official qemu-loongarch (new bios is required).
- Solve problems related to qemu in lab net and lab lock.
- Use normal disk io instead of writing header files, so lab fs can be completed without large memory.
- Create a pure repository that has only carried out the necessary configuration but has not completed the labs. Users can directly write codes in the teaching.
- Better reference implementation (improve code comments and parsing).
