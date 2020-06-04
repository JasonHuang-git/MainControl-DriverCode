#include <bsp.h>

//按键初始化函数
void key_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIO时钟初始化 */
	RCC_AHB1PeriphClockCmd(KEY0_RCC_CLK | WK_UP_RCC_CLK, ENABLE);

	/* KEY0按键初始化 */
	GPIO_InitStructure.GPIO_Pin  = KEY0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(KEY0_PORT, &GPIO_InitStructure);

	/* WK_UP按键初始化 */
	GPIO_InitStructure.GPIO_Pin  = WK_UP_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(WK_UP_PORT, &GPIO_InitStructure);
}

/**
 * @name	  key_scan
 * @brief     按键扫描检测
 * @param	  mode：不支持连按（0），支持连按（1）
 * @return    宏定义的键值，KEY0_PRES或WKUP_PRES
 */
uint8_t key_scan(uint8_t mode)
{
	static uint8_t key_up = 1;	// 按键松开标志
	if (mode) key_up = 1;		// 支持连按
	if (key_up && (KEY0 == 1 || WK_UP == 1)) // 有按键按下
	{
		delay_ms(10);			// 按键去抖
		key_up = 0;
		if (KEY0 == 1) return KEY0_PRES;
		else if (WK_UP == 1) return WKUP_PRES;
	} else if (KEY0 == 0 && WK_UP == 0)key_up = 1;
	return 0;				    // 无按键按下
}

