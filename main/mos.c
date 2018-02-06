//
// Created by song on 17-5-2.
//

#include "mos.h"
#include "Queue.h"

uint32_t TASKARR[TASKNUM][STACKSIZE];
uint8_t STATEARR[TASKNUM / 8 + 1];
uint8_t SUSPENDARR[TASKNUM / 8 + 1];
uint32_t PSPARR[TASKNUM];
int32_t SLEEPARR[TASKNUM];
Queue SEMAPHOREARR[TASKNUM];
//int8_t SEMAPHORESIZE[TASKNUM];

uint32_t curr_task = 0;
uint32_t next_task = 0;
volatile uint32_t systick_count = 0;
//嵌套层数
uint32_t criticalnesting;

void task_create(void *fun(void *), void *arg, uint8_t priority) {
    PSPARR[priority] = (unsigned int) &(TASKARR[priority]) + STACKSIZE * 4 - 18 * 4;

    uint8_t a = (uint8_t) (priority / 8);
    uint8_t b = (uint8_t) (priority % 8);
    STATEARR[a] |= 1 << b;
    SUSPENDARR[a] &= ~(1 << b);

    REG(PSPARR[priority] + (16 << 2)) = (unsigned int) fun;
    REG(PSPARR[priority] + (17 << 2)) = (unsigned int) 0x01000000;
//    REG(PSPARR[priority] + (0 << 2)) = (unsigned int) arg;
    REG(PSPARR[priority] + (0 << 2)) = (unsigned int) 0xfffffffd;
    REG(PSPARR[priority] + (1 << 2)) = (unsigned int) 0x02;


    SLEEPARR[priority] = 0;
    queue_init(&SEMAPHOREARR[priority], SEMAPHORECAP);

};

//void task_delete(uint8_t priority) {
//    mymemset(TASKARR[priority], 0, STACKSIZE);
//
//    uint8_t a = (uint8_t) (priority / 8);
//    uint8_t b = (uint8_t) (priority % 8);
//    STATEARR[a] &= ~(1 << b);
//
//    PSPARR[priority] = (uint32_t)0;
//}

void task_sleep(uint8_t priority, uint32_t tick) {

    SLEEPARR[priority] = tick;
    if (tick > 0) {
        uint8_t a = (uint8_t) (priority / 8);
        uint8_t b = (uint8_t) (priority % 8);
        STATEARR[a] &= ~(1 << b);
    }

    task_switch();

}


void os_init() {
    nvic_priority_group(3);
    nvic_init(15, 0, PendSV_IRQn, 4);
    nvic_init(14, 0, SysTick_IRQn, 4);
//    NVIC_SetPriority(PendSV_IRQn, 0xff);
//    NVIC_SetPriority(SysTick_IRQn, 0xef);
    for (uint8_t i = 0; i < TASKNUM; ++i) {
        SLEEPARR[i] = -1;
    }
}


void os_start() {

    task_create((void *(*)(void *)) idle_task, 0, TASKNUM - 1);
    curr_task = TASKNUM - 1;
    __set_PSP((PSPARR[curr_task] + 18 * 4));
//    NVIC_SetPriority(PendSV_IRQn, 0xff);
//    __set_CONTROL(0x03);
    __set_CONTROL(0x02); //这里为了较为方便的调用系统资源，而将整个系统置于特权状态。这也是freertos和ucos中的做法。


    __ISB();
    idle_task();
    while (1) {
    }
}

void idle_task() {
    while (1) {
    };
};


void SysTick_Handler(void) {
    systick_count++;
//    PGout(14) = ~PGout(14);
//    PGout(13) = ~PGout(13);
    for (uint8_t i = 0; i < TASKNUM; ++i) {
        if (SLEEPARR[i] == -1) {
            continue;
        } else {
            if (SLEEPARR[i] > 0) {

                //处于阻塞态的任务不进行睡眠自减操作

                if (!(SUSPENDARR[i / 8] & (1 << (i % 8)))) {
                    SLEEPARR[i]--;
                    if (SLEEPARR[i] == 0) {
                        uint8_t a = (uint8_t) (i / 8);
                        uint8_t b = (uint8_t) (i % 8);
                        STATEARR[a] |= 1 << b;
                    } else {
                        uint8_t a = (uint8_t) (i / 8);
                        uint8_t b = (uint8_t) (i % 8);
                        STATEARR[a] &= ~(1 << b);
                    }
                }
            }

            if (SEMAPHOREARR[i].size > 0) {
                uint8_t a = (uint8_t) (i / 8);
                uint8_t b = (uint8_t) (i % 8);
                SUSPENDARR[a] &= ~(1 << b);
            }
        }
    }

    uint8_t mask = 1;
    uint8_t num = 0;
    for (uint8_t i = 0; i < (TASKNUM / 8 + 1); ++i) {
        uint8_t tmp = STATEARR[i];
        uint8_t sus = SUSPENDARR[i];
        for (int j = 0; j < 8; ++j) {
            if ((tmp & mask) && (!(sus & mask))) {
                num = (uint8_t) (i * 8 + j);
                break;
            }
            mask = (uint8_t) (1 << (j + 1));
        }
    }

    next_task = num;
    if (curr_task != next_task) {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
}


void toggle_svc() {
    __asm volatile("SVC #0\n");
}

void SVC_Handler() {
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}


void PendSV_Handler() {
    __asm volatile(
    "CPSID   I    \n"
            "mrs r0,psp \n"
            "mov r2,lr  \n"
            "mrs r3,control  \n"

            "stmdb r0!,{r2-r11} \n"
            "ldr r1,=curr_task \n"
            "ldr r2,[r1] \n"
            "ldr r3,=PSPARR \n"
            "str r0,[r3,r2,lsl#2] \n"
            "ldr r4,=next_task \n"
            "ldr r4,[r4] \n"
            "str r4,[r1] \n"
            "ldr r0,[r3,r4,lsl#2] \n"
            "ldmia r0!,{r2-r11} \n"

            "mov lr,r2  \n"
            "msr control,r3  \n"

            "msr psp,r0 \n"
            "CPSIE   I \n"
            "bx lr \n"
            ".align \n"
    );
}

void disable_interrupt() {

    uint32_t ulNewBASEPRI;

    __asm volatile
    (
    "	mov %0, %1												\n"    \
        "	msr basepri, %0											\n" \
        "	isb														\n" \
        "	dsb														\n" \
        :"=r" (ulNewBASEPRI) : "i" ( MAX_INTERRUPT_PRI )
    );

}

void enable_interrupt() {
    setbasepri(0);
}

void setbasepri(uint32_t basepri) {
    __asm volatile
    (
    "	msr basepri, %0	"::"r" ( basepri )
    );
}

void enter_critical() {
    disable_interrupt();
    criticalnesting++;

}

void exit_critical() {

    criticalnesting--;
    if (criticalnesting == 0)
        enable_interrupt();
}


void task_switch() {
    enter_critical();

    uint8_t mask = 1;
    uint8_t num = 0;
    for (uint8_t i = 0; i < (TASKNUM / 8 + 1); ++i) {
        uint8_t tmp = STATEARR[i];
        uint8_t sus = SUSPENDARR[i];
        for (int j = 0; j < 8; ++j) {
            if ((tmp & mask) && (!(sus & mask))) {
                num = (uint8_t) (i * 8 + j);
                break;
            }
            mask = (uint8_t) (1 << (j + 1));
        }
    }

    next_task = num;

    exit_critical();
    if (curr_task != next_task) {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
//        toggle_svc();
    }

}


void task_suspend(uint8_t task_id) {

    if (SLEEPARR[task_id] != -1) {
        uint8_t a = (uint8_t) (task_id / 8);
        uint8_t b = (uint8_t) (task_id % 8);
        SUSPENDARR[a] |= (1 << b);
    }
    task_switch();
}


//只要是在statearr和suspendarr相应位变成有效，就要立即进行一次任务切换。

void task_unsuspend(uint8_t task_id) {

    if (SLEEPARR[task_id] != -1) {
        uint8_t a = (uint8_t) (task_id / 8);
        uint8_t b = (uint8_t) (task_id % 8);
        SUSPENDARR[a] &= ~(1 << b);
    }
    task_switch();
}

void task_suspend_irq(uint8_t task_id) {

    if (SLEEPARR[task_id] != -1) {
        uint8_t a = (uint8_t) (task_id / 8);
        uint8_t b = (uint8_t) (task_id % 8);
        SUSPENDARR[a] |= (1 << b);
    }
}

void task_unsuspend_irq(uint8_t task_id) {

    if (SLEEPARR[task_id] != -1) {
        uint8_t a = (uint8_t) (task_id / 8);
        uint8_t b = (uint8_t) (task_id % 8);
        SUSPENDARR[a] &= ~(1 << b);
    }
}


void os_delay(uint32_t nus) {
    enter_critical();
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;                //LOAD的值
    ticks = (nus * CLOCK) / 8;                   //需要的节拍数,这里sysclk的时钟是hclk的8分频；
    told = SysTick->VAL;                       //刚进入时的计数器值
    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told)tcnt += told - tnow;    //这里注意一下SYSTICK是一个递减的计数器就可以了.
            else tcnt += reload - tnow + told;
            told = tnow;
            if (tcnt >= ticks)break;            //时间超过/等于要延迟的时间,则退出.
        }
    };
    exit_critical();

}


uint8_t semaphore_request(uint8_t task_id) {
//    enter_critical();
    if (SEMAPHOREARR[task_id].size <= 0) {
        task_suspend(task_id);
    }
    //如果消息队列中的消息数为0，则在此处挂起。等消息树不为零时，systick会解挂，然后下一步从队列中取走消息。
    return queue_out(&SEMAPHOREARR[task_id]);
//    return queue_out(&SEMAPHOREARR[task_id]);

//    exit_critical();

}

void semaphore_post(uint8_t task_id, uint8_t semaphore) {
//    enter_critical();

    queue_in(&SEMAPHOREARR[task_id], semaphore);


//    exit_critical();

}
