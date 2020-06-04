#include <app.h>
#include <stdint.h>

float result_sample[3] = {0.0,0.0,0.0};
float result_sample1[3] = {0.0,0.0,0.0};
float result_sample2[3] = {0.0,0.0,0.0};
float output_voltage = 0.0;

uint16_t ref_gain = 130;
uint16_t new_3db = 0;
uint16_t old_3db = 180;


error_case error_code;

// DAC1���һ��5k�����Ҳ���
void DAC_Test_app(void)
{
	g_DAC1.ulAMP = 2048;
	g_DAC1.ulFreq = 50000;
	dac1_SetSinWave(g_DAC1.ulAMP, g_DAC1.ulFreq);	
}

// FFT�����LCD��ʾ
void FFT_Test_app(void)
{
    uint16_t i;
    /* ʵ��FFT����Ӧ��ʾ�� */
    for(i = 0; i < 2048;)
    {
        testInput_fft_2048[i] = (float32_t)(uhADCDualConvertedValue[i]&0x00FF);
        i++;
        testInput_fft_2048[i] = (float32_t)(uhADCDualConvertedValue[i-1]>>8);
    }
    testOutput_fft_2048 = arm_rfft_fast_f32_app(testInput_fft_2048, 0); 
    for(i=0; i < 2048; i++)
    {
        LCD_DrawLine(480-i*2000/1024,10,480-i*2000/1024, 10+testOutput_fft_2048[i]/100);
    }
    delay_ms(200);
    LED1 = !LED1;
    LCD_Clear(WHITE);
}

// �������Ƶ��
void AD9833_Set_Fre_app(uint32_t fre)
{
    AD9833_SetFrequency(AD9833_REG_FREQ0, 0, freq_trl_word(fre));
}

void display_menu(void)
{
    uint16_t i;
    POINT_COLOR = DARKBLUE;
    // ����������ʾ����
    LCD_DrawRectangle(10,0,470,90);
    // ��ʾ����������ֵ
    LCD_ShowString(30,0,240,24,24,(uint8_t *)"Ri = 00000.0000 Ohm");
//    LCD_ShowNum(30+48,0,52,2,24);
//    LCD_ShowNum(30+84,0,0,2,24);
    // ��ʾ����������ֵ
    LCD_ShowString(30,30,240,24,24,(uint8_t *)"Ro = 00000.0000 Ohm");
//    LCD_ShowNum(30+48,30,52,2,24);
//    LCD_ShowNum(30+84,30,0,2,24);
    // ��ʾ�Ŵ�����ѹ����
    LCD_ShowString(30,60,200,24,24,(uint8_t *)"AV = 000.0000 V/V");
//    LCD_ShowNum(30+48,60,150,3,24);
//    LCD_ShowNum(30+96,60,25,2,24);
    // ��ʾ��Ƶ������������Ƶ��
//    LCD_ShowString(288,0,200,24,24,(uint8_t *)"Fh = 000.000 KHz");
//    LCD_ShowNum(240+48,0,150,3,24);
//    LCD_ShowNum(240+96,0,25,2,24);
    // ����������ʾ��Ƶ��������
    LCD_DrawLine_color(15,110,15,290,BLACK);
    LCD_DrawLine_color(15,290,460,290,BLACK);
    LCD_ShowString(20,300,100,16,16,(uint8_t *)"1 KHz");
    LCD_ShowString(222,300,100,16,16,(uint8_t *)"240 KHz");
    LCD_ShowString(400,300,100,16,16,(uint8_t *)"450 KHz");
}

void display_menu_B(void)
{
    LCD_ShowString(120,0,300,24,24,(uint8_t *)"Measure Test Result");
    LCD_ShowString(20,30,200,16,16,(uint8_t *)"R1 Status:      ");
    LCD_ShowString(20,60,200,16,16,(uint8_t *)"R2 Status:      ");
    LCD_ShowString(20,90,200,16,16,(uint8_t *)"R3 Status:      ");
    LCD_ShowString(20,120,200,16,16,(uint8_t *)"R4 Status:      ");
    
    LCD_ShowString(20,150,200,16,16,(uint8_t *)"C1 Status:      ");
    LCD_ShowString(20,180,200,16,16,(uint8_t *)"C2 Status:      ");
    LCD_ShowString(20,210,200,16,16,(uint8_t *)"C3 Status:      ");
    
}

struct component cases_member;
struct component r1_status;
struct component r2_status;
struct component r3_status;
struct component r4_status;
struct component c1_status;
struct component c2_status;
struct component c3_status;

// ����ADC3 ͨ��3����ֵ
float * Get_adc_amp(void)
    
{
    uint32_t val[171];
    uint32_t max_vol = 0;
    uint32_t min_vol = 0;
    uint16_t i;
    
    for(i=0;i<171;i++)
    {
        val[i] = Get_Voltage();
    }
    // �����ֵ
    max_vol = val[0];
    for(i=0;i<171;i++)
    {
        if(max_vol < val[i])
        {
            max_vol = val[i];
        }
    }
//    printf("%d\r\n",max_vol);
    // ����Сֵ
    min_vol = val[0];
    for(i=0;i<171;i++)
    {
        if(min_vol > val[i])
        {
            min_vol = val[i];
        }
    }
//    printf("%d\r\n",min_vol);
    result_sample[0] = 3.3*(max_vol - min_vol)/4095;
    result_sample[1] = 3.3*(max_vol - min_vol)/4095/2;
    result_sample[2] = 0.707*3.3*(max_vol - min_vol)/4095/2;

    return result_sample;
}

// ����ADC1 ͨ��1����ֵ
float * Get_adc1_amp(void)
    
{
    uint32_t val[171];
    uint32_t max_vol = 0;
    uint32_t min_vol = 0;
    uint16_t i;
    
    for(i=0;i<171;i++)
    {
        val[i] = Get_ADC1_Voltage();
    }

    // �����ֵ
    max_vol = val[0];
    for(i=0;i<171;i++)
    {
        if(max_vol < val[i])
        {
            max_vol = val[i];
        }
    }
//    printf("%d\r\n",max_vol);
    // ����Сֵ
    min_vol = val[0];
    for(i=0;i<171;i++)
    {
        if(min_vol > val[i])
        {
            min_vol = val[i];
        }
    }
//    printf("%d\r\n",min_vol);
    result_sample1[0] = 3.3*(max_vol - min_vol)/4095;
    result_sample1[1] = 3.3*(max_vol - min_vol)/4095/2;
    result_sample1[2] = 0.707*3.3*(max_vol - min_vol)/4095/2;
//    for(i=0;i<3;i++)
//        printf("-------------%f\r\n",result_sample1[i]);
//    for(i=0;i<171;i++)
//    {
//        printf("%d\r\n",val[i]);
//    }

    return result_sample1;
}

// ����ADC1 ͨ��1����ֵ
float * Get_adc1_ch7_amp(void)
    
{
    uint32_t val[171];
    uint32_t max_vol = 0;
    uint32_t min_vol = 0;
    uint16_t i;
    
    for(i=0;i<171;i++)
    {
        val[i] = Get_ADC1Voltage();
    }

    // �����ֵ
    max_vol = val[0];
    for(i=0;i<171;i++)
    {
        if(max_vol < val[i])
        {
            max_vol = val[i];
        }
    }
//    printf("%d\r\n",max_vol);
    // ����Сֵ
    min_vol = val[0];
    for(i=0;i<171;i++)
    {
        if(min_vol > val[i])
        {
            min_vol = val[i];
        }
    }
//    printf("%d\r\n",min_vol);
    result_sample2[0] = 3.3*(max_vol - min_vol)/4095;
    result_sample2[1] = 3.3*(max_vol - min_vol)/4095/2;
    result_sample2[2] = 0.707*3.3*(max_vol - min_vol)/4095/2;
//    for(i=0;i<3;i++)
//        printf("-------------%f\r\n",result_sample1[i]);
//    for(i=0;i<171;i++)
//    {
//        printf("%d\r\n",val[i]);
//    }

    return result_sample2;
}

void get_ADC2_amp()
{
    uint32_t val[171];
    uint32_t max_vol = 0;
    uint32_t min_vol = 0;
    uint16_t i;
    
    for(i=0;i<171;i++)
    {
        val[i] =  Get_ADC2_Voltage();
    }

    // �����ֵ
    max_vol = val[0];
    for(i=0;i<171;i++)
    {
        if(max_vol < val[i])
        {
            max_vol = val[i];
        }
    }
//    printf("%d\r\n",max_vol);
    // ����Сֵ
    min_vol = val[0];
    for(i=0;i<171;i++)
    {
        if(min_vol > val[i])
        {
            min_vol = val[i];
        }
    }
//    printf("%d\r\n",min_vol);
    result_sample1[0] = 3.3*(max_vol - min_vol)/4095;
    result_sample1[1] = 3.3*(max_vol - min_vol)/4095/2;
    result_sample1[2] = 0.707*3.3*(max_vol - min_vol)/4095/2;
//    for(i=0;i<3;i++)
//        printf("ADC2-------------%f\r\n",result_sample1[i]);
//    for(i=0;i<171;i++)
//    {
//        printf("%d\r\n",val[i]);
//    }
}

// V0--->���ڿ���ADC3�ɼ������������˺͵��˵��л�
// V1--->���ڿ��Ʋ�����������ֱͨ���л�
// V2--->���V1ʹ��
// V3--->ѡ������غͿ��ز���
// V4--->ѡ��V3������ź���һ·���뵽ADC
void measure_select(uint8_t mode)
{
    switch(mode)
    {
    case 0:
        V0_GPIO_PIN = 0;    // ѡͨ����Ƿ����뵽ADC
        V1_GPIO_PIN = 0;    // �ر�ֱͨģʽ
        V2_GPIO_PIN = 0;    // ���V1�ر�ֱͨ
        V3_GPIO_PIN = 0;
        V4_GPIO_PIN = 0;
        V5_GPIO_PIN = 0;    // 
        V6_GPIO_PIN = 0;
        V7_GPIO_PIN = 0;
        V8_GPIO_PIN = 0;
        break;
    case 1:
        V0_GPIO_PIN = 1;    // ѡͨ�����Ƿ����뵽ADC
        V1_GPIO_PIN = 0;    // �ر�ֱͨģʽ
        V2_GPIO_PIN = 0;    // ���V1�ر�ֱͨ
        V3_GPIO_PIN = 0;
        V4_GPIO_PIN = 0;
        V5_GPIO_PIN = 0;
        V6_GPIO_PIN = 0;
        V7_GPIO_PIN = 0;
        V8_GPIO_PIN = 0;
        break;
    case 2:
        V0_GPIO_PIN = 0;
        V1_GPIO_PIN = 1;    // ֱ��ģʽ�򿪣���������������
        V2_GPIO_PIN = 1;
        V3_GPIO_PIN = 1;    // �����������踺��
        V4_GPIO_PIN = 0;
        V5_GPIO_PIN = 0;
        V6_GPIO_PIN = 0;
        V7_GPIO_PIN = 0;
        V8_GPIO_PIN = 0;
        break;
    case 3:
        V0_GPIO_PIN = 0;
        V1_GPIO_PIN = 1;    // ֱ��ģʽ�򿪣���������������
        V2_GPIO_PIN = 1;
        V3_GPIO_PIN = 0;    // ����
        V4_GPIO_PIN = 1;
        V5_GPIO_PIN = 0;
        V6_GPIO_PIN = 0;
        V7_GPIO_PIN = 0;
        V8_GPIO_PIN = 0;
        break;
    case 4:                 // ɨƵģʽ
       V0_GPIO_PIN = 0;
       V1_GPIO_PIN = 1;
       V2_GPIO_PIN = 1; 
       V3_GPIO_PIN = 0;
       V4_GPIO_PIN = 0;
       V5_GPIO_PIN = 1;
       V6_GPIO_PIN = 0;
       V7_GPIO_PIN = 0;
       V8_GPIO_PIN = 0;
       break;
    case 5:         // �رղ��Ե�·�����룬������̬�����ָ��
        V0_GPIO_PIN = 0;
        V1_GPIO_PIN = 0;
        V2_GPIO_PIN = 0;
        V3_GPIO_PIN = 0;
        V4_GPIO_PIN = 0;
        V5_GPIO_PIN = 0;
        V6_GPIO_PIN = 1;
        V7_GPIO_PIN = 0;
        V8_GPIO_PIN = 1;
        break;
    case 6:         // �رղ��Ե�·�����룬������̬�����ָ��
        V0_GPIO_PIN = 0;
        V1_GPIO_PIN = 0;
        V2_GPIO_PIN = 0;
        V3_GPIO_PIN = 0;
        V4_GPIO_PIN = 0;
        V5_GPIO_PIN = 0;
        V6_GPIO_PIN = 1;
        V7_GPIO_PIN = 1;
        V8_GPIO_PIN = 1;
        break;
    }
}
// �����������
float input_res_calc()
{
    float Iref = 0;
    float vin_vpp = 0;
    float res;
    float tem;
    AD9833_Set_Fre_app(1000);
    // ��������Ƿ������
    measure_select(0);
    delay_ms(500);
    // �õ������ѹ
    Get_adc_amp();
    // ����õ�����
    tem = result_sample[0];
    Iref = result_sample[0]/AMP_FACTOR/REF_RES;
    printf("����Ƿŵ�����%f\r\n",Iref);
    // ���������Ƿŵ������
    measure_select(1);
    delay_ms(500);
    Get_adc_amp();
    vin_vpp = (result_sample[0]/AMP2_FACTOR) - (tem/AMP_FACTOR);
    printf("�����Ƿ������ѹ��%f\r\n",vin_vpp);    
    res = vin_vpp/Iref;
    printf("������裺0%f\r\n",res);   
    if(res<0)
        res = -res;
    return res;
    
}

// �����������
float output_res_calc(void)
{
    float tmp;
    float Iref = 0;
    float vin_vpp = 0;
    float res_out = 0;
    
    AD9833_Set_Fre_app(1000);
    // �л�Ϊֱ��ģʽ��������ģʽ���������
    measure_select(2);
    delay_ms(500);
    // �õ������ѹ
    Get_adc1_amp();
    tmp = result_sample1[0];
    // �õ��������
    Iref = result_sample1[0]/OUTREF_RES;
    // �л�Ϊֱ��ģʽ������
    measure_select(3);
    delay_ms(500);
    // �õ������ѹ
    Get_adc1_amp();
    {
        for(int i=0;i<1;i++)
            printf("-------------%f\r\n",result_sample1[i]);
    }
    output_voltage = result_sample1[0];
    printf("�����ص�ѹֵ = %f,   ���ص�ѹֵ = %f\r\n",tmp,result_sample1[0]);
    res_out = (result_sample1[0]-tmp)/Iref;
    printf("%f\r\n",res_out);
    
    return res_out;
}

// ��������
float gain_calc(void)
{
    float gain = output_voltage/0.02;
    printf("%f\r\n",gain);
    return gain;
}

// ���Ʒ�Ƶ�������ߣ��������޽�ֹƵ��
void amp_freq_curve()
{
    static uint8_t flag =1;
    int i;
    uint16_t max_val = 0;
    uint16_t val_3db_index = 0;
    float val_3db_f = 0.0;
    uint16_t val_3db = 0;
    uint16_t data[480];
    uint32_t freq = 1000;
    uint32_t step = 1000;
    
    measure_select(4);
    delay_ms(500);
     delay_ms(500);
     delay_ms(500);
    
    
    for(i=0;i<480;i++)
        data[i] = 0;
//    printf("-----------------------8888-------------");
    if(flag)
    {
        flag = 0;
        for(i=0;i<480;i++)
        {
            AD9833_Set_Fre_app(freq);
            freq = freq + step;
            delay_ms(10);
            
            data[i] = Get_ADC2_Voltage();
        }
    }
    else if(!flag)
    {
        for(i=0;i<480;i++)
        {
            AD9833_Set_Fre_app(freq);
            freq = freq + step;
            delay_ms(10);
            
            data[i] = Get_ADC2_Voltage();
        }
        max_val = data[1];
        for(i=2;i<480;i++)
        {
            if(max_val<data[i])
            {
                max_val = data[i];
            }
        }
        val_3db = max_val*0.707;

        for(i=2;i<480;i++)
        {
            if((data[i-1]>=val_3db) && (data[i]<=val_3db))
            {
                val_3db_index = (i+i-1)/2;
                val_3db_f = (i+i-1)/2;
                printf("%d KHz\r\n",val_3db_index);
            }
     //       printf("%d   %d    %d\r\n", data[i-1], data[i], val_3db);
        }
        
        for(i=1;i<450;i++)
        {
            LCD_Fast_DrawPoint(i+15,300-data[i]/10,RED);
        }
        
        LCD_DrawLine_color(val_3db_index,110,val_3db_index,290,BLUE);
        LCD_ShowString(val_3db_index+60,150,200,30,24,"   .   Khz");
        LCD_ShowNum(val_3db_index+60,150,val_3db_index,3,24);
        LCD_ShowNum(val_3db_index+108,150,(uint32_t)(val_3db_f*100)%100,2,24);
        
    }
}

uint16_t amp_freq_curve_new(void)
{
    static uint8_t flag =1;
    int i;
    uint16_t max_val = 0;
    uint16_t val_3db_index = 0;
    uint16_t val_3db = 0;
    uint16_t data[1000];
    uint32_t freq = 1000;
    uint32_t step = 1000;
    
    measure_select(4);
    delay_ms(500);
    
    
    for(i=0;i<1000;i++)
        data[i] = 0;

    if(flag)
    {
        flag = 0;
        for(i=0;i<1000;i++)
        {
            AD9833_Set_Fre_app(freq);
            freq = freq + step;
            delay_ms(10);
            
            data[i] = Get_ADC2_Voltage();
        }
    }
    else if(!flag)
    {
        for(i=0;i<1000;i++)
        {
            AD9833_Set_Fre_app(freq);
            freq = freq + step;
            delay_ms(10);
            
            data[i] = Get_ADC2_Voltage();
        }
        max_val = data[1];
        for(i=2;i<1000;i++)
        {
            if(max_val<data[i])
            {
                max_val = data[i];
            }
        }
        val_3db = max_val*0.707;

        for(i=2;i<1000;i++)
        {
            if((data[i-1]>=val_3db) && (data[i]<=val_3db))
            {
                val_3db_index = (i+i-1)/2;
                printf("%d KHz\r\n",val_3db_index);
            }
    //        printf("%d   %d    %d\r\n", data[i-1], data[i], val_3db);
        }
        
        return val_3db_index;
    }
}


// ����̶��жϴ���
int res_judge(void)
{
    uint16_t adc_val = 0;
    uint16_t i;
    uint16_t int_vol = 0;
    uint8_t flag = 0;
    float resin = 0.00;
    float voltage = 0.0;    
     printf("����������!\r\n");
    // ����ADC1 ͨ��7
    adc1_channel_init(1);
    delay_ms(200);

//    // �رղ��Ե�·������
    measure_select(5);
    delay_ms(500);
    voltage = Get_ADC1Voltage()*Vref/4095.0;
    int_vol = 1000*voltage;
    printf("intvol = %d\r\n",int_vol);
    r3_status.status = normal;
    r1_status.status = normal;
    r2_status.status = normal;
    r4_status.status = normal;
    if((int_vol>1850) && int_vol<1950)
    {
        error_code = r3_shortcut;
        printf("���ϣ�R3��·\r\n");
        LCD_ShowString(0,50,300,24,24,"R3 Shortcut!");
        r3_status.status = shortcut;
        return 0;
    }
    else if((int_vol<1850) && (int_vol>1700))
    {
        error_code = r1_shortcut;
        printf("���ϣ�R1��·\r\n");
        LCD_ShowString(0,50,300,24,24,"R1 Shortcut!");
        r1_status.status = shortcut;
        return 0;
    }
    else if((int_vol>600) && (int_vol<680))
    {
        error_code = r2_nc;
        printf("���ϣ�R2��·\r\n");
        LCD_ShowString(0,50,300,24,24,"R2 Open!");
        r2_status.status = nc;
        return 0;
    }
    else if((int_vol < 40 ))
    {
        // ����Ŵ�ģʽ
        measure_select(6);
        delay_ms(500);
        
        voltage = Get_ADC1Voltage()*Vref/4095.0;
        int_vol = 1000*voltage;
//        printf("\r\n����Ŵ�ģʽ   %d\r\n",int_vol);
        if(int_vol > 1800 && int_vol < 2000)
        {
            printf("���ϣ�R3��·\r\n");
            LCD_ShowString(0,50,300,24,24,"R3 Open!");
            r3_status.status = nc;
            return 0;
        }
        else if(int_vol<480)
        {
            r4_status.status = shortcut;
            printf("���ϣ�R4��·\r\n");
            LCD_ShowString(0,50,300,24,24,"R4 Shortcut!");
            return 0;
        }
        // �˳��Ŵ�ģʽ
        measure_select(5);
        delay_ms(500);
    }
    else
    {
        error_code = normals;
        printf("��̬������ɣ�\r\n");
    }
    if(error_code == normals)
    {
        resin = input_res_calc();
        printf("%f\r\n",resin);
        if(resin>25000.0 && resin<35250.0)
        {
            r1_status.status = nc;
            printf("���ϣ�R1��·\r\n");
            LCD_ShowString(0,50,300,24,24,"R1 Open!");
            return 0;
        }
        else if(resin>20000.0 && resin<25000.0)
        {
            measure_select(5);
            delay_ms(500);
            voltage = Get_ADC1Voltage()*Vref/4095.0;
            int_vol = 1000*voltage;
            if((int_vol>1600) && (int_vol< 1700))
            {
                r4_status.status = nc;
                printf("���ϣ�R4��·\r\n");
                LCD_ShowString(0,50,300,24,24,"R4 Open!");
            }
            return 0;
        }
        else if(resin>7500.0 && resin<9000.0)
        {
            r2_status.status = shortcut;
            printf("���ϣ�R2��·\r\n");
            LCD_ShowString(0,50,300,24,24,"R2 Shortcut!");
            return 0;
        }
    }
    // ���³�ʼ��ΪADC1 ͨ��2
    adc1_channel_init(0);
    delay_ms(200);
    return 1;
}

// �����ͨ�϶�
uint8_t cut_cap(void)
{
    float tem = 0;
    float Iref = 0;
    float out_res;
    float gain = 0;
    float val = 0;
    uint16_t db = 0;
    uint16_t mref_gain = 120;
    c1_status.status = normal;
    c2_status.status = normal;
    c3_status.status = normal;
    printf("������ݲ���!\r\n");
     AD9833_Set_Fre_app(1000);
    // ��������Ƿ������
    measure_select(0);
    delay_ms(500);
    // �õ������ѹ
    Get_adc_amp();
    // ����õ�����
    Iref = result_sample[0];
    printf("%f\r\n",Iref);
    if(Iref<0.1)
    {
        c1_status.status = nc;
        printf("C1���ݿ�·��\r\n");
        if(c1_status.status == nc)
            LCD_ShowString(20 + 90, 150, 300, 16, 16, (uint8_t *)"Open");
        else if(c1_status.status == shortcut)
            LCD_ShowString(20 + 90, 150, 300, 16, 16, (uint8_t *)"Shortcut");
        return 0;
    }
    if(c1_status.status != nc && c1_status.status != shortcut)
        LCD_ShowString(20 + 90, 150, 300, 16, 16, (uint8_t *)"OK");

    
    AD9833_Set_Fre_app(1000);
    // �л�Ϊֱ��ģʽ������
    measure_select(3);
    delay_ms(500);
    // �õ������ѹ
    Get_adc1_amp();
    output_voltage = result_sample1[0];
    gain = gain_calc(); 
    if(gain < mref_gain)
    {
        printf("gain = %f\r\n",gain);
        printf("ref_gain1 = %d\r\n",mref_gain);
        printf("C2���ݿ�·��\r\n");
        c2_status.status = nc;
        if(c2_status.status == nc)
            LCD_ShowString(20 + 90, 180, 300, 16, 16, (uint8_t *)"Open");
        else if(c2_status.status == shortcut)
            LCD_ShowString(20 + 90, 180, 300, 16, 16, (uint8_t *)"Shortcut");
        return 0;
    }
    if(c2_status.status != nc && c2_status.status != shortcut)
        LCD_ShowString(20 + 90, 180, 300, 16, 16, (uint8_t *)"OK");

    // ������֤
//    db = amp_freq_curve_new();
    AD9833_Set_Fre_app(250000);
    measure_select(4);
    delay_ms(500);
    val = 3.3*Get_ADC2_Average(5)/4095;
    printf("output_voltage ==%f\r\n",val);
////////////////////////////////////////////////    
//    printf("%d\r\n",db);
//    printf("%d\r\n",old_3db);
    if(val>1.0)
    {
        printf("C3���ݿ�·��\r\n");
        c3_status.status = nc;
        if(c3_status.status == nc)
            LCD_ShowString(20 + 90, 210, 300, 16, 16, (uint8_t *)"Open");
        else if(c3_status.status == shortcut)
            LCD_ShowString(20 + 90, 210, 300, 16, 16, (uint8_t *)"Shortcut");
        return 0;
    }
    if(c3_status.status != nc && c3_status.status != shortcut)
        LCD_ShowString(20 + 90, 210, 300, 16, 16, (uint8_t *)"OK");
    
    printf("��֤��ɣ�\r\n");
    return 1;
    
}

// 10uF
uint8_t c1_fres(void)
{
    float tem;
    AD9833_Set_Fre_app(20);
    // ��������Ƿ������
    measure_select(0);
    delay_ms(500);
    // �õ������ѹ
    Get_adc_amp();
    
    printf("C1-> %f\r\n", result_sample[0]);
    if(result_sample[0] > 1.0)
        return 1;               // ��ʾ������C1����
     return 0;
    
}

// 47uF
uint8_t c2_fres(void)
{
    AD9833_Set_Fre_app(500);
    measure_select(2);
    delay_ms(500);
    Get_adc1_amp();
    printf("-> %f\r\n",result_sample1[0]);
    if(result_sample1[0] > 1.7)
        return 1;               // ��ʾ������C2����
     return 0;
}

// 470pf
uint8_t c3_fres(void)
{
    AD9833_Set_Fre_app(143000);
    measure_select(3);
    delay_ms(500);
    Get_adc1_amp();
    printf("-> %f\r\n",result_sample1[0]);
    if(result_sample1[0] > 1.2)
        return 1;               // ��ʾ������C2����
     return 0;
}


// ������ʾ
void update_display(void)
{
    float res;
    float out_res;
    float gain;
    uint32_t shi;
    uint32_t ge;
    
    res = input_res_calc();
    out_res = output_res_calc();
    gain = gain_calc(); 
    
    measure_select(4);
   
    POINT_COLOR = RED;
    // �����������
    shi = (uint32_t)res;
    ge = (uint32_t)(res*10000)%10000;
    LCD_ShowNum(30+60,0,shi,5,24);
    LCD_ShowNum(30+132,0,ge,4,24);
    // �����������
    shi = (uint32_t)out_res;
    ge = (uint32_t)(out_res*10000)%10000;
    LCD_ShowNum(30+60,30,shi,5,24);
    LCD_ShowNum(30+132,30,ge,4,24);
    // ���µ�·����
    shi = (uint16_t)gain;
    ge = (uint32_t)(gain*10000)%10000;
    LCD_ShowNum(30+60,60,shi,3,24);
    LCD_ShowNum(30+108,60,ge,4,24);
    amp_freq_curve();
    delay_ms(500);

}
