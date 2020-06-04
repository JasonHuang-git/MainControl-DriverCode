#ifndef __BSP_H
#define __BSP_H

#define NULL 0
// DAC/ADC参考电压，根据实际供电时测量电压修改下面的值
#define REF_VOLATGE 				3.137

// 系统头文件包含区域
// 下面三个头文件是使用正点原子提供的库函数以及位带操作而包含的，使用过程只需要调用即可
#include <sys.h>
#include <delay.h>
#include <usart.h>
// 包含标准C头文件
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
// DSP库所需头文件
#include <arm_math.h>
#include <arm_const_structs.h>
#include <math_helper.h>
#include <stm32_dsp.h>

// 用户头文件包含区域
#include <bsp_led.h>
#include <bsp_oled.h>
#include <bsp_spi.h>
#include <bsp_hmc472a.h>
#include <bsp_pe4302.h>
#include <bsp_mb1504.h>
#include <bsp_lcd.h>
#include <bsp_touch.h>
#include <bsp_key.h>
#include <bsp_ad9959.h>
#include <bsp_timer.h>
#include <bsp_uart4.h>
#include <bsp_uart6.h>
#include <bsp_dac.h>
#include <bsp_adc.h>
#include <bsp_adf4351.h>
#include <bsp_esp8266.h>
#include <bsp_dsp_fft.h>
#include <bsp_adc.h>
#include <bsp_ch9121.h>
#include <app.h>
#include <ad9833_spi.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*													外部变量区域							                     			  */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FFT输入和输出缓冲
extern float32_t testInput_fft_2048[2048];
extern float32_t *testOutput_fft_2048;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*													用户函数声明区域					                        			  */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void bsp_init(void);	// 板子系统初始化函数

#endif /* __BSP_H */
