#include "asm.h"

.section eentry
.globl handle_excep
.align 0x4
handle_excep:
	csrwr	$t0, LOONGARCH_CSR_SAVE0
    csrrd   $t0, LOONGARCH_CSR_PRMD
    andi    $t0, $t0, 0x3
    beqz    $t0, kernelvec
    b       uservec