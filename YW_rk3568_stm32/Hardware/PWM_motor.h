#ifndef _PWM_H_
#define _PWM_H_
#include "stm32f10x.h"                  // Device header

//extern int TIM2_Count;
//PWM
void PWMMotor_Init(void);

//正转
void PWMMotor_Forward(void);
//反转
void PWMMotor_Reverse(void);
//停止
void PWMMotor_Stop(void);

//设置速度
void PWMMotor_SetSpeed(int speed);

#endif
