#include <bsp.h>
extern struct component cases_member;
extern struct component r1_status;
extern struct component r2_status;
extern struct component r3_status;
extern struct component r4_status;
extern struct component c1_status;
extern struct component c2_status;
extern struct component c3_status;

uint8_t key_old = 0;
/**
 * @name	  main
 * @brief     执行裸机程序
 * @param	  空
 * @return    int
 * @DateTime  2019-7-17
 */
int main(void)
{
    uint8_t str[50];
    uint8_t res = 0;
    uint8_t capres = 1;
    uint8_t loop = 1;
    uint8_t key_res = 0;
    // 系统初始化
    bsp_init();

    pe4302_a_set(0x25);
    AD9833_Set_Fre_app(1000);

    LCD_Clear(WHITE);
    LCD_ShowString(200, 150, 200, 24, 24, "Waiting...");
    amp_freq_curve();
    LCD_Clear(WHITE);
    LCD_ShowString(20, 150, 300, 24, 24, "S2:Bias Mode  S3:Advantance Mode");
    LCD_ShowString(20, 180, 300, 24, 24, "Please Select");
    while(1)
    {
        LED1 = !LED1;
        key_res = key_scan(0);
        
        if(key_res==S3_Val)
        {
            key_old = S3_Val;
            LCD_Clear(WHITE);
            LCD_ShowString(20, 150, 300, 24, 24, "Advantance Mode");
            
            res = res_judge();
            
            if(res)
            {
         //     delay_ms(15);
         //     res = res_judge();
                if(res)
                {
                    LCD_Clear(WHITE);
                    display_menu_B();
                    if((r1_status.status != nc) && (r1_status.status != shortcut))
                        LCD_ShowString(20 + 90, 30, 120, 16, 16, (uint8_t *)"OK");
                    else if(r1_status.status == nc)
                        LCD_ShowString(20 + 90, 30, 120, 16, 16, (uint8_t *)"Open");
                    else if(r1_status.status == shortcut)
                        LCD_ShowString(20 + 90, 30, 120, 16, 16, (uint8_t *)"Shortcut");

                    if(r2_status.status != nc && r2_status.status != shortcut)
                        LCD_ShowString(20 + 90, 60, 120, 16, 16, (uint8_t *)"OK");
                    else if(r2_status.status == nc)
                        LCD_ShowString(20 + 90, 60, 120, 16, 16, (uint8_t *)"Open");
                    else if(r2_status.status == shortcut)
                        LCD_ShowString(20 + 90, 60, 120, 16, 16, (uint8_t *)"Shortcut");

                    if(r3_status.status != nc && r3_status.status != shortcut)
                        LCD_ShowString(20 + 90, 90, 120, 16, 16, (uint8_t *)"OK");
                    else if(r3_status.status == nc)
                        LCD_ShowString(20 + 90, 90, 120, 16, 16, (uint8_t *)"Open");
                    else if(r3_status.status == shortcut)
                        LCD_ShowString(20 + 90, 90, 120, 16, 16, (uint8_t *)"Shortcut");

                    if(r4_status.status != nc && r4_status.status != shortcut)
                        LCD_ShowString(20 + 90, 120, 120, 16, 16, (uint8_t *)"OK");
                    else if(r4_status.status == nc)
                        LCD_ShowString(20 + 90, 120, 120, 16, 16, (uint8_t *)"Open");
                    else if(r4_status.status == shortcut)
                        LCD_ShowString(20 + 90, 120, 120, 16, 16, (uint8_t *)"Shortcut");
                   
                    capres = cut_cap();
                    if(capres)
                    {
                        if(c3_fres())
                        {
                            printf("并入C3\r\n");
                             LCD_ShowString(90, 240, 300, 16, 16, (uint8_t *)"C3 x2");
                        }
                        else if(c2_fres())
                        {
                            printf("并入C2\r\n");
                            LCD_ShowString(90, 240, 300, 16, 16, (uint8_t *)"C2 x2");
                        }
                    }
                    delay_ms(1000);
                }

            }
            else
            {
                LCD_ShowString(0, 0, 300, 24, 24, "Resitor Measure Error!");
            }
           delay_ms(500);
        }
        if(key_res==S2_Val)
        {
            LCD_Clear(WHITE);
            LCD_ShowString(20, 150, 300, 24, 24, "Normal Mode");
            delay_ms(250);
            key_old = S2_Val;
            LCD_Clear(WHITE);
            display_menu();
            update_display();
            delay_ms(2500);
        }
    }
}

