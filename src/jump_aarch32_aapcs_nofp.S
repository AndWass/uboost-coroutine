/****************************************************
 *
 * -------------------------------------------------
 * |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
 * -------------------------------------------------
 * | r0  | r1  | r2  | r3  | r4  | r5  | r6  | r7  |
 * -------------------------------------------------
 * |  8  |  9  | 10  | 11  | 12  | 13  | 14  | 15  |
 * -------------------------------------------------
 * | r8  | r9  | r10 | r11 | r12 | sp  | lr  | pc  |
 * -------------------------------------------------
 * |  16   |
 * ---------
 * |  PSR  |
 * ---------
 *
 ****************************************************/

.file "jump_aarch32_aapcs_nofp.S"
.text
.globl jump_fcontext
.align 2
.type jump_fcontext,%function
.syntax unified
/*
 * r0 contains pointer to return value structure
 * r1 contains pointer to context
 * r2 contains pointer to extra data
 */
jump_fcontext:
    push {lr}
    push {r0,r4-r11,lr}

    mov r0, sp
    mov sp, r1

    pop {r1, r4-r11, lr}
    str r0, [r1, #0]
    str r2, [r1, #4]
    mov r1, r2
    pop {pc}


.globl ontop_fcontext
.align 2
.type ontop_fcontext,%function
.syntax unified
ontop_fcontext:
    push {lr}
    push {r0,r4-r11,lr}

    mov r0, sp
    mov sp, r1
    @ parent context is stored in data
    mov r1, r0

    pop {r0, r4-r11, lr}
    str r1, [r0, #0]
    str r2, [r0, #4]
    mov r1, r2
    add sp, sp, #4
    bx r3

