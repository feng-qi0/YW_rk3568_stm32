#include "Delay.h"

//微妙延时
void delayByUs(uint32_t us)
{
	//us(微妙数) * 每微秒计数(没微秒震动次数) => 需要计数的总数；注：寄存器最多 24位
	SysTick -> LOAD = SystemCoreClock / 1000000 * us - 1;
	
	//标志位清零
	SysTick -> VAL = 0;
	
	//时钟使能
	SysTick -> CTRL = 0x00000005;
	
	//判断结束
	while(!(SysTick -> CTRL & 0x00010000));
}

//毫秒延时
void delayByMs(uint32_t ms)
{
	while(ms--)
	{
		delayByUs(1000);
	}
}

//秒延时
void delayByS(uint32_t s)
{
	while(s--)
	{
		delayByMs(1000);
	}
}
