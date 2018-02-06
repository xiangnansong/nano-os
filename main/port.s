    .syntax unified

    .global PendSV_Handler

	.thumb
	.section ".text"
	.align  4
    .thumb_func:
	PendSV_Handler:

	    mrs r0,msp
        stmdb r0!,{r4-r11}
        ldr r1,=curr_task
        ldr r2,[r1]
        ldr r3,=PSPARR
        str r0,[r3,r2,lsl#2]
        ldr r4,=next_task
        ldr r4,[r4]
        str r4,[r1]
        ldr r0,[r3,r4,lsl#2]
        ldmia r0!,{r4-r11}
        msr msp,r0
        bx lr

	.end