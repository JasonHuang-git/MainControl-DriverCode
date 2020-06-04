#include <bsp.h>


// ��ʼ������
void bsp_init(void)
{  
    NVIC_SetPriorityGrouping(2);    // �����ж����ȼ�����Ϊ2
    delay_init(168);                // ��ʼϵͳ��ʱ��Ϊ168MHz
    uart_init(115200);              // ���õ��Դ��ڲ�����Ϊ115200

    LCD_Init();                     // ��ʼ��LCD
    tp_dev.init();                  // ��ʼ����������
    led_gpio_init();                // ��ʼ��LED����
    key_gpio_init();                // ��ʼ����������
    SPI1_Init();		   			// ��ʼ��SPI
    flash_mode_app();               // ��ʼ��W25Qxx
    font_init();                    // ��ʼ�������ֿ�    
    my_mem_init(SRAMIN);            // ��ʼ���ڲ��ڴ��
    my_mem_init(SRAMCCM);           // ��ʼ��CCM�ڴ��
    fatfs_mount_app();              // ����SD�����ⲿFlash
    show_sdcard_info_app();         // ��ʾSD�������Ϣ
    lwip_config_app();              // DHCP��ʽ��ȡIP����ʼ����������
}
