#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include <bsp.h>

/* 定义按键引脚相关 */
#define KEY0_RCC_CLK	RCC_AHB1Periph_GPIOA
#define KEY0_PORT		GPIOB
#define KEY0_PIN		GPIO_Pin_2
#define KEY0			PBin(2)

#define WK_UP_RCC_CLK	RCC_AHB1Periph_GPIOB
#define WK_UP_PORT		GPIOA
#define WK_UP_PIN		GPIO_Pin_0
#define WK_UP			PAin(0)

/* 定义返回的键值 */
#define KEY0_PRES		1
#define WKUP_PRES		2

/* 外部API函数 */
void key_gpio_init(void);
uint8_t key_scan(uint8_t mode);

#endif 
