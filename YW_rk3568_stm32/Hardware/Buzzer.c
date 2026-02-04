#include "Buzzer.h"

//初始化
void Buzzer_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //关闭响铃
    Buzzer_Close();
}

//响铃
void Buzzer_Beep(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

//关闭响铃
void Buzzer_Close(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
}
