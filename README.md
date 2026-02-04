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

3.光照强度