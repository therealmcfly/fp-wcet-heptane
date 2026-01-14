	.file	"pm.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.comm	sig_buff,200,4
	.comm	snapshot_buff,200,4
	.globl	state
	.section	.sbss,"aw",@nobits
	.align	2
	.type	state, @object
	.size	state, 4
state:
	.zero	4
	.globl	sig_idx
	.section	.sdata,"aw"
	.align	2
	.type	sig_idx, @object
	.size	sig_idx, 4
sig_idx:
	.word	1
	.globl	time_counter
	.section	.sbss
	.align	2
	.type	time_counter, @object
	.size	time_counter, 4
time_counter:
	.zero	4
	.globl	lri_counter
	.align	2
	.type	lri_counter, @object
	.size	lri_counter, 4
lri_counter:
	.zero	4
	.globl	gri_counter
	.align	2
	.type	gri_counter, @object
	.size	gri_counter, 4
gri_counter:
	.zero	4
	.globl	activation_flag
	.align	2
	.type	activation_flag, @object
	.size	activation_flag, 4
activation_flag:
	.zero	4
	.globl	lowest_slope_sum
	.align	2
	.type	lowest_slope_sum, @object
	.size	lowest_slope_sum, 4
lowest_slope_sum:
	.zero	4
	.globl	lowest_slope_count
	.align	2
	.type	lowest_slope_count, @object
	.size	lowest_slope_count, 4
lowest_slope_count:
	.zero	4
	.comm	detection_threshold,4,4
	.text
	.align	2
	.globl	detect_activation
	.type	detect_activation, @function
detect_activation:
	addi	sp,sp,-32
	sw	s0,28(sp)
	addi	s0,sp,32
	sw	a0,-20(s0)
	lui	a5,%hi(detection_threshold)
	lw	a5,%lo(detection_threshold)(a5)
	lw	a4,-20(s0)
	bge	a4,a5,.L2
	lui	a5,%hi(activation_flag)
	li	a4,1
	sw	a4,%lo(activation_flag)(a5)
	j	.L4
.L2:
	lui	a5,%hi(activation_flag)
	sw	zero,%lo(activation_flag)(a5)
.L4:
	nop
	lw	s0,28(sp)
	addi	sp,sp,32
	jr	ra
	.size	detect_activation, .-detect_activation
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-48
	sw	ra,44(sp)
	sw	s0,40(sp)
	addi	s0,sp,48
	sw	a0,-36(s0)
	lui	a5,%hi(sig_buff)
	addi	a5,a5,%lo(sig_buff)
	lw	a4,4(a5)
	lui	a5,%hi(sig_buff)
	addi	a5,a5,%lo(sig_buff)
	lw	a5,0(a5)
	sub	a5,a4,a5
	sw	a5,-20(s0)
	li	a5,2
	sw	a5,-24(s0)
	j	.L6
.L8:
 #APP
# 62 "pm.c" 1
	1:
        .section .wcet_annot
        .long 1b
        .long 1
        .long 50
         .text
# 0 "" 2
 #NO_APP
	lui	a5,%hi(sig_buff)
	addi	a4,a5,%lo(sig_buff)
	lw	a5,-24(s0)
	slli	a5,a5,2
	add	a5,a4,a5
	lw	a4,0(a5)
	lw	a5,-24(s0)
	addi	a5,a5,-1
	lui	a3,%hi(sig_buff)
	addi	a3,a3,%lo(sig_buff)
	slli	a5,a5,2
	add	a5,a3,a5
	lw	a5,0(a5)
	sub	a5,a4,a5
	sw	a5,-28(s0)
	lw	a4,-28(s0)
	lw	a5,-20(s0)
	bge	a4,a5,.L7
	lw	a5,-28(s0)
	sw	a5,-20(s0)
.L7:
	lw	a5,-24(s0)
	addi	a5,a5,1
	sw	a5,-24(s0)
.L6:
	lw	a4,-24(s0)
	li	a5,49
	ble	a4,a5,.L8
 #APP
# 71 "pm.c" 1
	1:
        .section .switch_begin
        .long 1b
        .text
# 0 "" 2
 #NO_APP
	lui	a5,%hi(state)
	lw	a5,%lo(state)(a5)
	li	a4,3
	beq	a5,a4,.L9
	li	a4,3
	bgtu	a5,a4,.L10
	li	a4,2
	beq	a5,a4,.L11
	li	a4,2
	bgtu	a5,a4,.L10
	beq	a5,zero,.L12
	li	a4,1
	beq	a5,a4,.L13
	j	.L10
.L12:
	lui	a5,%hi(time_counter)
	lw	a4,%lo(time_counter)(a5)
	li	a5,81920
	bge	a4,a5,.L14
	lui	a5,%hi(lowest_slope_sum)
	lw	a4,%lo(lowest_slope_sum)(a5)
	lw	a5,-20(s0)
	add	a4,a4,a5
	lui	a5,%hi(lowest_slope_sum)
	sw	a4,%lo(lowest_slope_sum)(a5)
	lui	a5,%hi(lowest_slope_count)
	lw	a5,%lo(lowest_slope_count)(a5)
	addi	a4,a5,1
	lui	a5,%hi(lowest_slope_count)
	sw	a4,%lo(lowest_slope_count)(a5)
	j	.L16
.L14:
	lui	a5,%hi(lowest_slope_sum)
	lw	a5,%lo(lowest_slope_sum)(a5)
	srai	a4,a5,13
	lui	a5,%hi(detection_threshold)
	sw	a4,%lo(detection_threshold)(a5)
	lui	a5,%hi(lri_counter)
	sw	zero,%lo(lri_counter)(a5)
	lui	a5,%hi(state)
	li	a4,1
	sw	a4,%lo(state)(a5)
	j	.L16
.L13:
	lw	a0,-20(s0)
	call	detect_activation
	lui	a5,%hi(lri_counter)
	lw	a4,%lo(lri_counter)(a5)
	li	a5,20480
	addi	a5,a5,20
	bgt	a4,a5,.L17
	lui	a5,%hi(activation_flag)
	lw	a5,%lo(activation_flag)(a5)
	beq	a5,zero,.L23
	lui	a5,%hi(lri_counter)
	sw	zero,%lo(lri_counter)(a5)
	lui	a5,%hi(gri_counter)
	sw	zero,%lo(gri_counter)(a5)
	lui	a5,%hi(state)
	li	a4,2
	sw	a4,%lo(state)(a5)
	j	.L23
.L17:
	lui	a5,%hi(state)
	li	a4,3
	sw	a4,%lo(state)(a5)
	j	.L23
.L11:
	lui	a5,%hi(gri_counter)
	lw	a4,%lo(gri_counter)(a5)
	li	a5,12288
	addi	a5,a5,-288
	ble	a4,a5,.L24
	lui	a5,%hi(state)
	li	a4,1
	sw	a4,%lo(state)(a5)
	j	.L24
.L9:
	lw	a0,-20(s0)
	call	detect_activation
	lui	a5,%hi(activation_flag)
	lw	a5,%lo(activation_flag)(a5)
	beq	a5,zero,.L25
	lui	a5,%hi(lri_counter)
	sw	zero,%lo(lri_counter)(a5)
	lui	a5,%hi(gri_counter)
	sw	zero,%lo(gri_counter)(a5)
	lui	a5,%hi(state)
	li	a4,2
	sw	a4,%lo(state)(a5)
	j	.L25
.L10:
	li	a5,1
	j	.L21
.L23:
	nop
	j	.L16
.L24:
	nop
	j	.L16
.L25:
	nop
.L16:
	lui	a5,%hi(time_counter)
	lw	a5,%lo(time_counter)(a5)
	addi	a4,a5,10
	lui	a5,%hi(time_counter)
	sw	a4,%lo(time_counter)(a5)
	lui	a5,%hi(lri_counter)
	lw	a5,%lo(lri_counter)(a5)
	addi	a4,a5,10
	lui	a5,%hi(lri_counter)
	sw	a4,%lo(lri_counter)(a5)
	lui	a5,%hi(gri_counter)
	lw	a5,%lo(gri_counter)(a5)
	addi	a4,a5,10
	lui	a5,%hi(gri_counter)
	sw	a4,%lo(gri_counter)(a5)
	lui	a5,%hi(state)
	lw	a4,%lo(state)(a5)
	li	a5,3
	bne	a4,a5,.L22
	li	a5,0
	j	.L21
.L22:
	li	a5,0
.L21:
	mv	a0,a5
	lw	ra,44(sp)
	lw	s0,40(sp)
	addi	sp,sp,48
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 9.2.0"
