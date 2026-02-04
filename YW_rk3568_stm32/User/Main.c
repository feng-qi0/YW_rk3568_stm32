#include "stm32f10x.h"    
#include "OLED.h"
#include "Delay.h"
#include "LED.h"

int main(void)
{
	//OLED初始化
	OLED_Init();
	//控制LED亮度
	LED_SetBright(9000);
	//延时
	delayByMs(3000);
	while(1)
	{
		// //控制LED亮度
		// LED_SetBright(0);
		// //延时
		// delayByMs(3000);
		// //控制LED亮度
		// LED_SetBright(2000);
		// //延时
		// delayByMs(3000);
	}
}