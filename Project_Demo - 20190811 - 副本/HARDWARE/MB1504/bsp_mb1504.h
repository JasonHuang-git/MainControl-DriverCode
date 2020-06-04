#ifndef __BSP_MB1504_H
#define __BSP_MB1504_H

#include <bsp.h>

/*
							编程说明：
Control data “H” : Data is transferred into the 15-bit latch.
Control data “L” : Data is transferred into the 18-bit latch.

1、C: Control bit (control bit is set to low)
		|---|---------------|--------------------------------|
		| C | 1 2 3 4 5 6 7 | 8 9 10 11 12 13 14 15 16 17 18 |
		|---|---------------|--------------------------------|
		S8 to S18 : Divide ratio of programmable counter setting bits (16 to 2047)
		S1 to S7 : Divide ratio of swallow counter setting bits (0 to 127)
		Data is input from the MSB.
2、C: Control bit (control bit is set to high)
		|---|------------------------------------|---|
		| C |LSB 2 3 4 5 6 7 8 9 10 11 12 13 MSB |SW |
		|---|------------------------------------|---|
		SW: Divide ratio of prescaler setting bit
		SW=“H”: 32
		SW=“L” : 64
		S1 to S14: Divide ratio of programmable reference counter setting bits (8 to 16383)
*/


#define MB1504_GPIO_CLK		RCC_AHB1Periph_GPIOB
#define MB1504_GPIO_PORT	GPIOB
#define CLK_PIN				GPIO_Pin_0
#define DATA_PIN			GPIO_Pin_1
#define LE_PIN				GPIO_Pin_3

// 管脚宏定义
#define CLK					PBout(0)
#define DATA				PBout(1)
#define LE					PBout(3)

// 高低电平宏定义
#define CLK_0   			{PBout(0) = 0;}
#define CLK_1 				{PBout(0) = 1;}	
#define DATA_0				{PBout(1) = 0;}	
#define DATA_1  			{PBout(1) = 1;}  	
#define LE_0				{PBout(3) = 0;}	
#define LE_1    			{PBout(3) = 1;}  	

void mb1504_init(void);
void SendReferF(void);
void FrequenceToSend(uint16_t FrequenceD);

#endif /* __BSP_MB1504_H */
