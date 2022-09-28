#include "asm.h"

.section tlbrentry
.globl handle_tlbr
.align 0x4
handle_tlbr:

	csrwr	$t0, LOONGARCH_CSR_TLBRSAVE
	csrwr	$t1, LOONGARCH_CSR_SAVE3
	csrwr	$t2, LOONGARCH_CSR_SAVE4

	csrrd	$t0, LOONGARCH_CSR_PGD
	lddir	$t0, $t0, 3
	addi.d  $t0, $t0, -1
	lddir	$t0, $t0, 2
	addi.d  $t0, $t0, -1
	lddir	$t0, $t0, 1
	addi.d  $t0, $t0, -1

	and		$t2, $t2, $r0
	addi.d  $t2, $t2, 8

	csrrd	$t1, LOONGARCH_CSR_TLBRBADV
	srli.d	$t1, $t1, 12
	andi	$t1, $t1, 0x1FE
	mul.d	$t1, $t1, $t2
	add.d	$t1, $t1, $t0
	ld.d	$t1, $t1, 0
	srli.d	$t1, $t1, 60
	andi	$t1, $t1, 1
	bnez	$t1, tag1
	csrwr	$r0, LOONGARCH_CSR_TLBRELO0
	bl		tag2
tag1:
	ldpte	$t0, 0
tag2:
	csrrd	$t1, LOONGARCH_CSR_TLBRBADV
	srli.d	$t1, $t1, 12
	andi	$t1, $t1, 0x1FE
	addi.d  $t1, $t1, 1
	mul.d	$t1, $t1, $t2
	add.d	$t1, $t1, $t0
	ld.d	$t1, $t1, 0
	srli.d	$t1, $t1, 60
	andi	$t1, $t1, 1
	bnez	$t1, tag3
	csrwr	$r0, LOONGARCH_CSR_TLBRELO1
	bl 		tag4
tag3:
	ldpte	$t0, 1
tag4:
	tlbfill

	csrrd	$t0, LOONGARCH_CSR_TLBRSAVE
	csrrd	$t1, LOONGARCH_CSR_SAVE3
	csrrd 	$t2, LOONGARCH_CSR_SAVE4
	ertn
