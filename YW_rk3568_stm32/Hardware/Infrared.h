#ifndef _INFRARED_H
#define _INFRARED_H
#include "stm32f10x.h"                  // Device header

//初始化红外引脚
void InfraredInit(uint32_t RCC_APB2Periph,GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

//读取红外的引脚电平
uint8_t ReadInfrsred(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif
