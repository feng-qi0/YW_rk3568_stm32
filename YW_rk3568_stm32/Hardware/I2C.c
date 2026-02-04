#include "I2C.h"

//1:主机控制 SCL 时钟线输出一个比特数据（主机控制SCL高低电平）
void I2C_WriteSCL(uint8_t data)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,(BitAction)data);
	delayByUs(5);
}


//2：主机读取 SCL 时钟线一个比特数据（主机读取SCL高低电平）；
//SCL是控制从设备的，我们是主机，读取SCL没有用处


//3：主机控制 SDA 输出一个比特数据（主机控制SDA高低电平）
void I2C_WriteSDA(uint8_t data)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_11,(BitAction)data);
	delayByUs(5);
}

//4：SDA 读取一个比特数据（主机读取SDA高低电平）
uint8_t I2C_ReadSDA(void)
{
	uint8_t data = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
	delayByUs(5);
	return data;
}

//初始化 PB10 - SCL,PB11 - SDA
void I2C_ModelInit(void)
{
	//1:开时钟GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	//2.配置GPIO
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Mode = GPIO_Mode_Out_OD;//开漏
	GPIO_Struct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;
	GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
	
	//I2C的SCL和SDA，初始应该都是高电平，输出1，实际就是高阻态。
	//默认把SCL和SDA拉到高电平
	GPIO_Init(GPIOB, &GPIO_Struct);
}

// 主机发送开始信号：
void I2C_ModelStart(void){
	I2C_WriteSCL(1);	// SCL时钟线开始时是高电平，1是高阻态，线的电平由外界上拉电阻拉高
	I2C_WriteSDA(1);	// SDA数据线开始时是高电平 。
	
	I2C_WriteSDA(0);	// 在SCL是高电平的情况下，先拉低SDA
	I2C_WriteSCL(0);	// 再拉低 SCL (SCL是低电平时，写入数据)
}

//主机结束信号--SCL在发送单个比特的情况下，都是以低电平结束；只有在所有数据发送完成的时候，是高电平结束的
void I2C_ModelStop(void)
{
	I2C_WriteSDA(0);
	//I2C_WriteSCL(0);
	
	//在SCL高的情况下，拉高SDA表示结束
	I2C_WriteSCL(1);
	I2C_WriteSDA(1);
}

// 主机发送一个字节的数据；
// 高位先行：1011 0010
void I2C_ModelSendByte(uint8_t data) {
    // 1: SCL低电平时，使主机可以放数据（SCL本来就是以低电平结束，所以这一句省掉）
	
    for(uint8_t i = 0; i < 8; i++){
        // 2: 主机放一个比特的数据（SCL低电平时）
        I2C_WriteSDA( (data & (0x80 >> i)) ? 1 : 0 );
        
		// 3: SCL切换到高电平
        I2C_WriteSCL(1);
        
		// 4: 从机读一个比特的数据（从机自己处理）
        
		// 5: SCL切换到低电平，准备加载另一个数据
        I2C_WriteSCL(0);
    }
}


// 主机接收一个字节的数据：假如从机发：1011 0010
uint8_t I2C_ModelReceiveByte(void) {
    // 用来存数据的变量
    uint8_t data = 0x00;
	
    // 1: SCL低电平时，使从机也能放数据（SCL本来就是以低电平结束，所以这一句省掉）
	
    // 2: 主机让从机操作SDA线(主机放开SDA的控制)
    I2C_WriteSDA(1);
	
    for(uint8_t i = 0; i < 8; i++) {
        // 3: 从机放一个比特的数据（从机自己处理）
        
		// 4: 拉高SCL
        I2C_WriteSCL(1);
        
		// 5: 主机读一个比特的数据
        if(I2C_ReadSDA()) {
            data |= (0x80 >> i);
        } else {
            // 就是读到了0，不用操作
        }
        
		// 6: 拉低SCL，准备加载另一个数据
        I2C_WriteSCL(0);
    }
    return data;
}

// 主机发送一个响应(应答)
// 主机在SCL低电平的时候，放一个0表示应答，放一个1表示非应答，从机在SCL拉高后读取
// 本质就是发送一个数据，把发送一个字节的数据代码修改即可。
void I2C_ModelSendAck(uint8_t ack){
    // 1: SCL低电平时
    
    // 2: 要SCL低电平时，主机放数据
    //I2C_WritesDA( data & (0x80 >> i) );
    I2C_WriteSDA( ack );
    
    // 3: SCL高电平时
    I2C_WriteSCL(1);
    
    // 4: 从机读数据
    
    // 5: 结束（低电平结束）
    I2C_WriteSCL(0);
}

// 主机接收一个响应（应答） 0表示应答，1表示非应答
uint8_t I2C_ModelReceiveAck(void) {
    // 1: SCL低电平时(都是以低电平结束，这里可以省略)
    
    // 2: 主机放开SDA数据线的控制，给从机操作
    I2C_WriteSDA(1); // 开漏模式，正常只能输出0；输出1，就代表放掉控制
    
    // 3: 从机放数据(从机操作，不用我们写)
    
    // 4: 拉高SCL，主机读数据
    I2C_WriteSCL(1);
    
    // 5: 主机读SDA应答
    uint8_t ack = I2C_ReadSDA();
    
    // 6: 拉低SCL
    I2C_WriteSCL(0);
    
    return ack;
}
