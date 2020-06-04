#ifndef __BSP_H
#define __BSP_H

#define NULL 0
// DAC/ADC�ο���ѹ������ʵ�ʹ���ʱ������ѹ�޸������ֵ
#define REF_VOLATGE 				3.137

// ϵͳͷ�ļ���������
// ��������ͷ�ļ���ʹ������ԭ���ṩ�Ŀ⺯���Լ�λ�������������ģ�ʹ�ù���ֻ��Ҫ���ü���
#include <sys.h>
#include <delay.h>
#include <usart.h>
// ������׼Cͷ�ļ�
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
// DSP������ͷ�ļ�
#include <arm_math.h>
#include <arm_const_structs.h>
#include <math_helper.h>
#include <stm32_dsp.h>

// �û�ͷ�ļ���������
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
/*													�ⲿ��������							                     			  */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FFT������������
extern float32_t testInput_fft_2048[2048];
extern float32_t *testOutput_fft_2048;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*													�û�������������					                        			  */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void bsp_init(void);	// ����ϵͳ��ʼ������

#endif /* __BSP_H */
