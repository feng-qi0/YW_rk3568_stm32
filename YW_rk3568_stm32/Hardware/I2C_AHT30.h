#include "I2C.h"
#ifndef _I2C_AHT30_H
#define _I2C_AHT30_H
#include "Delay.h"

// 添加宏定义
#define AHT30_ADDR_WRITE  0x70            // 0x70
#define AHT30_ADDR_READ   0x71            // 0x71
#define AHT30_INIT_CMD    0xBA            // 初始化命令
#define AHT30_MEASURE_CMD 0xAC            // 测量命令

//初始化协议
void AHT30_Init(void);

//等待忙
void waitBusy(void);

//向芯片发送测量的指令
void measurement(void);

//获取测量信息
void Get_information(float *humidity_h, float *temperature_h);
//void Get_information(float *humidity, float *temperature);

#endif 

