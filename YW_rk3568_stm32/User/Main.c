#include "stm32f10x.h"    
#include "OLED.h"
#include "Delay.h"
#include "Buzzer.h"

int main(void)
{
	//初始化蜂鸣器
	Buzzer_Init();
	
	while(1)
	{
		//响铃
		Buzzer_Beep();
		delayByMs(3000);
		//停止响铃
		Buzzer_Close();
		delayByMs(30000);
	}

}
