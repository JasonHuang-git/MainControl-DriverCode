#include <bsp.h>

//������ʼ������
void key_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOʱ�ӳ�ʼ�� */
	RCC_AHB1PeriphClockCmd(KEY0_RCC_CLK | WK_UP_RCC_CLK, ENABLE);

	/* KEY0������ʼ�� */
	GPIO_InitStructure.GPIO_Pin  = KEY0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(KEY0_PORT, &GPIO_InitStructure);

	/* WK_UP������ʼ�� */
	GPIO_InitStructure.GPIO_Pin  = WK_UP_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(WK_UP_PORT, &GPIO_InitStructure);
}

/**
 * @name	  key_scan
 * @brief     ����ɨ����
 * @param	  mode����֧��������0����֧��������1��
 * @return    �궨��ļ�ֵ��KEY0_PRES��WKUP_PRES
 */
uint8_t key_scan(uint8_t mode)
{
	static uint8_t key_up = 1;	// �����ɿ���־
	if (mode) key_up = 1;		// ֧������
	if (key_up && (KEY0 == 1 || WK_UP == 1)) // �а�������
	{
		delay_ms(10);			// ����ȥ��
		key_up = 0;
		if (KEY0 == 1) return KEY0_PRES;
		else if (WK_UP == 1) return WKUP_PRES;
	} else if (KEY0 == 0 && WK_UP == 0)key_up = 1;
	return 0;				    // �ް�������
}

