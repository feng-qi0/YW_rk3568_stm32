# YW_rk3568_stm32 项目

## 项目概述

这是一个基于STM32F103微控制器的嵌入式系统项目，集成了多种传感器和执行器，用于环境监测和控制。项目名称中的"YW"可能代表"温湿度"(Temperature and Humidity)，表明该项目重点关注环境参数的监测。

## 硬件平台

- 微控制器: STM32F103
- IDE: Keil uVision (由项目文件 `.uvprojx` 可知)
- 项目文件: `FirstDay_stm32.uvprojx`

## 功能模块

### 1. 温湿度监测 (AHT30)

- **传感器**: AHT30 数字温湿度传感器
- **接口**: I2C
- **引脚**:
  - SDA -> PB11
  - SCL -> PB10
- **功能**: 实时测量环境温湿度

**示例代码**:
```c
#include "stm32f10x.h"
#include "Delay.h"
#include "I2C_AHT30.h"
#include "OLED.h"

int main(void)
{
    OLED_Init();
    
    float humidity = 0;
    float temperature = 0;
    
    AHT30_Init();
    
    while(1)
    {
        measurement();  // 发送测量指令
        Get_information(&humidity, &temperature);  // 获取数据
        
        OLED_ShowNum(1, 1, (int)temperature, 2);  // 显示温度
        OLED_ShowNum(1, 7, (int)humidity, 2);     // 显示湿度
    }
}
```

### 2. 红外人体检测

- **传感器**: 红外人体感应模块
- **引脚**: PA4
- **功能**: 检测区域内是否有人

**示例代码**:
```c
#include "stm32f10x.h"
#include "Infrared.h"
#include "OLED.h"
#include "Delay.h"

int main(void)
{
    OLED_Init();
    InfraredInit(RCC_APB2Periph_GPIOA, GPIOA, GPIO_Pin_4);
    
    while(1)
    {
        uint8_t infrared = ReadInfrsred(GPIOA, GPIO_Pin_4);
        OLED_ShowNum(1, 1, infrared, 8);
        // 无人输出0，有人输出1
    }
}
```

### 3. 光照强度检测

- **传感器**: ADC光敏传感器
- **引脚**: PA6
- **功能**: 测量环境光照强度

**示例代码**:
```c
#include "stm32f10x.h"
#include "Light.h"
#include "OLED.h"
#include "Delay.h"

int main(void)
{
    OLED_Init();
    Light_Init();
    
    while(1)
    {
        uint8_t light = Light_ADC();
        OLED_ShowNum(1, 1, light, 3);  // 显示光敏值
    }
}
```

### 4. 电机控制

- **功能**: PWM控制直流电机
- **引脚**:
  - PA0 -> PWMA (PWM信号)
  - PB0 -> AIN1 (方向控制)
  - PB1 -> AIN2 (方向控制)
- **功能**: 控制电机速度和方向

**示例代码**:
```c
#include "stm32f10x.h"
#include "PWM_motor.h"
#include "Delay.h"

int main(void)
{
    PWMMotor_Init();
    
    while(1)
    {
        PWMMotor_SetSpeed(3000);  // 设置速度
        delayByMs(2000);
        
        PWMMotor_SetSpeed(5000);  // 设置速度
        delayByMs(2000);
        
        PWMMotor_Forward();       // 正转
        delayByMs(2000);
        
        PWMMotor_Reverse();       // 反转
        delayByMs(2000);
        
        PWMMotor_Stop();          // 停止
        delayByMs(2000);
    }
}
```

### 5. LED亮度控制

- **功能**: PWM控制LED亮度
- **引脚**: PA7
- **功能**: 调节LED亮度

**示例代码**:
```c
#include "stm32f10x.h"
#include "LED.h"
#include "Delay.h"

int main(void)
{
    LED_Init();
    
    while(1)
    {
        LED_SetBright(0);     // 最暗
        delayByMs(3000);
        
        LED_SetBright(2000);  // 较亮
        delayByMs(3000);
    }
}
```

### 6. 蜂鸣器控制

- **功能**: 控制蜂鸣器发声
- **引脚**: PA5
- **功能**: 发出提示音或警报声

**示例代码**:
```c
#include "stm32f10x.h"
#include "Buzzer.h"
#include "Delay.h"

int main(void)
{
    Buzzer_Init();
    
    while(1)
    {
        Buzzer_Beep();        // 响铃
        delayByMs(3000);
        
        Buzzer_Close();       // 停止响铃
        delayByMs(30000);     // 等待30秒
    }
}
```

## 项目结构

```
YW_rk3568_stm32/
├── Hardware/                 # 硬件驱动层
│   ├── Buzzer.c/h            # 蜂鸣器驱动
│   ├── Delay.c/h             # 延时函数
│   ├── I2C.c/h               # I2C通信底层
│   ├── I2C_AHT30.c/h         # AHT30温湿度传感器
│   ├── Infrared.c/h          # 红外传感器
│   ├── LED.c/h               # LED控制
│   ├── Light.c/h             # 光敏传感器
│   ├── OLED.c/h              # OLED显示屏
│   ├── OLED_Font.h           # OLED字体库
│   └── PWM_motor.c/h         # PWM电机控制
├── User/                     # 用户应用层
│   └── Main.c                # 主程序入口
├── .vscode/                  # VSCode配置
├── DebugConfig/              # 调试配置
├── Listings/                 # 编译列表文件
├── Objects/                  # 编译对象文件
├── RTE/                      # 运行时环境
├── FirstDay_stm32.uvprojx    # Keil项目文件
└── README.md                 # 项目说明文档
```

## 使用方法

1. 打开Keil uVision
2. 加载 `FirstDay_stm32.uvprojx` 项目文件
3. 根据需要修改 `Main.c` 中的应用代码
4. 编译并下载到STM32F103开发板
5. 连接相应的传感器和执行器硬件
6. 上电运行

## 注意事项

- 确保硬件连接正确，特别是电源和地线
- 检查引脚分配是否与实际硬件一致
- 在使用前请校准传感器以获得准确读数
- 根据实际需求调整PWM频率和占空比参数
