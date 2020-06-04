#ifndef __BSP_ADC_H
#define __BSP_ADC_H	
#include <bsp.h>

// 宏定义
#define Vref REF_VOLATGE

void bsp_InitADC(uint8_t mode);
uint16_t Get_ADC_Average(uint8_t times);
uint16_t Get_Voltage(void);
void measure_select(uint8_t mode);
extern __IO uint16_t uhADCDualConvertedValue[1024*10];
uint16_t Get_ADC1_Average(uint8_t times);
uint16_t Get_ADC1_Voltage(void);
float output_res_calc(void);
float gain_calc(void);
void amp_freq_curve(void);
void AD9833_Set_Fre_app(uint32_t fre);
extern __IO uint16_t uhADCDualConver;
uint16_t Get_ADC2_Voltage(void) ;
uint16_t Get_ADC2_Average(uint8_t times);
void adc1_channel_init(uint8_t mode);
uint16_t Get_ADC1Voltage(void);         // ADC1通道7

#endif /* __BSP_ADC_H */















