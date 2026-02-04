#include "infrared.h"

//初始化红外引脚
void InfraredInit(uint32_t RCC_APB2Periph,GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
	//为总线开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph,ENABLE);
	
	//配置
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Struct.GPIO_Pin = GPIO_Pin;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//绑定
	GPIO_Init(GPIOx,&GPIO_Struct);
	
}

//读取红外的引脚电平
uint8_t ReadInfrsred(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{
	return GPIO_ReadInputDataBit(GPIOx,GPIO_Pin);
}

