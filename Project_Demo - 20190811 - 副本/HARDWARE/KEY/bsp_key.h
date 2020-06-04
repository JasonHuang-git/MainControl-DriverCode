#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include <bsp.h>

// 如果使用4x4矩阵键盘，需要定义KEYBOARD这个宏
#define KEYBOARD

#ifdef KEYBOARD
// 矩阵键盘GPIO相关宏定义
#define KEY_BOARD_CLK  RCC_AHB1Periph_GPIOG
#define KEY_BOARD_PORT GPIOG
#define ROW_PIN_0	   GPIO_Pin_0
#define ROW_PIN_1	   GPIO_Pin_1
#define ROW_PIN_2	   GPIO_Pin_2
#define ROW_PIN_3	   GPIO_Pin_3
#define COL_PIN_0	   GPIO_Pin_4
#define COL_PIN_1	   GPIO_Pin_5
#define COL_PIN_2	   GPIO_Pin_6
#define COL_PIN_3	   GPIO_Pin_7

// 行列输入线位带操作宏定义
#define ROW0_IN		   PGin(0)
#define ROW1_IN		   PGin(1)
#define ROW2_IN		   PGin(2)
#define ROW3_IN		   PGin(3)

#define COL0_IN		   PGin(4)
#define COL1_IN		   PGin(5)
#define COL2_IN		   PGin(6)
#define COL3_IN		   PGin(7)

#endif

// 独立按键相关宏定义
#define S2_CLK		RCC_AHB1Periph_GPIOB
#define S2_PORT		GPIOB
#define S2_PIN		GPIO_Pin_2

#define S3_CLK		RCC_AHB1Periph_GPIOE
#define S3_PORT		GPIOE
#define S3_PIN		GPIO_Pin_6

// S2对应板子上丝印S2
// S3对应板子上丝印S3
#define S2		     PBin(2)
#define S3			 PEin(6)
#define S3_Val	     1
#define S2_Val		 2
#define ERROR		 0xff


// 4x4矩阵键盘相关函数
uint8_t keyboard_scan(uint8_t mode);
uint8_t key_code(uint8_t mode);
void init_keyboard(void);
extern void ResetCol(void);
extern void ResetRow(void);

// 独立按键相关函数
uint8_t key_scan(uint8_t mode);
void init_key(void);

#endif /* __BSP_KEY_H */
