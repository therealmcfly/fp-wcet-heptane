	.file	"test2.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-32
	sw	s0,28(sp)
	addi	s0,sp,32
	sw	a0,-20(s0)
 #APP
# 5 "test2.c" 1
	1:
        .section .switch_begin
        .long 1b
        .text
# 0 "" 2
 #NO_APP
	lw	a5,-20(s0)
	bne	a5,zero,.L2
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	addi	a5,a5,2
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	addi	a5,a5,3
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	addi	a5,a5,4
	sw	a5,-20(s0)
	j	.L3
.L2:
	lw	a5,-20(s0)
	addi	a5,a5,1
	sw	a5,-20(s0)
	nop
.L3:
	nop
	lw	s0,28(sp)
	addi	sp,sp,32
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 9.2.0"
