#ifndef __BSP_ADF4351_H_
#define __BSP_ADF4351_H_

#include <bsp.h>

// Ĭ���ⲿ����Ϊ100MHz��R��Ƶϵ��Ϊ4��f_PFD = 100/4 = 25MHz
// ����޸��ⲿ����Ļ�����Ҫ�����޸ļĴ���2��R��Ƶ����ֵ
// ��λMHz
#define f_PFD				25

// �Ĵ�����ַ
#define ADF4351_R0			((uint32_t)0X2C8018)
#define ADF4351_R1			((uint32_t)0X8029)
#define ADF4351_R2			((uint32_t)0X11EC2)
#define ADF4351_R3			((uint32_t)0X4B3)
#define ADF4351_R4			((uint32_t)0XEC80FC)
#define ADF4351_R5			((uint32_t)0X580005)

#define ADF4351_R1_Base		((uint32_t)0X8001)
#define ADF4351_R4_Base		((uint32_t)0X8C80FC)
#define ADF4351_R4_ON		((uint32_t)0X8C80FC)
#define ADF4351_R4_OFF		((uint32_t)0X8C88FC)
											 
#define ADF4351_RF_OFF		((uint32_t)0XEC801C)

#define ADF4351_PD_ON		((uint32_t)0X11E42)
#define ADF4351_PD_OFF		((uint32_t)0X11E02)

/*
*
*   CE      ��оƬʹ�ܣ��ߵ�ƽ��Ч
*   LE      ������ʹ�ܣ��ߵ�ƽ��Ч
*   CLK     ������ʱ�����룬��������Ч
*   DATA    �������������룬����������MSB���ȷ�ʽ���أ�����LSB��������λ
*	��ע    ���ⲿ����Ϊ100MHz
**/

// ����ADF_DATA���������ߵĲ�������
#define ADF_DATA_READ()  {GPIOG->MODER &= ~(3 << 13*2); GPIOG->MODER |= (0 << 13*2);}
#define ADF_DATA_WRITE() {GPIOG->MODER &= ~(3 << 13*2); GPIOG->MODER |= (1 << 13*2);}

// GPIO��غ궨��
#define ADF_GPIO_CLK                    RCC_AHB1Periph_GPIOG
#define ADF_GPIO_PORT                  	GPIOG

#define LE_GPIO_PIN                  	GPIO_Pin_15
#define CE_GPIO_PIN                     GPIO_Pin_14
#define DATA_GPIO_PIN					GPIO_Pin_13
#define SCK_GPIO_PIN                    GPIO_Pin_12

// λ������
#define ADF_CE          				PGout(14)
#define ADF_LE          				PGout(15)
#define ADF_CLK         				PGout(12)

// ��дʱע�������߲�������
#define ADF_DATA_OUT    				PGout(13)
#define ADF_DATA_IN     				PGin(13)

// �ⲿ���ú���
void ADF_Init(void);
void ADF4351_WriteFreq(float Fre);		//	(xx.x) M Hz
void ADF_SweepFre(float start_fre, float end_fre, float step, uint16_t step_time);

#endif /* __BSP_ADF4351_H_ */

