//
// Created by song on 17-4-12.
//

#include "string.h"
uint8_t strlen(uint8_t* s){
    uint8_t len = 0;
    while (*(s+len)!=0){
        len++;
    }
    return len;
}