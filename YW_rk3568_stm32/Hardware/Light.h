#ifndef _ADC_CONVERTER_H_
#define _ADC_CONVERTER_H_
#include "stm32f10x.h"              // Device header

//初始化
void Light_Init(void);

//获取模数转换结果
uint8_t Light_ADC(void);

#endif

