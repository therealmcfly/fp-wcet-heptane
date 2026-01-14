	.file	"sqrt.c"
	.option nopic
	.attribute arch, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.align	1
	.globl	my_fabs
	.type	my_fabs, @function
my_fabs:
.LFB0:
	.file 1 "sqrt.c"
	.loc 1 56 1
	.cfi_startproc
	addi	sp,sp,-16
	.cfi_def_cfa_offset 16
	fsw	fa0,12(sp)
	.loc 1 57 5
	flw	fa5,12(sp)
	fmv.s.x	fa4,zero
	flt.s	a5,fa5,fa4
	beq	a5,zero,.L6
	.loc 1 58 10
	flw	fa5,12(sp)
	fneg.s	fa5,fa5
	j	.L4
.L6:
	.loc 1 60 10
	flw	fa5,12(sp)
.L4:
	.loc 1 61 1
	fmv.s	fa0,fa5
	addi	sp,sp,16
	.cfi_def_cfa_offset 0
	jr	ra
	.cfi_endproc
.LFE0:
	.size	my_fabs, .-my_fabs
	.align	1
	.globl	my_sqrt
	.type	my_sqrt, @function
my_sqrt:
.LFB1:
	.loc 1 65 1
	.cfi_startproc
	addi	sp,sp,-80
	.cfi_def_cfa_offset 80
	sd	ra,72(sp)
	.cfi_offset 1, -8
	fsw	fa0,12(sp)
	.loc 1 66 8
	flw	fa4,12(sp)
	lui	a5,%hi(.LC0)
	flw	fa5,%lo(.LC0)(a5)
	fdiv.s	fa5,fa4,fa5
	fsw	fa5,60(sp)
	.loc 1 71 9
	lui	a5,%hi(.LC1)
	fld	fa5,%lo(.LC1)(a5)
	fsd	fa5,40(sp)
	.loc 1 75 7
	sw	zero,52(sp)
	.loc 1 76 5
	flw	fa5,12(sp)
	fmv.s.x	fa4,zero
	feq.s	a5,fa5,fa4
	beq	a5,zero,.L8
	.loc 1 77 5
	sw	zero,60(sp)
	j	.L9
.L8:
	.loc 1 79 10
	li	a5,1
	sw	a5,56(sp)
	.loc 1 79 3
	j	.L10
.L14:
	.loc 1 80 5
 #APP
# 80 "sqrt.c" 1
	1:
        .section .wcet_annot
        .long 1b
        .long 1
        .long 19
         .text
# 0 "" 2
	.loc 1 81 8
 #NO_APP
	lw	a5,52(sp)
	sext.w	a5,a5
	bne	a5,zero,.L12
	.loc 1 82 22
	flw	fa5,60(sp)
	fmul.s	fa5,fa5,fa5
	.loc 1 82 17
	flw	fa4,12(sp)
	fsub.s	fa5,fa4,fa5
	fcvt.d.s	fa4,fa5
	.loc 1 82 35
	flw	fa5,60(sp)
	fcvt.d.s	fa5,fa5
	fadd.d	fa5,fa5,fa5
	.loc 1 82 28
	fdiv.d	fa5,fa4,fa5
	.loc 1 82 10
	fcvt.s.d	fa5,fa5
	fsw	fa5,36(sp)
	.loc 1 83 9
	flw	fa4,60(sp)
	flw	fa5,36(sp)
	fadd.s	fa5,fa4,fa5
	fsw	fa5,60(sp)
	.loc 1 84 23
	flw	fa5,60(sp)
	fmul.s	fa5,fa5,fa5
	.loc 1 84 18
	flw	fa4,12(sp)
	fsub.s	fa5,fa4,fa5
	.loc 1 84 12
	fcvt.d.s	fa5,fa5
	fsd	fa5,24(sp)
	.loc 1 85 11
	fld	fa5,24(sp)
	fcvt.s.d	fa5,fa5
	fmv.s	fa0,fa5
	call	my_fabs
	fmv.s	fa5,fa0
	fcvt.d.s	fa5,fa5
	.loc 1 85 10
	fld	fa4,40(sp)
	fge.d	a5,fa4,fa5
	beq	a5,zero,.L12
	.loc 1 86 14
	li	a5,1
	sw	a5,52(sp)
.L12:
	.loc 1 79 24 discriminator 2
	lw	a5,56(sp)
	addiw	a5,a5,1
	sw	a5,56(sp)
.L10:
	.loc 1 79 3 discriminator 1
	lw	a5,56(sp)
	sext.w	a4,a5
	li	a5,19
	ble	a4,a5,.L14
.L9:
	.loc 1 91 9
	flw	fa5,60(sp)
	.loc 1 92 1
	fmv.s	fa0,fa5
	ld	ra,72(sp)
	.cfi_restore 1
	addi	sp,sp,80
	.cfi_def_cfa_offset 0
	jr	ra
	.cfi_endproc
.LFE1:
	.size	my_sqrt, .-my_sqrt
	.align	1
	.globl	main
	.type	main, @function
main:
.LFB2:
	.loc 1 96 1
	.cfi_startproc
	addi	sp,sp,-16
	.cfi_def_cfa_offset 16
	sd	ra,8(sp)
	.cfi_offset 1, -8
	.loc 1 97 2
	lui	a5,%hi(.LC2)
	flw	fa5,%lo(.LC2)(a5)
	fmv.s	fa0,fa5
	call	my_sqrt
	.loc 1 98 9
	li	a5,0
	.loc 1 99 1
	mv	a0,a5
	ld	ra,8(sp)
	.cfi_restore 1
	addi	sp,sp,16
	.cfi_def_cfa_offset 0
	jr	ra
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.section	.rodata
	.align	2
.LC0:
	.word	1092616192
	.align	3
.LC1:
	.word	2296604913
	.word	1055193269
	.align	2
.LC2:
	.word	1100742656
	.text
.Letext0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.4byte	0x111
	.2byte	0x4
	.4byte	.Ldebug_abbrev0
	.byte	0x8
	.byte	0x1
	.4byte	.LASF5
	.byte	0xc
	.4byte	.LASF6
	.4byte	.LASF7
	.8byte	.Ltext0
	.8byte	.Letext0-.Ltext0
	.4byte	.Ldebug_line0
	.byte	0x2
	.4byte	.LASF8
	.byte	0x1
	.byte	0x5f
	.byte	0x1
	.4byte	0x4b
	.8byte	.LFB2
	.8byte	.LFE2-.LFB2
	.byte	0x1
	.byte	0x9c
	.byte	0x3
	.byte	0x4
	.byte	0x5
	.string	"int"
	.byte	0x4
	.4byte	.LASF9
	.byte	0x1
	.byte	0x40
	.byte	0x1
	.4byte	0xda
	.8byte	.LFB1
	.8byte	.LFE1-.LFB1
	.byte	0x1
	.byte	0x9c
	.4byte	0xda
	.byte	0x5
	.string	"val"
	.byte	0x1
	.byte	0x40
	.byte	0xf
	.4byte	0xda
	.byte	0x3
	.byte	0x91
	.byte	0xbc,0x7f
	.byte	0x6
	.string	"x"
	.byte	0x1
	.byte	0x42
	.byte	0x8
	.4byte	0xda
	.byte	0x2
	.byte	0x91
	.byte	0x6c
	.byte	0x6
	.string	"dx"
	.byte	0x1
	.byte	0x44
	.byte	0x8
	.4byte	0xda
	.byte	0x2
	.byte	0x91
	.byte	0x54
	.byte	0x7
	.4byte	.LASF0
	.byte	0x1
	.byte	0x46
	.byte	0x9
	.4byte	0xe1
	.byte	0x2
	.byte	0x91
	.byte	0x48
	.byte	0x7
	.4byte	.LASF1
	.byte	0x1
	.byte	0x47
	.byte	0x9
	.4byte	0xe1
	.byte	0x2
	.byte	0x91
	.byte	0x58
	.byte	0x6
	.string	"i"
	.byte	0x1
	.byte	0x49
	.byte	0x6
	.4byte	0x4b
	.byte	0x2
	.byte	0x91
	.byte	0x68
	.byte	0x7
	.4byte	.LASF2
	.byte	0x1
	.byte	0x49
	.byte	0x9
	.4byte	0x4b
	.byte	0x2
	.byte	0x91
	.byte	0x64
	.byte	0
	.byte	0x8
	.byte	0x4
	.byte	0x4
	.4byte	.LASF3
	.byte	0x8
	.byte	0x8
	.byte	0x4
	.4byte	.LASF4
	.byte	0x9
	.4byte	.LASF10
	.byte	0x1
	.byte	0x37
	.byte	0x1
	.4byte	0xda
	.8byte	.LFB0
	.8byte	.LFE0-.LFB0
	.byte	0x1
	.byte	0x9c
	.byte	0x5
	.string	"x"
	.byte	0x1
	.byte	0x37
	.byte	0xf
	.4byte	0xda
	.byte	0x2
	.byte	0x91
	.byte	0x7c
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.byte	0x1
	.byte	0x11
	.byte	0x1
	.byte	0x25
	.byte	0xe
	.byte	0x13
	.byte	0xb
	.byte	0x3
	.byte	0xe
	.byte	0x1b
	.byte	0xe
	.byte	0x11
	.byte	0x1
	.byte	0x12
	.byte	0x7
	.byte	0x10
	.byte	0x17
	.byte	0
	.byte	0
	.byte	0x2
	.byte	0x2e
	.byte	0
	.byte	0x3f
	.byte	0x19
	.byte	0x3
	.byte	0xe
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x39
	.byte	0xb
	.byte	0x27
	.byte	0x19
	.byte	0x49
	.byte	0x13
	.byte	0x11
	.byte	0x1
	.byte	0x12
	.byte	0x7
	.byte	0x40
	.byte	0x18
	.byte	0x96,0x42
	.byte	0x19
	.byte	0
	.byte	0
	.byte	0x3
	.byte	0x24
	.byte	0
	.byte	0xb
	.byte	0xb
	.byte	0x3e
	.byte	0xb
	.byte	0x3
	.byte	0x8
	.byte	0
	.byte	0
	.byte	0x4
	.byte	0x2e
	.byte	0x1
	.byte	0x3f
	.byte	0x19
	.byte	0x3
	.byte	0xe
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x39
	.byte	0xb
	.byte	0x27
	.byte	0x19
	.byte	0x49
	.byte	0x13
	.byte	0x11
	.byte	0x1
	.byte	0x12
	.byte	0x7
	.byte	0x40
	.byte	0x18
	.byte	0x96,0x42
	.byte	0x19
	.byte	0x1
	.byte	0x13
	.byte	0
	.byte	0
	.byte	0x5
	.byte	0x5
	.byte	0
	.byte	0x3
	.byte	0x8
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x39
	.byte	0xb
	.byte	0x49
	.byte	0x13
	.byte	0x2
	.byte	0x18
	.byte	0
	.byte	0
	.byte	0x6
	.byte	0x34
	.byte	0
	.byte	0x3
	.byte	0x8
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x39
	.byte	0xb
	.byte	0x49
	.byte	0x13
	.byte	0x2
	.byte	0x18
	.byte	0
	.byte	0
	.byte	0x7
	.byte	0x34
	.byte	0
	.byte	0x3
	.byte	0xe
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x39
	.byte	0xb
	.byte	0x49
	.byte	0x13
	.byte	0x2
	.byte	0x18
	.byte	0
	.byte	0
	.byte	0x8
	.byte	0x24
	.byte	0
	.byte	0xb
	.byte	0xb
	.byte	0x3e
	.byte	0xb
	.byte	0x3
	.byte	0xe
	.byte	0
	.byte	0
	.byte	0x9
	.byte	0x2e
	.byte	0x1
	.byte	0x3f
	.byte	0x19
	.byte	0x3
	.byte	0xe
	.byte	0x3a
	.byte	0xb
	.byte	0x3b
	.byte	0xb
	.byte	0x39
	.byte	0xb
	.byte	0x27
	.byte	0x19
	.byte	0x49
	.byte	0x13
	.byte	0x11
	.byte	0x1
	.byte	0x12
	.byte	0x7
	.byte	0x40
	.byte	0x18
	.byte	0x97,0x42
	.byte	0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.4byte	0x2c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x8
	.byte	0
	.2byte	0
	.2byte	0
	.8byte	.Ltext0
	.8byte	.Letext0-.Ltext0
	.8byte	0
	.8byte	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF3:
	.string	"float"
.LASF0:
	.string	"diff"
.LASF7:
	.string	"/home/eugene/heptane-master/benchmarks/sqrt"
.LASF9:
	.string	"my_sqrt"
.LASF6:
	.string	"/tmp/sqrt.c"
.LASF10:
	.string	"my_fabs"
.LASF1:
	.string	"min_tol"
.LASF2:
	.string	"flag"
.LASF4:
	.string	"double"
.LASF8:
	.string	"main"
.LASF5:
	.string	"GNU C17 9.2.0 -mtune=rocket -march=rv64imafdc -mabi=lp64d -ggdb -O0 -fomit-frame-pointer"
	.ident	"GCC: (GNU) 9.2.0"
