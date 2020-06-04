#include "AD9833_SPI.h"						


//-----------------------------------------------------------------
//
// 函数功能: AD9833 IO口配置
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: RCC_APB2PeriphClockCmd();GPIO_Init();   
// 注意事项: 各引脚设置为推挽输出
//			名称 	引脚
//			SDATA   	PC10	
//			SCLK			PC12	
//			FSUNC			PC14	
//-----------------------------------------------------------------
void GPIO_AD9833_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	// 使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  
	// 	SDATA	   		SCLK			FSUNC			 		    		 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//-----------------------------------------------------------------
// void SPI_Init (void)
//-----------------------------------------------------------------
//
// 函数功能: SPI通讯线初始化设置
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无
// 注意事项: 
//			
//-----------------------------------------------------------------
void M_SPI_Init (void)
{
	ADI_CS_HIGH;
	ADI_SCL_HIGH;
	ADI_SD_LOW;
}


//-----------------------------------------------------------------
// SPI_Write (unsigned short ddara)
//-----------------------------------------------------------------
//
// 函数功能: 通过SPI方式写数据
// 入口参数: ddara：要写入的16位的参数
// 返回参数: 无
// 全局变量: 无
// 调用模块: 无   
// 注意事项: 无
//					 
//-----------------------------------------------------------------
void SPI_Write (unsigned short ddara)
{
	unsigned char i;

	for (i = 0; i < 16; i ++)
	{
		if ((ddara & 0x8000) == 0x8000)
		{
			ADI_SD_HIGH;
		}
		else 
		{
			ADI_SD_LOW;
		}
		ADI_SCL_LOW;
        delay_us(10);
		ddara <<= 1;
		ADI_SCL_HIGH;
        delay_us(10);
	}
}

//-----------------------------------------------------------------
// unsigned char AD9833_Init(void)
//-----------------------------------------------------------------
//
// 函数功能: AD9833初始化函数
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPI_Init(); AD9833_SetRegisterValue(), 	
// 注意事项: 无
//
//-----------------------------------------------------------------
unsigned char AD9833_Init(void)
{
    GPIO_AD9833_Configuration();
    M_SPI_Init();
    AD9833_SetRegisterValue(AD9833_REG_CMD | AD9833_RESET);
    
    return (1);
}

//-----------------------------------------------------------------
// void AD9833_Reset(void)
//-----------------------------------------------------------------
//
// 函数功能: AD9833复位函数
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: AD9833_SetRegisterValue(), 			
// 注意事项: 无
// 		
//-----------------------------------------------------------------
void AD9833_Reset(void)
{
    AD9833_SetRegisterValue(AD9833_REG_CMD | AD9833_RESET);
}

//-----------------------------------------------------------------
// void AD9833_ClearReset(void)
//-----------------------------------------------------------------
//
// 函数功能: AD9833清除复位函数
// 入口参数: 无
// 返回参数: 无
// 全局变量: 无
// 调用模块: AD9833_SetRegisterValue(), 
// 					
// 注意事项: 无
//-----------------------------------------------------------------
void AD9833_ClearReset(void)
{
	AD9833_SetRegisterValue(AD9833_REG_CMD);
}

//-----------------------------------------------------------------
// void AD9833_SetRegisterValue(unsigned short regValue)
//-----------------------------------------------------------------
//
// 函数功能: 设置AD9833寄存器的值
// 入口参数: regValue：要设置寄存器的值
// 返回参数: 无
// 全局变量: 无
// 调用模块: SPI_Write(), 
// 					
// 注意事项: 无
//-----------------------------------------------------------------
void AD9833_SetRegisterValue(unsigned short regValue)
{
	ADI_CS_LOW;
    delay_us(10);
	SPI_Write(regValue);
	ADI_CS_HIGH;
    delay_us(10);
}

//-----------------------------------------------------------------
// AD9833_SetFrequency  ()
//-----------------------------------------------------------------
//
// 函数功能: AD9833 设置频率寄存器
// 入口参数: reg：频率寄存器写入 wave_type：波形类型 val：值
// 返回参数: 无
// 全局变量: 无
// 调用模块: AD9833_SetRegisterValue(), 
// 					
// 注意事项: 无
//-----------------------------------------------------------------
void AD9833_SetFrequency(unsigned short reg,  unsigned short wave_type,  unsigned long val)
{
	unsigned short  freqHi = reg;
	unsigned short  freqLo = reg;
	
	freqHi |= (val & 0xFFFC000) >> 14 ;
	freqLo |= (val & 0x3FFF);

	AD9833_SetRegisterValue(AD9833_B28);
	AD9833_SetRegisterValue(freqLo);
	AD9833_SetRegisterValue(freqHi);
	AD9833_SetRegisterValue(AD9833_REG_CMD | AD9833_B28 | wave_type);
}


//-----------------------------------------------------------------
// void AD9833_SetPhase(unsigned short reg, unsigned short val)
//-----------------------------------------------------------------
//
// 函数功能: 设置AD9833的相位寄存器的值
// 入口参数: reg：相位寄存器 val：值
// 返回参数: 无
// 全局变量: 无
// 调用模块: AD9833_SetRegisterValue(),
// 注意事项: 无
//-----------------------------------------------------------------
void AD9833_SetPhase(unsigned short reg, unsigned short val)
{
	unsigned short phase = reg;
	phase |= val;
	AD9833_SetRegisterValue(phase);
}

//-----------------------------------------------------------------
// void AD9833_Setup(unsigned short freq,
//				  unsigned short phase,
//			 	  unsigned short type)
//-----------------------------------------------------------------
//
// 函数功能: 设置AD9833的频率，相位，波形类型 的值
// 入口参数: freq：频率 phase：相位 type：波形类型
// 返回参数: 无
// 全局变量: 无
// 调用模块: AD9833_SetRegisterValue(),
// 注意事项: 无
//-----------------------------------------------------------------
void AD9833_Setup(unsigned short freq,
				  unsigned short phase,
			 	  unsigned short type)
{
	unsigned short val = 0;
	
	val = freq | phase | type;
	AD9833_SetRegisterValue(val);
}

//-----------------------------------------------------------------
// void AD9833_SetWave(unsigned short type)
//-----------------------------------------------------------------
//
// 函数功能: 设置AD9833的波形类型
// 入口参数: type：波形类型
// 返回参数: 无
// 全局变量: 无
// 调用模块: AD9833_SetRegisterValue(),
// 注意事项: 无
//-----------------------------------------------------------------
void AD9833_SetWave(unsigned short type)
{
	AD9833_SetRegisterValue(type);
}


//-----------------------------------------------------------------
// unsigned long freq_trl_word (unsigned long freq)
//-----------------------------------------------------------------
//
// 函数功能: 计算频率控制字
// 入口参数: freq：要计算的频率数值（十进制）
// 返回参数: 频率控制字
// 全局变量: 无
// 调用模块: 无
// 注意事项: 
//			
//-----------------------------------------------------------------
unsigned long freq_trl_word (unsigned long freq)
{
	return (unsigned long )(freq * 10.73742);				
	// 输出频率为Fmclk/（2^28）*freq    freq计算频率控制字，Fmclk采用25MHZ晶振
}
