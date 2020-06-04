#ifndef __BSP_LED_H
#define __BSP_LED_H

#include <bsp.h>

// 时钟以及GPIO引脚宏定义，便于移植到不同的板子
#define LED_CLK   RCC_AHB1Periph_GPIOD
#define LED_PORT  GPIOD
#define LED1_PIN  GPIO_Pin_12
#define LED2_PIN  GPIO_Pin_13

// LED端口定义
#define LED1 PDout(12)		// 对应丝印 D1
#define LED2 PDout(13)		// 对应丝印 D2	 

void led_gpio_init(void);	// LED GPIO初始化	

#endif
