//
// Created by song on 17-2-11.
//

#ifndef APP_USART_H
#define APP_USART_H

#include "sys.h"
#include "stdio.h"
#define BUFLEN 100
void usart_init(uint32_t bound);
void sputc(uint8_t ch);
void send(uint8_t* s);
void sprint(char *s);
#endif //APP_USART_H
