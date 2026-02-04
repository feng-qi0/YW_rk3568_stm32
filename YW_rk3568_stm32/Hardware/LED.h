#ifndef LED_H
#define LED_H
#include "stm32f10x.h"  // Device header

//初始化
void LED_Init(void);

void LED_SetBright(uint16_t Brightness);

#endif
