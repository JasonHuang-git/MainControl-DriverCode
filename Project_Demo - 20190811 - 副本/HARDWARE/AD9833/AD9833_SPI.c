#include "AD9833_SPI.h"						


//-----------------------------------------------------------------
//
// ��������: AD9833 IO������
// ��ڲ���: ��
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: RCC_APB2PeriphClockCmd();GPIO_Init();   
// ע������: ����������Ϊ�������
//			���� 	����
//			SDATA   	PC10	
//			SCLK			PC12	
//			FSUNC			PC14	
//-----------------------------------------------------------------
void GPIO_AD9833_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	// ʹ��ʱ��
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
// ��������: SPIͨѶ�߳�ʼ������
// ��ڲ���: ��
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: ��
// ע������: 
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
// ��������: ͨ��SPI��ʽд����
// ��ڲ���: ddara��Ҫд���16λ�Ĳ���
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: ��   
// ע������: ��
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
// ��������: AD9833��ʼ������
// ��ڲ���: ��
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPI_Init(); AD9833_SetRegisterValue(), 	
// ע������: ��
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
// ��������: AD9833��λ����
// ��ڲ���: ��
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: AD9833_SetRegisterValue(), 			
// ע������: ��
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
// ��������: AD9833�����λ����
// ��ڲ���: ��
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: AD9833_SetRegisterValue(), 
// 					
// ע������: ��
//-----------------------------------------------------------------
void AD9833_ClearReset(void)
{
	AD9833_SetRegisterValue(AD9833_REG_CMD);
}

//-----------------------------------------------------------------
// void AD9833_SetRegisterValue(unsigned short regValue)
//-----------------------------------------------------------------
//
// ��������: ����AD9833�Ĵ�����ֵ
// ��ڲ���: regValue��Ҫ���üĴ�����ֵ
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: SPI_Write(), 
// 					
// ע������: ��
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
// ��������: AD9833 ����Ƶ�ʼĴ���
// ��ڲ���: reg��Ƶ�ʼĴ���д�� wave_type���������� val��ֵ
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: AD9833_SetRegisterValue(), 
// 					
// ע������: ��
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
// ��������: ����AD9833����λ�Ĵ�����ֵ
// ��ڲ���: reg����λ�Ĵ��� val��ֵ
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: AD9833_SetRegisterValue(),
// ע������: ��
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
// ��������: ����AD9833��Ƶ�ʣ���λ���������� ��ֵ
// ��ڲ���: freq��Ƶ�� phase����λ type����������
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: AD9833_SetRegisterValue(),
// ע������: ��
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
// ��������: ����AD9833�Ĳ�������
// ��ڲ���: type����������
// ���ز���: ��
// ȫ�ֱ���: ��
// ����ģ��: AD9833_SetRegisterValue(),
// ע������: ��
//-----------------------------------------------------------------
void AD9833_SetWave(unsigned short type)
{
	AD9833_SetRegisterValue(type);
}


//-----------------------------------------------------------------
// unsigned long freq_trl_word (unsigned long freq)
//-----------------------------------------------------------------
//
// ��������: ����Ƶ�ʿ�����
// ��ڲ���: freq��Ҫ�����Ƶ����ֵ��ʮ���ƣ�
// ���ز���: Ƶ�ʿ�����
// ȫ�ֱ���: ��
// ����ģ��: ��
// ע������: 
//			
//-----------------------------------------------------------------
unsigned long freq_trl_word (unsigned long freq)
{
	return (unsigned long )(freq * 10.73742);				
	// ���Ƶ��ΪFmclk/��2^28��*freq    freq����Ƶ�ʿ����֣�Fmclk����25MHZ����
}
