//
// Created by song on 17-5-2.
//

#ifndef MOS_MOS_H
#define MOS_MOS_H

#include "main.h"

#define TASKNUM 6
#define STACKSIZE 512
#define REG(ADDRESS) (*((volatile unsigned int *)(ADDRESS)))
#define SEMAPHORECAP 10
//用于终端管理，只对优先级大于5的中断进行屏蔽，当有优先级小于5的中断响应时，正常响应，以保证系统实时性
#define MAX_INTERRUPT_PRI 5
#define CLOCK 180


void task_create(void *fun(void *), void *arg, uint8_t priority);
void os_start();
void os_init();
void idle_task();
void task_sleep(uint8_t priority,uint32_t tick);
void toggle_svc();

void disable_interrupt();
void enable_interrupt();
void setbasepri(uint32_t basepri);

void task_switch();

void task_suspend(uint8_t task_id);
void task_unsuspend(uint8_t task_id);

void task_suspend_irq(uint8_t task_id);
void task_unsuspend_irq(uint8_t task_id);

void enter_critical();
void exit_critical();

uint8_t semaphore_request(uint8_t task_id);
void semaphore_post(uint8_t task_id,uint8_t semaphore);

//不进行任务调度的延时函数，以us最为单位
void os_delay(uint32_t nus);
#endif //MOS_MOS_H
