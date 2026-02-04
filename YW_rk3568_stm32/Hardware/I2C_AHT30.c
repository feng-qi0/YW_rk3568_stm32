#include "I2C_AHT30.h"

//初始化协议
void AHT30_Init(void)
{
	//初始化
	I2C_ModelInit();
}

void measurement(void)
{
	I2C_ModelStart();
    I2C_ModelSendByte(AHT30_ADDR_WRITE);
	I2C_ModelReceiveAck();
    //指令
	uint8_t instruction[3] = {0xAC,0X33,0X00};
	
	//发送指令
	for(int i = 0;i < 3;i++)
	{
		//发送指令
	    I2C_ModelSendByte(instruction[i]);
	    
	    //接收ACK响应
	    I2C_ModelReceiveAck();
	}
	
    I2C_ModelStop();
    delayByMs(80);
}

// CRC8校验函数（直接使用说明书提供的代码）
unsigned char Calc_CRC8(unsigned char *message,unsigned char Num) {
	unsigned char i; 
	unsigned char byte;
	unsigned char crc =0xFF;
	
	for (byte = 0;byte<Num;byte++) {
		
		crc^=(message[byte]); 
		
		for(i=8;i>0;--i) {  
			if(crc&0x80) 
			{
				crc=(crc<<1)^0x31;
			}
			else 
			{
				crc=(crc<<1);	
            }
		}
	}
	return crc; 
}

//获取测量信息
void Get_information(float *humidity_h, float *temperature_h)
{
	uint8_t data[7], crc;
    uint32_t raw_humidity, raw_temp;
    
    // 读取6字节数据（状态字+湿度+温度）
    I2C_ModelStart();
    I2C_ModelSendByte(AHT30_ADDR_READ);
	// 接受从机的ACK
	I2C_ModelReceiveAck();
    for (uint8_t i = 0; i < 7; i++) {
        data[i] = I2C_ModelReceiveByte();
        // 发送应答
		I2C_ModelSendAck( (i == 6) ? 1 : 0 );
    }
    I2C_ModelStop();
	
	// CRC校验
    crc = Calc_CRC8(data, 6);  // 校验前6字节
    if (crc != data[6]) {
        // CRC错误处理
        return;
    }
	
	raw_humidity = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
    raw_temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
	
	//将数据传递给形参
	*humidity_h = (raw_humidity / 1048576.0f) * 100.0f;            // %RH
    *temperature_h = (raw_temp / 1048576.0f) * 200.0f - 50.0f;     // °C
}
