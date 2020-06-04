#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include <bsp.h>

/* ���尴��������� */
#define KEY0_RCC_CLK	RCC_AHB1Periph_GPIOA
#define KEY0_PORT		GPIOB
#define KEY0_PIN		GPIO_Pin_2
#define KEY0			PBin(2)

#define WK_UP_RCC_CLK	RCC_AHB1Periph_GPIOB
#define WK_UP_PORT		GPIOA
#define WK_UP_PIN		GPIO_Pin_0
#define WK_UP			PAin(0)

/* ���巵�صļ�ֵ */
#define KEY0_PRES		1
#define WKUP_PRES		2

/* �ⲿAPI���� */
void key_gpio_init(void);
uint8_t key_scan(uint8_t mode);

#endif 
