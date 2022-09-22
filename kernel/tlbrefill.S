#include "asm.h"

.section tlbrentry
.globl handle_tlbr
.align 0x4
handle_tlbr:
	csrwr	$t0, LOONGARCH_CSR_TLBRSAVE
	
	csrrd	$t0, LOONGARCH_CSR_PGD
	lddir	$t0, $t0, 3
	addi.d  $t0, $t0, -1
	lddir	$t0, $t0, 2
	addi.d  $t0, $t0, -1
	lddir	$t0, $t0, 1
	addi.d  $t0, $t0, -1
	ldpte	$t0, 0
	ldpte	$t0, 1

	; csrrd 	$t0, LOONGARCH_CSR_TLBELO0
	; ori     $t0, $t0, 1
	; xori	$t0, $t0, 1 
	; csrwr	$t0, LOONGARCH_CSR_TLBELO0
	
	; csrrd 	$t0, LOONGARCH_CSR_TLBELO1
	; ori 	$t0, $t0, 1
	; xori	$t0, $t0, 1 
	; csrwr	$t0, LOONGARCH_CSR_TLBELO1
	tlbfill

	csrrd   $t1, LOONGARCH_CSR_TLBRBADV
	li.d	$t2, 0b111111111
	slli.d	$t2, $t2, 12
	and		$t1, $t2, $t1
	srli.d 	$t1, $t1, 12
	li.d    $t3, 8
	mul.d   $t1, $t1, $t3
	add.d   $t3, $t0, $t1
	ld.d    $t1, $t3, 0
	li.d    $t2, 1
	slli.d	$t2, $t2, 60
	or		$t1, $t2, $t1
	st.d  	$t1, $t3, 0

	csrrd	$t0, LOONGARCH_CSR_TLBRSAVE
	ertn


	 
