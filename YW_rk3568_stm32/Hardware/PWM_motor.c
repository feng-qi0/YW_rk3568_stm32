#include "PWM_motor.h"

void PWMMotor_Init(void){
	
		//PWM初始化
    // 1:开时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
    // 2:配置时钟源
    TIM_InternalClockConfig(TIM2);
	
    // 3:配置时基单元
    TIM_TimeBaseInitTypeDef TimeBaseStruct;
    TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TimeBaseStruct.TIM_Period = 10000 - 1;     // 10000
    TimeBaseStruct.TIM_Prescaler = 144 - 1;    // 50HZ
    TimeBaseStruct.TIM_RepetitionCounter = 0x00;
    TIM_TimeBaseInit(TIM2, &TimeBaseStruct);

    // 4:配置比较输出单元
    TIM_OCInitTypeDef TIM_OC_Struct;
    TIM_OC_Struct.TIM_OCMode = TIM_OCMode_PWM1;    // CNT < CCR时输出高电平，否则输出低电平
    TIM_OC_Struct.TIM_OCPolarity = TIM_OCPolarity_High;    // 极性不反转
    TIM_OC_Struct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OC_Struct.TIM_Pulse = 3000;    // CCR
    TIM_OC1Init(TIM2, &TIM_OC_Struct); //将设定好的波输出到 PA0 引脚
	
		TIM_OC_Struct.TIM_Pulse = 3000;    // CCR
    TIM_OC2Init(TIM2, &TIM_OC_Struct); //将设定好的波输出到 PA1 引脚

    // 5: 通过GPIO输出PWM波
    GPIO_InitTypeDef GPIO_Struct;
    // 如果碰到，由硬件自己输出的，使用AF_PP是我们自己做的，使用 OUT_PP
    GPIO_Struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Struct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Struct);
    // 6: 启动定时器
    TIM_Cmd(TIM2, ENABLE);
		

    //正反转
		//1.正传反转控制引脚
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    //2.配置GPIO
    GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_PP;//推挽
    GPIO_Struct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_Struct);

    //3.初始化为正转
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}

//正转
void PWMMotor_Forward(void){
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}

//反转
void PWMMotor_Reverse(void){
    GPIO_SetBits(GPIOB, GPIO_Pin_1);
    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

//不转
void PWMMotor_Stop(void){
    GPIO_SetBits(GPIOB, GPIO_Pin_0);
    GPIO_SetBits(GPIOB, GPIO_Pin_1);
}

//设置速度
void PWMMotor_SetSpeed(int speed){
  TIM_SetCompare1(TIM2, speed);
}
