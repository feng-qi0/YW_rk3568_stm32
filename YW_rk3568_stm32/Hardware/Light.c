#include "Light.h"
#include "Delay.h"  // 

void Light_Init(void) {
    // 1: 开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // GPIO
    GPIO_InitTypeDef GPIO_Struct;
    GPIO_Struct.GPIO_Mode = GPIO_Mode_AIN;      //
    GPIO_Struct.GPIO_Pin = GPIO_Pin_6;          //
    GPIO_Struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Struct);
    
    // 2:72M / 6 = 12MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    // 3: 参数:
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_1Cycles5);
    
    // 4: ADC:
    ADC_InitTypeDef ADC_Struct;
    ADC_Struct.ADC_ContinuousConvMode = DISABLE;          // ADC使能
    ADC_Struct.ADC_DataAlign = ADC_DataAlign_Right;       // 
    ADC_Struct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 
    ADC_Struct.ADC_Mode = ADC_Mode_Independent;           // 
    ADC_Struct.ADC_NbrOfChannel = 1;                      // 
    ADC_Struct.ADC_ScanConvMode = DISABLE;                // 
    ADC_Init(ADC1, &ADC_Struct);
    
    // 5: ADC
    ADC_Cmd(ADC1, ENABLE);
    
    // :ADC
    delayByUs(10);
    
    // 6:
    ADC_ResetCalibration(ADC1);                    // 
    while(ADC_GetResetCalibrationStatus(ADC1));    // 
     
    ADC_StartCalibration(ADC1);                    // 
    while(ADC_GetCalibrationStatus(ADC1));         // 
}

// 获取模数转换结果
uint8_t Light_ADC(void)
{
    //使用代码推动第一次转换
    ADC_SoftwareStartConvCmd(ADC1,ENABLE);
    
    //等待转化结果
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);

    //返回转换结果
    return ADC_GetConversionValue(ADC1);
}
