#ifndef __BSP_LED_H
#define __BSP_LED_H

#include <bsp.h>

// ʱ���Լ�GPIO���ź궨�壬������ֲ����ͬ�İ���
#define LED_CLK   RCC_AHB1Periph_GPIOD
#define LED_PORT  GPIOD
#define LED1_PIN  GPIO_Pin_12
#define LED2_PIN  GPIO_Pin_13

// LED�˿ڶ���
#define LED1 PDout(12)		// ��Ӧ˿ӡ D1
#define LED2 PDout(13)		// ��Ӧ˿ӡ D2	 

void led_gpio_init(void);	// LED GPIO��ʼ��	

#endif
