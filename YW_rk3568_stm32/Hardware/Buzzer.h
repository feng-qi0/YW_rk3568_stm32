#ifndef BUZZER_H
#define BUZZER_H
#include "stm32f10x.h"                  // Device header

//初始化
void Buzzer_Init(void);

//蜂鸣
void Buzzer_Beep(void);

//停止蜂鸣
void Buzzer_Close(void);

#endif
