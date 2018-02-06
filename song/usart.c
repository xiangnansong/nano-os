//
// Created by song on 17-2-11.
//

#include "usart.h"

uint8_t uBuffer[BUFLEN + 2];
//串口设置的步骤
/** 使能时钟
 * 设置IO复用
 * 设置CR寄存器，从而设置 奇偶，停止位，过采样等
 * 设置BRR寄存器，从而设置波特率
 * 设置CR 分别打开接受，发送，接收中断，串口的开关
 * 设置接受优先级
 *
 *
 * */
void usart_init(uint32_t bound) {
    float temp;
    uint16_t z;
    uint16_t f;
    RCC->AHB1ENR |= 1 << 0;//是能GPIOA，与m3不同。gpio在AHB总线上
    RCC->APB2ENR |= 1 << 4;//开启USART时钟
    temp = (float) (90 * 1000000) / (bound * 16);
    z = (uint16_t) temp;
    f = (uint16_t) ((temp - z) * 16);
    z <<= 4;
    z += f;
    gpio_set(GPIOA, PIN9 | PIN10, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_50M, GPIO_PUPD_PU);
    gpio_af_set(GPIOA, 9, 7);
    gpio_af_set(GPIOA, 10, 7);
    //设置波特率
    USART1->BRR = z;
    //16倍过采样
    USART1->CR1 &= ~(1 << 15);
    //设置1位停止位
    USART1->CR2 &= ~(3 << 12);
    //禁止奇偶校验
    USART1->CR1 &= ~(1 << 10);
    //发送使能
    USART1->CR1 |= 1 << 3;
    //接收使能
    USART1->CR1 |= 1 << 2;    //串口接收使能
    //接收中断使能
    USART1->CR1 |= 1 << 5;        //接收缓冲区非空中断使能
    nvic_init(3, 0, USART1_IRQn, 4);//组2，最低优先级
    //uart开启
    USART1->CR1 |= 1 << 13;
}

void sputc(uint8_t ch) {
    while ((USART1->SR & 0X40) == 0);//循环发送,直到发送完毕
    USART1->DR = (uint8_t) ch;
}

void send(uint8_t *s) {
    for (int i = 0; i < s[BUFLEN]; ++i) {
        sputc(s[i]);
    }
    sputc('\n');
    uBuffer[BUFLEN + 1] = 0;
    uBuffer[BUFLEN] = 0;

}

void sprint(char *s) {

    for (int i = 0; i < strlen(s); ++i) {
        sputc(s[i]);
    }
    sputc('\n');

}


