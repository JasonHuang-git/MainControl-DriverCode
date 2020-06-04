#include <bsp_led.h>

/**
 * @name	  led_gpio_init
 * @brief     初始化LED1、LED2的I/O的工作方式
 * @param	  空
 * @return    空
 */
void led_gpio_init(void)
{    	 
    GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能GPIO时钟
    RCC_AHB1PeriphClockCmd(LED_CLK, ENABLE);   

    // 引脚配置初始化
    GPIO_InitStructure.GPIO_Pin   = LED1_PIN | LED2_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;            // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;        // 100MHz
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             // 上拉
    GPIO_Init(LED_PORT, &GPIO_InitStructure);                 // 初始化
	
	// LED2所在引脚设置低电平，LED1所在引脚输出高电平，灯灭
	GPIO_ResetBits(LED_PORT, LED2_PIN);
    GPIO_SetBits(LED_PORT, LED1_PIN);
}
