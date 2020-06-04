#include <bsp_led.h>

/**
 * @name	  led_gpio_init
 * @brief     ��ʼ��LED1��LED2��I/O�Ĺ�����ʽ
 * @param	  ��
 * @return    ��
 */
void led_gpio_init(void)
{    	 
    GPIO_InitTypeDef GPIO_InitStructure;
	
	// ʹ��GPIOʱ��
    RCC_AHB1PeriphClockCmd(LED_CLK, ENABLE);   

    // �������ó�ʼ��
    GPIO_InitStructure.GPIO_Pin   = LED1_PIN | LED2_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;            // ��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;        // 100MHz
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             // ����
    GPIO_Init(LED_PORT, &GPIO_InitStructure);                 // ��ʼ��
	
	// LED2�����������õ͵�ƽ��LED1������������ߵ�ƽ������
	GPIO_ResetBits(LED_PORT, LED2_PIN);
    GPIO_SetBits(LED_PORT, LED1_PIN);
}
