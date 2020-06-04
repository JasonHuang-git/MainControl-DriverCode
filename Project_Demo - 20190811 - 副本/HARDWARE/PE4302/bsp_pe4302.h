#ifndef __BSP_PE4302_H_
#define __BSP_PE4302_H_

#include <bsp.h>

// A组GPIO宏定义
#define A_PE4302_AHxCLK		RCC_AHB1Periph_GPIOB
#define A_PE4302_PORT		GPIOB
#define A_PE4302_DATA_PIN	GPIO_Pin_1
#define A_PE4302_CLK_PIN	GPIO_Pin_0
#define A_PE4302_CS_PIN		GPIO_Pin_3
#define B_PE4302_CS_PIN		GPIO_Pin_4
// B组GPIO宏定义
#define B_PE4302_AHxCLK		RCC_AHB1Periph_GPIOA
#define B_PE4302_PORT		GPIOA
#define B_PE4302_DATA_PIN	GPIO_Pin_12
#define B_PE4302_CLK_PIN	GPIO_Pin_11

// A part
#define A_PE_LE				PBout(3)
#define A_PE_CLK			PBout(0)
#define A_PE_DAT			PBout(1)
// B part
#define B_PE_LE				PBout(4)
#define B_PE_CLK			PAout(11)
#define B_PE_DAT			PAout(12)
// A组控制线位状态宏定义
#define A_PE_LE_EN      	A_PE_LE = 1
#define A_PE_LE_EN      	A_PE_LE = 1
#define A_PE_LE_DIS     	A_PE_LE = 0
#define A_PE_LE_DIS     	A_PE_LE = 0
#define A_PE_CLK_L        	A_PE_CLK = 0
#define A_PE_CLK_H        	A_PE_CLK = 1
#define A_PE_DAT_L        	A_PE_DAT = 0
#define A_PE_DAT_H        	A_PE_DAT = 1
// B组控制线位状态宏定义
#define B_PE_LE_EN      	B_PE_LE = 1
#define B_PE_LE_EN      	B_PE_LE = 1
#define B_PE_LE_DIS     	B_PE_LE = 0
#define B_PE_LE_DIS     	B_PE_LE = 0
#define B_PE_CLK_L        	B_PE_CLK = 0
#define B_PE_CLK_H        	B_PE_CLK = 1
#define B_PE_DAT_L        	B_PE_DAT = 0
#define B_PE_DAT_H        	B_PE_DAT = 1

// 用户外部调用函数
void pe4302_gpio_init(void);
void pe4302_a_set(uint8_t db);
void pe4302_b_set(uint8_t db);

#endif /* __BSP_PE4302_H_ */
