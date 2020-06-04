#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include <bsp_ad9959.h>

/* ---------------------------------------------------------------------------------
	system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

	HCLK = SYSCLK / 1     (AHB1Periph)
	PCLK2 = HCLK / 2      (APB2Periph)
	PCLK1 = HCLK / 4      (APB1Periph)

	因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
	因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

	APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
	APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11
	例如：0	  
	TIM6 更新周期是 = TIM6CLK / （TIM_Period + 1）/（TIM_Prescaler + 1）= TIM6CLK/256
------------------------------------------------------------------------------------ */

#define CCR4_Val  1500
#define CCR3_Val  3000
#define CCR2_Val  6000
#define CCR1_Val  12000

#define AF_CLK      RCC_AHB1Periph_GPIOA
#define AF_PORT 	GPIOA
#define AF_PIN_0    GPIO_Pin_0
#define AF_PIN_1    GPIO_Pin_1
#define AF_PIN_2    GPIO_Pin_2
#define AF_PIN_3    GPIO_Pin_3

// 外部调用函数
void TIM2_Init(void);
void TIM3_Init(void);
void TIM4_Init(uint16_t step_time_ms);
void TIM2_Mode_Init(uint16_t arr,uint16_t psc);
void TIM2_GPIO_Init(void);
void TIM3_Mode_Init(uint16_t arr,uint16_t psc);
void TIM4_Mode_Init(uint16_t arr,uint16_t psc);
void DAC_TIM_Config(uint16_t arr, uint16_t psc);
void TIM1_Config(void);		// 用于ADC触发

#endif /* __BSP_TIMER_H */
