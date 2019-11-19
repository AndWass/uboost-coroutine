/****************************************************
 *
 * -------------------------------------------------
 * |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
 * -------------------------------------------------
 * | r0  | r4  | r5  | r6  | r7  | r8  | r9  | r10 |
 * -------------------------------------------------
 * |  8  |  9  | 10  | 11  | 12  | 13  | 14  |
 * -------------------------------------------
 * | r11 | lr  | pc  | fctx| data|
 * -------------------------------------------
 *
 ****************************************************/

.file "make_arm_none_cortexm3_nofp.S"
.text
.globl make_fcontext
.align 2
.type make_fcontext,%function
.syntax unified
/*
 * r0 contains pointer to stack
 * r1 contains pointer to context function
 * A complete context switch consists of
 * pushing {lr} as PC and then {r0, r4-r11, lr}
 * which means we push 11 registers = 40 bytes
 */
make_fcontext:
    @ Align the context pointer to 8 bytes
    bic r0, r0, #7
    @ Use 64 bytes to store the current context and
    @ some space between function data and
    @ the context.
    sub r0, r0, #64

    @ Set PC and lr to the second argument
    str r1, [r0, #36]
    str r1, [r0, #40]

    @ Address of returned transfer_t is put in
    @ r0
    add r2, r0, #44
    str r2, [r0]

    bx lr

