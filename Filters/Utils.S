/*
 * FFT.S
 *
 *  Created on: 4 Jun 2015
 *      Author: Hurtman
 */


.section .text
.globl BitReverse
BitReverse:
	rbit r3, r0
	lsr r0, r3, #25
	bx lr
.globl Sqrt2
Sqrt2:
	mov R1, #4
	mov R3, #128
loop1:
	cbz R1,loop1exit
	sdiv R2, R0, R3
	add R3, R2
    lsr R3, R3, #1
	sub R1, #1
	b loop1
loop1exit:
	mov R0, R3
	bx LR
.globl FIR
FIR:
    push {R4}
    push {R5}
    push {R6}
	mov R3, #32
	mov R4, #0
	lsl R2, #1
loop2:
	cbz R3,loop2exit
	ldrh R5, [R0, R2]
	sub R2, #2
	ubfx R2, R2, #0, #11
	ldrsh R6, [R1, R3]
	mul R5, R5, R6
	add R4, R5
	sub R3, #2
	b loop2
loop2exit:
	lsr R0, R4, #7
    pop {R6}
    pop {R5}
	pop {R4}
    bx LR
