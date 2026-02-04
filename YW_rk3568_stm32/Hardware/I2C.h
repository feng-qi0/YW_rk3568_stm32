#ifndef _I2C_H
#define _I2C_H
#include "Delay.h"
//低电平 主机和从机 -> 数据加载（将数据化为高或低电平，加载到引脚）
//高电平 主机和从机 -> 数据接收（半双工；一个写，那另一个只能读）
//流程：低电平 -> 将数据化为高或低电平，加载到引脚；高电平 -> 接收加载好的数据；

//1:主机控制 SCL 时钟线输出一个比特数据（主机控制SCL高低电平）
void I2C_WriteSCL(uint8_t data);


//3：主机控制 SDA 输出一个比特数据（主机控制SDA高低电平）
void I2C_WriteSDA(uint8_t data);


//4：SDA 读取一个比特数据（主机读取SDA高低电平）
uint8_t I2C_ReadSDA(void);


//初始化
void I2C_ModelInit(void);


// 主机发送开始信号：
void I2C_ModelStart(void);


//主机结束信号--SCL在所有的情况下，都是以低电平结束，只有在结束的时候，是高电平结束的
void I2C_ModelStop(void);


// 主机发送一个字节的数据；
// 高位先行：1011 0010
void I2C_ModelSendByte(uint8_t data);


// 主机接收一个字节的数据：假如从机发：1011 0010
uint8_t I2C_ModelReceiveByte(void);


// 主机发送一个响应(应答)
// 主机在SCL低电平的时候，放一个0表示应答，放一个1表示非应答，从机在SCL拉高后读取
// 本质就是发送一个数据，把发送一个字节的数据代码修改即可。
void I2C_ModelSendAck(uint8_t ack);


// 主机接收一个响应（应答） 0表示应答，1表示非应答
uint8_t I2C_ModelReceiveAck(void);

#endif 
