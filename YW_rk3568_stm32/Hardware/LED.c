#include "LED.h"

void LED_Init(void)
{
    // 1. 开启所有必要的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   // AFIO时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   // TIM3时钟
    
    // 2. 配置GPIO为复用推挽输出
    GPIO_InitTypeDef GPIO_Struct;
    GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;     // 复用推挽输出
    GPIO_Struct.GPIO_Pin = GPIO_Pin_7;           // PA7 - TIM3_CH2
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Struct);
    
    // 4. 配置TIM3时基单元
    TIM_TimeBaseInitTypeDef TimeBaseStruct;
    TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;        // 时钟分频
    TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;    // 向上计数
    TimeBaseStruct.TIM_Period = 10000 - 1;                  // ARR
    TimeBaseStruct.TIM_Prescaler = 144 - 1;                 // PSC
    TimeBaseStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TimeBaseStruct);
    
    // 5. 配置PWM输出模式（通道2）
    TIM_OCInitTypeDef TIM_OC_Struct;
    TIM_OC_Struct.TIM_OCMode = TIM_OCMode_PWM1;          // PWM模式1
    TIM_OC_Struct.TIM_OCPolarity = TIM_OCPolarity_High;  // 高电平有效
    TIM_OC_Struct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC_Struct.TIM_Pulse = 8000;                      // 初始CCR值
    
    TIM_OC2Init(TIM3, &TIM_OC_Struct);
    
    // 6.使能预装载寄存器
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);   // 通道2预装载
    TIM_ARRPreloadConfig(TIM3, ENABLE);                 // ARR预装载
    
    // 7. 使能计数器
    TIM_Cmd(TIM3, ENABLE);
}

void LED_SetBright(uint16_t Brightness)
{
    if(Brightness > 9999) Brightness = 9999;
    TIM_SetCompare2(TIM3, Brightness);
}