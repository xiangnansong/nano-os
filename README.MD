# 简易的嵌入式RTOS

## 概要

这个rtos用于stm32可以再stm32f429 discovery开发板上直接运行。之所以说简易，因为os的功能实现和使用都非常简单，而且有部分功能不太完整，会在以后一直开发完善，并且最好能加入文件系统和网络功能。虽然只有数百行代码，但是一个os所必须应有的一些核心功能都得到了实现。

### 开发环境

- cmake 3.5.1
- arm-linux-gnueabi-gcc 5.4
- stm32f429 discovery开发板
- archlinux系统
- clion

## 任务调度

常见的rtos比如ucos，freertos管理任务时采用的是链表的方式对任务块进行管理。考虑到复杂度，这里就直接使用数组的方式进行任务的管理，每个任务占据数组空间，数组的索引就是任务的优先级。这样的优点在于任务的调度非常迅速，调度器在调度优先级高的任务时不用运行额外的排序算法，实时性很高。缺点在于不适用于完成比较复杂的任务，需要占用的sram空间较大。主要的数组变量为：

```c
//任务堆栈空间，用于存储每个任务的寄存器变量等
uint32_t TASKARR[TASKNUM][STACKSIZE];
//用来标示任务是否处于睡眠状态
uint8_t STATEARR[TASKNUM / 8 + 1];
//用来标示任务是否处于阻塞状态
uint8_t SUSPENDARR[TASKNUM / 8 + 1];
//用来保存任务的栈地址
uint32_t PSPARR[TASKNUM];
//用来保存任务的睡眠时间
int32_t SLEEPARR[TASKNUM];
```

### 调度器

调度器是os的核心，调度器使用的是systick的中断处理函数进行调度。在os中具体实现为：

```c
//systick_count++; //调试使用
    //跳过空任务
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
            //信号量不为0，解除其阻塞状态
            if (SEMAPHOREARR[i].size > 0) {
                uint8_t a = (uint8_t) (i / 8);
                uint8_t b = (uint8_t) (i % 8);
                SUSPENDARR[a] &= ~(1 << b);
            }
        }
    }
    //寻找最高优先级的任务
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
    //若最高优先级的任务不是当前任务，进行任务切换
    next_task = num;
    if (curr_task != next_task) {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
```

## 任务通信

任务通信采用的是循环对列的方式，每一个任务都分配有一个任务队列：

```c
Queue SEMAPHOREARR[TASKNUM];
```

其中Queue是循环队列。当任务A要向任务B通信，只需要任务A向任务B的队列里发送信息。B任务在请求消息时，如果发现自身队列里没有信息则将自身处于阻塞状态，如果有信息就收取信息。这样的机制同样是过于简陋，因为接收方不会检查发送方，安全性很低。但是适用于对安全性要求不高，对运行效率和实时性要求较高的场合。

## 内存管理

内存管理也是使用最简单的方式实现。因为stm32没有mmu，不能使用虚拟地址映射，所以这里的内存管理的主要功能是进行更有效的管理外扩的sdram。包括：

- 内存使用情况的统计
- 最小碎片化的地址分配策略
- 删除地址