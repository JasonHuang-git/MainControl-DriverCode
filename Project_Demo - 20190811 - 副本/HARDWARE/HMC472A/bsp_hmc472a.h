#ifndef __HMC472A_H
#define __HMC472A_H

#include <bsp.h>

// ����ATTENUATIONΪ2��ʾ������˥������ע�͵���ʾֻ��һ��˥����
#define ATTENUATION

// A�� HMC472A GPIO��غ궨�壬����0~31.5dB˥��
#define A_HMC472A_CLK	RCC_AHB1Periph_GPIOC
#define A_HMC472A_PORT	GPIOC	
#define V5_Pin			GPIO_Pin_13
#define V4_Pin			GPIO_Pin_12
#define V3_Pin			GPIO_Pin_11
#define V2_Pin			GPIO_Pin_10
#define V1_Pin			GPIO_Pin_9
#define V0_Pin			GPIO_Pin_8

// ��������λ�������궨��
#define V5_GPIO_PIN 	PCout(13)
#define V4_GPIO_PIN 	PCout(12)
#define V3_GPIO_PIN 	PCout(11)
#define V2_GPIO_PIN 	PCout(10)
#define V1_GPIO_PIN 	PCout(9)
#define V0_GPIO_PIN 	PCout(8)

#ifdef ATTENUATION				
// B�� HMC472A GPIO��غ궨�壬����31.5~63dB˥��
#define B_HMC472A_CLK	RCC_AHB1Periph_GPIOE
#define B_HMC472A_PORT	GPIOE	
#define V11_Pin			GPIO_Pin_5
#define V10_Pin			GPIO_Pin_4
#define V9_Pin			GPIO_Pin_3
#define V8_Pin			GPIO_Pin_2
#define V7_Pin			GPIO_Pin_1
#define V6_Pin			GPIO_Pin_0

#define V11_GPIO_PIN 	PEout(5)
#define V10_GPIO_PIN 	PEout(4)
#define V9_GPIO_PIN 	PEout(3)
#define V8_GPIO_PIN 	PEout(2)
#define V7_GPIO_PIN 	PEout(1)
#define V6_GPIO_PIN 	PEout(0)

// ˥����Χ�궨��
#define MAX_MULTIPLE   126
#define A_MAX_MULTIPLE 63
#else
#define MAX_MULTIPLE   63
#endif

#define MIN_MULTIPLE   0

// ��λ������ֵ�궨��
#define GPIO_PIN_SET   1
#define GPIO_PIN_RESET 0
// ��������ֵ�궨��
#define FALSE 1
#define TRUE  0


// �ⲿ���ú���
void hmc472_init(void);				// ��ʼ��GPIO
uint8_t hmc472_ctr(float _db);		// 0.5dB��������˥�������������ǲ�����ģ�

#endif /* __HMC472A_H */
