#include <bsp.h>

/**
 * @name	  bsp_init
 * @brief     初始化板子上需要使用到的外设
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
 */
void bsp_init(void)
{
	NVIC_SetPriorityGrouping(2);				// 设置中断优先级分组为2
	delay_init(168);							// 系统时钟为168MHz初始化
	uart_init(115200);							// 串口1波特率位115200初始化
	led_gpio_init();							// 板子上的LED初始化
	hmc472_init();								// HMC472A衰减器I/O初始化
	pe4302_gpio_init();							// PE4302衰减器I/O初始化
	LCD_Init();					    			// LCD显示屏初始化
	tp_dev.init();								// LCD电容触摸芯片初始化
	init_key();									// 独立按键初始化
//	init_keyboard();							// 4x4矩阵键盘初始化
//	Init_AD9959();								// DDS初始化
//	ADF_Init();									// PLL初始化
//	TIM3_Init();								// 初始化定时器3，提供测试二阶调制功能的脉冲  
//	ESP8266_Init();								// ESP8266初始化
//	mb1504_init();								// MB1504分频器初始化
	bsp_InitDAC1();								// DAC1初始化
	bsp_InitDAC2();								// DAC2初始化
	bsp_InitADC(0);								// ADC1/2/3初始化
//    init_ch9121();
    AD9833_Init();
}
