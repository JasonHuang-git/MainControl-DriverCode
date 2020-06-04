#include <bsp_lcd.h>
#include <font.h>
//#include <bmp.h>

// LCD�Ļ�����ɫ�ͱ���ɫ
u16 POINT_COLOR = 0x0000;	 // ������ɫ
u16 BACK_COLOR  = 0xFFFF;    // ����ɫ

// ����LCD��Ҫ����
// Ĭ��Ϊ����
_lcd_dev lcddev;

// д�Ĵ�������
// regval:�Ĵ���ֵ
void LCD_WR_REG(vu16 regval)
{
    regval = regval;		  // ʹ��-O2�Ż���ʱ��,����������ʱ
    LCD->LCD_REG = regval;    // д��Ҫд�ļĴ������
}

// дLCD����
// data:Ҫд���ֵ
void LCD_WR_DATA(vu16 data)
{
    data = data;			  // ʹ��-O2�Ż���ʱ��,����������ʱ
    LCD->LCD_RAM = data;
}

// ��LCD����
// ����ֵ:������ֵ
u16 LCD_RD_DATA(void)
{
    vu16 ram;			     // ��ֹ���Ż�
    ram = LCD->LCD_RAM;
    return ram;
}

/* �����ID���� */
u16 Read_ID(void)
{
    u16 id;
    u16 id1, id2;

    LCD_WR_REG(0xBF);
    id  = LCD_RD_DATA();
    id  = LCD_RD_DATA();
    id  = LCD_RD_DATA();
    id1 = LCD_RD_DATA();
    id2 = LCD_RD_DATA();
    id = (id1 << 8) + id2;
    return id;
}

// д�Ĵ���
// LCD_Reg:�Ĵ�����ַ
// LCD_RegValue:Ҫд�������
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;		    // д��Ҫд�ļĴ������
    LCD->LCD_RAM = LCD_RegValue;    // д������
}

// ���Ĵ���
// LCD_Reg:�Ĵ�����ַ
// ����ֵ:����������
u16 LCD_ReadReg(u16 LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);		    // д��Ҫ���ļĴ������
    delay_us(5);
    return LCD_RD_DATA();		    // ���ض�����ֵ
}

// ��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD_Set_Window(0, lcddev.width - 1, 0, lcddev.height - 1); // ����������Ļ���ڴ�С
}

// LCDдGRAM
// RGB_Code:��ɫֵ
void LCD_WriteRAM(u16 RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;    // дʮ��λGRAM
}

/* write_memory_start */
void LCD_WriteREG_Prepare(void)
{
    LCD_WR_REG(0x2c);           // write_memory_start
}

// ��mdk -O1ʱ���Ż�ʱ��Ҫ����
// ��ʱi
void opt_delay(u8 i)
{
    while(i--);
}

// ��ȡ��ĳ�����ɫֵ
// x,y:����
// ����ֵ:�˵����ɫ
u16 LCD_ReadPoint(u16 x, u16 y)
{
    LCD_WR_REG(0x2a);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0xff);
    LCD_WR_REG(0x2b);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0xff);
    LCD_WR_REG(0x2E);
    x = LCD->LCD_RAM;
    x = LCD->LCD_RAM;
    LCD_WR_REG(0x2E);
    x = LCD->LCD_RAM;
    x = LCD->LCD_RAM;
    x = (x & 0xf800) | ((x & 0x00fc) << 3);
    y = LCD->LCD_RAM;

    return x | (y >> 11);
}

// LCD������ʾ
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0x11);   // �˳�˯��ģʽ
    LCD_WR_REG(0x29); 	// ������ʾ
}

// LCD�ر���ʾ
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0x28);	// �ر���ʾ
    LCD_WR_REG(0x10);   // ����˯��ģʽ
}

// ���ù��λ��
// Xpos:������
// Ypos:������
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
    /* Set X position */
    LCD_WR_REG(0x2a);           // set_column_address
    LCD_WR_DATA(Xpos >> 8);
    LCD_WR_DATA(Xpos & 0xFF);
    LCD_WR_DATA(lcddev.width >> 8);
    LCD_WR_DATA(lcddev.width & 0xFF);
    LCD_WR_REG(0x2b);           // set_page_address
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_DATA(Ypos & 0xFF);
    LCD_WR_DATA(lcddev.height >> 8);
    LCD_WR_DATA(lcddev.height & 0xFF);
}

// ����ʱ����
// ����
// x,y:����
// POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);		   // ���ù��λ��
    LCD_WriteREG_Prepare();	       // ��ʼд��GRAM
    LCD->LCD_RAM = POINT_COLOR;
}

// ��ʾ����ʱ����
// ���ٻ���
// x,y:����
// color:��ɫ
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_WR_REG(0x2a);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0xff);
    LCD_WR_DATA(x >> 8);
    LCD_WR_DATA(x & 0xff);
    LCD_WR_REG(0x2b);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0xff);
    LCD_WR_DATA(y >> 8);
    LCD_WR_DATA(y & 0xff);
    LCD_WR_REG(0x2c);
    LCD_WR_DATA(color);
}

// ����LCD��ʾ����
// dir:0,������1,����
void LCD_Display_Dir(u8 dir)
{
    if(!dir)			  
    {
        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x00);
        lcddev.dir = 0;	  // ����
        lcddev.width = 320;
        lcddev.height = 480;
    }
    else
    {
        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x60);
        lcddev.dir = 1;	  // ����
        lcddev.width = 480;
        lcddev.height = 320;
    }
}

//����LCD���Զ�ɨ�跽��
void LCD_Scan_Dir(u8 dir)
{
    
	u16 regval=0;
	u16 dirreg=0;
    
	if(lcddev.dir)//����ʱ
	{			   
		switch(dir)//����ת��
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	} 
    switch(dir)
    {
        case L2R_U2D://������,���ϵ���
            regval |= (0<<5)|(0<<6)|(0<<7); // 000
            break;
        case L2R_D2U://������,���µ���
            regval |= (0<<5)|(0<<6)|(1<<7); // 001
            break;
        case R2L_U2D://���ҵ���,���ϵ���
            regval |= (0<<5)|(1<<6)|(0<<7); // 010
            break;
        case R2L_D2U://���ҵ���,���µ���
            regval |= (0<<5)|(1<<6)|(1<<7); // 011
            break;	 
        case U2D_L2R://���ϵ���,������
            regval |= (1<<5)|(0<<6)|(0<<7); // 100
            break;
        case D2U_L2R://���µ���,������
            regval |= (1<<5)|(0<<6)|(1<<7); // 101 
            break;
        case U2D_R2L://���ϵ���,���ҵ���
            regval |= (1<<5)|(1<<6)|(0<<7); // 110
            break;
        case D2U_R2L://���µ���,���ҵ���
            regval |= (1<<5)|(1<<6)|(1<<7); // 111
            break;	 
    } 
    dirreg = 0x36;
    regval |= 1<<12; 
    LCD_WR_REG(0x2C);       // Back to Start Column 
    LCD_ReadReg(0x2E);
    LCD_WriteReg(dirreg,regval);
}     


/**********************************************
    ��������Lcd��ѡ����
    ���ܣ�ѡ��Lcd��ָ���ľ�������

    ע�⣺xStart��yStart��Xend��Yend������Ļ����ת���ı䣬λ���Ǿ��ο���ĸ���
    ��ڲ�����   xStart x�������ʼ��
                ySrart y�������ʼ��
                Xend   y�������ֹ��
                Yend   y�������ֹ��
    ����ֵ����
***********************************************/
void LCD_Set_Window(u16 Xstart, u16 Xend, u16 Ystart, u16 Yend)
{
    //HX8352-C
    LCD_WR_REG(0x2a);
    LCD_WR_DATA(Xstart >> 8);
    LCD_WR_DATA(Xstart & 0xff);
    LCD_WR_DATA(Xend >> 8);
    LCD_WR_DATA(Xend & 0xff);

    LCD_WR_REG(0x2b);
    LCD_WR_DATA(Ystart >> 8);
    LCD_WR_DATA(Ystart & 0xff);
    LCD_WR_DATA(Yend >> 8);
    LCD_WR_DATA(Yend & 0xff);

    LCD_WR_REG(0x2c);
}

// ��ʼ��lcd
// �ó�ʼ���������Գ�ʼ������ILI93XXҺ��,�������������ǻ���ILI9320��!!!
// �������ͺŵ�����оƬ��û�в���!
void LCD_Init(void)
{
    vu32 i = 0;

    GPIO_InitTypeDef  GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);            // ʹ��FSMCʱ��

    GPIO_InitStructure.GPIO_Pin = (3 << 0) | (3 << 4) | (0x1f << 7) | (3 << 14); // PD0,1,4,5,7,8,9,10,11,14,15 AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = (0X1FF << 7);                     // PE7~15,AF OUT
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15, GPIO_AF_FSMC);

    GPIO_SetBits(GPIOD, GPIO_Pin_4);		 // RD = 1
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 // WR = 1
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 //	CS = 1
    delay_ms(10);

    readWriteTiming.FSMC_AddressSetupTime = 0x02;	     // ��ַ����ʱ�䣨ADDSET��Ϊ2��HCLK 1/168M=6ns*2=12ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	     // ��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
    readWriteTiming.FSMC_DataSetupTime = 0x05;		     // ���ݱ���ʱ��Ϊ5��HCLK	=6*60=360ns
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A; // ģʽA

    writeTiming.FSMC_AddressSetupTime = 7;	             // ��ַ����ʱ�䣨ADDSET��Ϊ9��HCLK =54ns
    writeTiming.FSMC_AddressHoldTime = 0x00;	         // ��ַ����ʱ�䣨A
    writeTiming.FSMC_DataSetupTime = 8;		             // ���ݱ���ʱ��Ϊ6ns*9��HCLK=54ns
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 // ģʽA

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;                          // ��������ʹ��NE1��
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;        // ���������ݵ�ַ
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;                   // FSMC_MemoryType_SRAM;  //SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;          // �洢�����ݿ��Ϊ16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;      // FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	         // �洢��дʹ��
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;              // ��дʹ�ò�ͬ��ʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;             // ��дʱ��
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;                     // дʱ��

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  // ʹ��BANK1

    delay_ms(20);
    LCD_WR_REG(0xB0);
    LCD_WR_DATA(0x04);
    lcddev.id = Read_ID();

    printf("LCD ID: %x\r\n", lcddev.id); //��ӡLCD ID

    if(lcddev.id == 0X1529)
    {
        LCD_WR_REG(0x3A);
        LCD_WR_DATA(0x55);

        LCD_WR_DATA(0xB4);
        LCD_WR_DATA(0X00);

        LCD_WR_REG(0xC0);
        LCD_WR_DATA(0x03);  // 0013
        LCD_WR_DATA(0xDF);  // 480
        LCD_WR_DATA(0x40);
        LCD_WR_DATA(0x12);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x43);

        LCD_WR_REG(0xC1);   // frame frequency
        LCD_WR_DATA(0x05);  // BCn,DIVn[1:0
        LCD_WR_DATA(0x2f);  // RTNn[4:0]
        LCD_WR_DATA(0x08);  // BPn[7:0]
        LCD_WR_DATA(0x08);  // FPn[7:0]
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xC4);
        LCD_WR_DATA(0x63);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x08);

        LCD_WR_REG(0xC8);   // Gamma
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x16);
        LCD_WR_DATA(0x24);  // 26
        LCD_WR_DATA(0x30);  // 32
        LCD_WR_DATA(0x48);
        LCD_WR_DATA(0x3d);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x20);
        LCD_WR_DATA(0x14);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x04);

        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x16);
        LCD_WR_DATA(0x24);
        LCD_WR_DATA(0x30);
        LCD_WR_DATA(0x48);
        LCD_WR_DATA(0x3d);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x20);
        LCD_WR_DATA(0x14);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x04);

        LCD_WR_REG(0xC9);   // Gamma
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x16);
        LCD_WR_DATA(0x24);  // 26
        LCD_WR_DATA(0x30);  // 32
        LCD_WR_DATA(0x48);
        LCD_WR_DATA(0x3d);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x20);
        LCD_WR_DATA(0x14);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x04);

        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x16);
        LCD_WR_DATA(0x24);
        LCD_WR_DATA(0x30);
        LCD_WR_DATA(0x48);
        LCD_WR_DATA(0x3d);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x20);
        LCD_WR_DATA(0x14);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x04);

        LCD_WR_REG(0xB6);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xCA);   // Gamma
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x16);
        LCD_WR_DATA(0x24);  // 26
        LCD_WR_DATA(0x30);  // 32
        LCD_WR_DATA(0x48);
        LCD_WR_DATA(0x3d);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x20);
        LCD_WR_DATA(0x14);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x04);

        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x16);
        LCD_WR_DATA(0x24);
        LCD_WR_DATA(0x30);
        LCD_WR_DATA(0x48);
        LCD_WR_DATA(0x3d);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x20);
        LCD_WR_DATA(0x14);
        LCD_WR_DATA(0x0c);
        LCD_WR_DATA(0x04);

        LCD_WR_REG(0xD0);
        LCD_WR_DATA(0x95);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x08);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x3f);

        LCD_WR_REG(0xD1);
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x28);
        LCD_WR_DATA(0x40);

        LCD_WR_REG(0xE1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0xE2);
        LCD_WR_DATA(0x80);

        LCD_WR_REG(0x2A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x3F);

        LCD_WR_REG(0x2B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0xDF);

        LCD_WR_REG(0x11);
        delay_ms(120);
        LCD_WR_REG(0x29);

        LCD_WR_REG(0xC1);   // frame frequency
        LCD_WR_DATA(0x05);  // BCn,DIVn[1:0
        LCD_WR_DATA(0x2f);  // RTNn[4:0]
        LCD_WR_DATA(0x08);  // BPn[7:0]
        LCD_WR_DATA(0x08);  // FPn[7:0]
        LCD_WR_DATA(0x00);
    }
    else
    {
        printf("LCD��ʼ��ʧ�ܣ�\r\n");
        return;
    }
    LCD_Display_Dir(__DIR__);		// Ĭ��Ϊ����
    LCD_Scan_Dir(DFT_SCAN_DIR);
    LCD_Clear(BACK_COLOR);  
}

// ��������
// color:Ҫ���������ɫ
void LCD_Clear(u16 color)
{
    u32 index = 0;
    u32 totalpoint;
    totalpoint  = lcddev.width;
    totalpoint *= lcddev.height; 	        // �õ��ܵ���
    LCD_WriteRAM_Prepare();   		        // ��ʼд��GRAM
    for(index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}

// ��ָ����������䵥����ɫ
// (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
// color:Ҫ������ɫ
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
    u16 i, j;
    u16 xlen = 0;

    xlen = ex - sx + 1;
    for(i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);      	// ���ù��λ��
        LCD_WriteREG_Prepare();  	// ��ʼд�Ĵ���
        for(j = 0; j < xlen; j++)LCD->LCD_RAM = color;	// ��ʾ��ɫ
    }
}

// ��ָ�����������ָ����ɫ��
// (sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
// color:Ҫ������ɫ
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    u16 height, width;
    u16 i, j;
    width = ex - sx + 1; 			      // �õ����Ŀ��
    height = ey - sy + 1;			      // �߶�
    for(i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i);   	  // ���ù��λ��
        LCD_WriteREG_Prepare();
        for(j = 0; j < width; j++)LCD->LCD_RAM = color[i * width + j]; // д������
    }
}

// ����
// x1,y1:�������
// x2,y2:�յ�����
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;                         // ������������
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0)incx = 1;                   // ���õ�������
    else if(delta_x == 0)incx = 0;            //  ��ֱ��
    else {
        incx = -1;
        delta_x = -delta_x;
    }
    if(delta_y > 0)incy = 1;
    else if(delta_y == 0)incy = 0;            // ˮƽ��
    else {
        incy = -1;
        delta_y = -delta_y;
    }
    if( delta_x > delta_y)distance = delta_x; // ѡȡ��������������
    else distance = delta_y;
    for(t = 0; t <= distance + 1; t++ )       // �������
    {
        LCD_DrawPoint(uRow, uCol);            // ����
        xerr += delta_x ;
        yerr += delta_y ;
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

// ������
// (x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

// ��ָ��λ�û�һ��ָ����С��Բ
// (x,y):���ĵ�
// r    :�뾶
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);          // �ж��¸���λ�õı�־
    while(a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b);        //5
        LCD_DrawPoint(x0 + b, y0 - a);        //0
        LCD_DrawPoint(x0 + b, y0 + a);        //4
        LCD_DrawPoint(x0 + a, y0 + b);        //6
        LCD_DrawPoint(x0 - a, y0 + b);        //1
        LCD_DrawPoint(x0 - b, y0 + a);
        LCD_DrawPoint(x0 - a, y0 - b);        //2
        LCD_DrawPoint(x0 - b, y0 - a);        //7
        a++;
        // ʹ��Bresenham�㷨��Բ
        if(di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

// ��ָ��λ����ʾһ���ַ�
// x,y:��ʼ����
// num:Ҫ��ʾ���ַ�:" "--->"~"
// size:�����С 12/16/24
// mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp, t1, t;
    u16 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���

    num = num - ' '; // �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
    for(t = 0; t < csize; t++)
    {
        if(size == 12)temp = asc2_1206[num][t]; 	 	// ����1206����
        else if(size == 16)temp = asc2_1608[num][t];	// ����1608����
        else if(size == 24)temp = asc2_2412[num][t];	// ����2412����
        else return;								    // û�е��ֿ�
        for(t1 = 0; t1 < 8; t1++)
        {
            if(temp & 0x80)LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if(mode == 0)LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            if(y >= lcddev.height)return;		       // ��������
            if((y - y0) == size)
            {
                y = y0;
                x++;
                if(x >= lcddev.width)return;	       // ��������
                break;
            }
        }
    }
}

// m^n����
// ����ֵ:m^n�η�.
u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--)result *= m;
    return result;
}

// ��ʾ����,��λΪ0,����ʾ
// x,y :�������
// len :���ֵ�λ��
// size:�����С
// color:��ɫ
// num:��ֵ(0~4294967295);
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;
    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x + (size / 2)*t, y, ' ', size, 0);
                continue;
            } else enshow = 1;

        }
        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, 0);
    }
}

// ��ʾ����,��λΪ0,������ʾ
// x,y:�������
// num:��ֵ(0~999999999);
// len:����(��Ҫ��ʾ��λ��)
// size:�����С
// mode:
// [7]:0,�����;1,���0.
// [6:1]:����
// [0]:0,�ǵ�����ʾ;1,������ʾ.
void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;
    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                if(mode & 0X80)LCD_ShowChar(x + (size / 2)*t, y, '0', size, mode & 0X01);
                else LCD_ShowChar(x + (size / 2)*t, y, ' ', size, mode & 0X01);
                continue;
            } else enshow = 1;

        }
        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size, mode & 0X01);
    }
}

// ��ʾ�ַ���
// x,y:�������
// width,height:�����С
// size:�����С
// *p:�ַ�����ʼ��ַ
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0 = x;
    width += x;
    height += y;
    while((*p <= '~') && (*p >= ' ')) // �ж��ǲ��ǷǷ��ַ�!
    {
        if(x >= width) {
            x = x0;
            y += size;
        }
        if(y >= height)break;         // �˳�
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}

/**
  * @brief  Displays picture.
  * @param  pData: picture address.
  * @param  Xpos: Image X position in the LCD
  * @param  Ypos: Image Y position in the LCD
  * @param  Xsize: Image X size in the LCD
  * @param  Ysize: Image Y size in the LCD
  * @retval None
  */
void DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, u8 *pData)
{
	u32 index;
  u32 ImageSize = 0;
  u16 data;
	
	ImageSize = (Xsize * Ysize);
	
	LCD_Set_Window(Xpos,Xsize+Xpos-1,Ypos,Ysize+Ypos-1);
	for(index = 0; index < ImageSize; index++)
	{
		data=(*pData);
		data+=(*(pData+1))<<8;
		LCD_WR_DATA((u16)data);
		pData += 2;
	}
}
