#include <bsp.h>


// 初始化函数
void bsp_init(void)
{  
    NVIC_SetPriorityGrouping(2);    // 设置中断优先级分组为2
    delay_init(168);                // 初始系统主时钟为168MHz
    uart_init(115200);              // 设置调试串口波特率为115200

    LCD_Init();                     // 初始化LCD
    tp_dev.init();                  // 初始化触摸功能
    led_gpio_init();                // 初始化LED外设
    key_gpio_init();                // 初始化独立按键
    SPI1_Init();		   			// 初始化SPI
    flash_mode_app();               // 初始化W25Qxx
    font_init();                    // 初始化中文字库    
    my_mem_init(SRAMIN);            // 初始化内部内存池
    my_mem_init(SRAMCCM);           // 初始化CCM内存池
    fatfs_mount_app();              // 挂载SD卡和外部Flash
    show_sdcard_info_app();         // 显示SD卡相关信息
    lwip_config_app();              // DHCP方式获取IP，初始化网卡驱动
}
