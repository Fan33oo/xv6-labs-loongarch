#define LOONGARCH_CSR_CRMD		    0x0	    /* Current mode info */
#define LOONGARCH_CSR_PRMD          0x1     /* Prev-exception mode info */
#define LOONGARCH_CSR_CPUID		    0x20	/* CPU core id */

#define LOONGARCH_CSR_SAVE0		    0x30    /* Kscratch registers */
#define LOONGARCH_CSR_SAVE1         0x31    /* Kscratch registers */
#define LOONGARCH_CSR_SAVE2         0x32
#define LOONGARCH_CSR_SAVE3         0x33
#define LOONGARCH_CSR_SAVE4         0x34
#define LOONGARCH_CSR_SAVE5         0x35
#define LOONGARCH_CSR_SAVE6         0x36
#define LOONGARCH_CSR_SAVE7         0x37
#define LOONGARCH_CSR_SAVE8         0x38

#define LOONGARCH_CSR_DMWIN0		0x180	/* 64 direct map win0: MEM & IF */
#define LOONGARCH_CSR_DMWIN1		0x181	/* 64 direct map win1: MEM & IF */
#define LOONGARCH_CSR_DMWIN2		0x182	/* 64 direct map win2: MEM */
#define LOONGARCH_CSR_DMWIN3		0x183	/* 64 direct map win3: MEM */

#define LOONGARCH_CSR_TLBEHI		0x11	/* TLB EntryHi */
#define LOONGARCH_CSR_PGDL          0x19
#define LOONGARCH_CSR_PGD           0x1b
#define LOONGARCH_CSR_TLBRENTRY		0x88	/* TLB refill exception entry */
#define LOONGARCH_CSR_TLBRBADV		0x89	/* TLB refill badvaddr */
#define LOONGARCH_CSR_TLBRSAVE		0x8b	/* KScratch for TLB refill exception */

/* Direct Map window 0/1 */