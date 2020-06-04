#include <bsp.h>

/**
 * @name	  bsp_init
 * @brief     ��ʼ����������Ҫʹ�õ�������
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void bsp_init(void)
{
	NVIC_SetPriorityGrouping(2);				// �����ж����ȼ�����Ϊ2
	delay_init(168);							// ϵͳʱ��Ϊ168MHz��ʼ��
	uart_init(115200);							// ����1������λ115200��ʼ��
	led_gpio_init();							// �����ϵ�LED��ʼ��
	hmc472_init();								// HMC472A˥����I/O��ʼ��
	pe4302_gpio_init();							// PE4302˥����I/O��ʼ��
	LCD_Init();					    			// LCD��ʾ����ʼ��
	tp_dev.init();								// LCD���ݴ���оƬ��ʼ��
	init_key();									// ����������ʼ��
//	init_keyboard();							// 4x4������̳�ʼ��
//	Init_AD9959();								// DDS��ʼ��
//	ADF_Init();									// PLL��ʼ��
//	TIM3_Init();								// ��ʼ����ʱ��3���ṩ���Զ��׵��ƹ��ܵ�����  
//	ESP8266_Init();								// ESP8266��ʼ��
//	mb1504_init();								// MB1504��Ƶ����ʼ��
	bsp_InitDAC1();								// DAC1��ʼ��
	bsp_InitDAC2();								// DAC2��ʼ��
	bsp_InitADC(0);								// ADC1/2/3��ʼ��
//    init_ch9121();
    AD9833_Init();
}
