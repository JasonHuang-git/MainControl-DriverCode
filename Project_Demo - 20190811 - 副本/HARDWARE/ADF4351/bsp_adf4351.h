#ifndef __BSP_ADF4351_H_
#define __BSP_ADF4351_H_

#include <bsp.h>

// 默认外部晶振为100MHz，R分频系数为4，f_PFD = 100/4 = 25MHz
// 如果修改外部晶振的话，需要自行修改寄存器2中R分频器的值
// 单位MHz
#define f_PFD				25

// 寄存器地址
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
*   CE      ：芯片使能，高电平有效
*   LE      ：加载使能，高电平有效
*   CLK     ：串行时钟输入，上升沿有效
*   DATA    ：串行数据输入，串行数据以MSB优先方式加载，三个LSB用作控制位
*	备注    ：外部晶振为100MHz
**/

// 定义ADF_DATA串行数据线的操作方向
#define ADF_DATA_READ()  {GPIOG->MODER &= ~(3 << 13*2); GPIOG->MODER |= (0 << 13*2);}
#define ADF_DATA_WRITE() {GPIOG->MODER &= ~(3 << 13*2); GPIOG->MODER |= (1 << 13*2);}

// GPIO相关宏定义
#define ADF_GPIO_CLK                    RCC_AHB1Periph_GPIOG
#define ADF_GPIO_PORT                  	GPIOG

#define LE_GPIO_PIN                  	GPIO_Pin_15
#define CE_GPIO_PIN                     GPIO_Pin_14
#define DATA_GPIO_PIN					GPIO_Pin_13
#define SCK_GPIO_PIN                    GPIO_Pin_12

// 位带操作
#define ADF_CE          				PGout(14)
#define ADF_LE          				PGout(15)
#define ADF_CLK         				PGout(12)

// 读写时注意数据线操作方向
#define ADF_DATA_OUT    				PGout(13)
#define ADF_DATA_IN     				PGin(13)

// 外部调用函数
void ADF_Init(void);
void ADF4351_WriteFreq(float Fre);		//	(xx.x) M Hz
void ADF_SweepFre(float start_fre, float end_fre, float step, uint16_t step_time);

#endif /* __BSP_ADF4351_H_ */

