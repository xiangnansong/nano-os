//
// Created by song on 17-5-7.
//

#ifndef C_STACK_STACK_H
#define C_STACK_STACK_H

#include "mos.h"

typedef struct Queue {
    uint8_t rear;
    uint8_t front;
    uint8_t size;
    uint8_t cap;
    uint8_t s[SEMAPHORECAP];
} Queue;

void queue_init(Queue *queue, uint8_t size);

void queue_in(Queue *queue, uint8_t data);

uint8_t queue_out(Queue *queue);

#endif //C_STACK_STACK_H
