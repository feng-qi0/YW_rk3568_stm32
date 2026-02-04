### 1.温湿度说明:

​			使用AHT30

​			引脚:  SDA->PB11

​					  SCL->PB10

​			测量温湿度

​			测试代码：

```c
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "I2C_AHT30.h"
#include "OLED.h"

int main(void)
{
	//初始化OLED
	OLED_Init();

	//定义湿度和温度
	float humidity = 0;
	float temperature = 0;

	//初始化AHT30
	AHT30_Init();

	//单片机循环
    while(1)
	{
		//发送测量指令
		measurement();

		//获取温度和湿度
		Get_information(&humidity, &temperature);

		//获取温度和湿度
		OLED_ShowNum(1, 1, (int)temperature, 2);
		OLED_ShowNum(1, 7, (int)humidity, 2);
	}
}
```



### 2.红外说明：

​    引脚：PA4 -> OD

​	测量有没有人

​	测试代码：

```c
#include "stm32f10x.h"                  // Device header
#include "Infrared.h"
#include "OLED.h"
#include "Delay.h"

int main(void)
{
	//初始化OLED
	OLED_Init();

	//初始化红外门引脚
	InfraredInit(RCC_APB2Periph_GPIOA,GPIOA,GPIO_Pin_4);

	//单片机循环
    while(1)
	{
		//创建一个接收红外的引脚的变量
		uint8_t infrared = 0;

		//读取红外的引脚电平
		infrared = ReadInfrsred(GPIOA,GPIO_Pin_4);

		// 输出
		OLED_ShowNum(1, 1, infrared, 8);
		/*
		如果没人,则输出0
		如果有人,则输出1
		*/
	}
}
```

### 3.光照强度：

引脚：PA6->OA

测量光照强度的ADC（待修正）：

```C
#include "stm32f10x.h"                  // Device header
#include "Infrared.h"
#include "OLED.h"
#include "Delay.h"
#include "Light.h"

int main(void)
{
	//初始化屏幕
	OLED_Init();
	
	//初始化光敏
	Light_Init();

	//单片机循环
    while(1)
	{
		//获取光敏值
		uint8_t light = Light_ADC();
		//显示光敏值
		OLED_ShowNum(1, 1, light, 3);
	}
}
```



### 4.电机控制

引脚：PA0->PWMA

​			B0->AIN1

​			B1->AIN2

电机控制测试：

```c
#include "stm32f10x.h"    
#include "OLED.h"
#include "Delay.h"
#include "PWM_motor.h"

int main(void)
{
	//初始化电机
	PWMMotor_Init();
	
	while(1)
	{
		//设置速度
		PWMMotor_SetSpeed(3000);
		//延时
		delayByMs(2000);
		//设置速度
		PWMMotor_SetSpeed(5000);
		//延时
		delayByMs(2000);
		//正转
		PWMMotor_Forward();
		//延时
		delayByMs(2000);
		//反转
		PWMMotor_Reverse();
		//延时
		delayByMs(2000);
		//停止
		PWMMotor_Stop();
		//延时
		delayByMs(2000);
	}
}
```

5.LED照明灯亮度控制



```c

```





