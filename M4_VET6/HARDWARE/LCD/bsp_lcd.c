#include <bsp_lcd.h>
#include <font.h>
//#include <bmp.h>

// LCD的画笔颜色和背景色
u16 POINT_COLOR = 0x0000;	 // 画笔颜色
u16 BACK_COLOR  = 0xFFFF;    // 背景色

// 管理LCD重要参数
// 默认为竖屏
_lcd_dev lcddev;

// 写寄存器函数
// regval:寄存器值
void LCD_WR_REG(vu16 regval)
{
    regval = regval;		  // 使用-O2优化的时候,必须插入的延时
    LCD->LCD_REG = regval;    // 写入要写的寄存器序号
}

// 写LCD数据
// data:要写入的值
void LCD_WR_DATA(vu16 data)
{
    data = data;			  // 使用-O2优化的时候,必须插入的延时
    LCD->LCD_RAM = data;
}

// 读LCD数据
// 返回值:读到的值
u16 LCD_RD_DATA(void)
{
    vu16 ram;			     // 防止被优化
    ram = LCD->LCD_RAM;
    return ram;
}

/* 插入读ID函数 */
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

// 写寄存器
// LCD_Reg:寄存器地址
// LCD_RegValue:要写入的数据
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue)
{
    LCD->LCD_REG = LCD_Reg;		    // 写入要写的寄存器序号
    LCD->LCD_RAM = LCD_RegValue;    // 写入数据
}

// 读寄存器
// LCD_Reg:寄存器地址
// 返回值:读到的数据
u16 LCD_ReadReg(u16 LCD_Reg)
{
    LCD_WR_REG(LCD_Reg);		    // 写入要读的寄存器序号
    delay_us(5);
    return LCD_RD_DATA();		    // 返回读到的值
}

// 开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD_Set_Window(0, lcddev.width - 1, 0, lcddev.height - 1); // 重新设置屏幕窗口大小
}

// LCD写GRAM
// RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{
    LCD->LCD_RAM = RGB_Code;    // 写十六位GRAM
}

/* write_memory_start */
void LCD_WriteREG_Prepare(void)
{
    LCD_WR_REG(0x2c);           // write_memory_start
}

// 当mdk -O1时间优化时需要设置
// 延时i
void opt_delay(u8 i)
{
    while(i--);
}

// 读取个某点的颜色值
// x,y:坐标
// 返回值:此点的颜色
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

// LCD开启显示
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0x11);   // 退出睡眠模式
    LCD_WR_REG(0x29); 	// 开启显示
}

// LCD关闭显示
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0x28);	// 关闭显示
    LCD_WR_REG(0x10);   // 进入睡眠模式
}

// 设置光标位置
// Xpos:横坐标
// Ypos:纵坐标
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

// 画线时调用
// 画点
// x,y:坐标
// POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x, u16 y)
{
    LCD_SetCursor(x, y);		   // 设置光标位置
    LCD_WriteREG_Prepare();	       // 开始写入GRAM
    LCD->LCD_RAM = POINT_COLOR;
}

// 显示字体时调用
// 快速画点
// x,y:坐标
// color:颜色
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

// 设置LCD显示方向
// dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
    if(!dir)			  
    {
        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x00);
        lcddev.dir = 0;	  // 竖屏
        lcddev.width = 320;
        lcddev.height = 480;
    }
    else
    {
        LCD_WR_REG(0x36);
        LCD_WR_DATA(0x60);
        lcddev.dir = 1;	  // 横屏
        lcddev.width = 480;
        lcddev.height = 320;
    }
}

//设置LCD的自动扫描方向
void LCD_Scan_Dir(u8 dir)
{
    
	u16 regval=0;
	u16 dirreg=0;
    
	if(lcddev.dir)//横屏时
	{			   
		switch(dir)//方向转换
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
        case L2R_U2D://从左到右,从上到下
            regval |= (0<<5)|(0<<6)|(0<<7); // 000
            break;
        case L2R_D2U://从左到右,从下到上
            regval |= (0<<5)|(0<<6)|(1<<7); // 001
            break;
        case R2L_U2D://从右到左,从上到下
            regval |= (0<<5)|(1<<6)|(0<<7); // 010
            break;
        case R2L_D2U://从右到左,从下到上
            regval |= (0<<5)|(1<<6)|(1<<7); // 011
            break;	 
        case U2D_L2R://从上到下,从左到右
            regval |= (1<<5)|(0<<6)|(0<<7); // 100
            break;
        case D2U_L2R://从下到上,从左到右
            regval |= (1<<5)|(0<<6)|(1<<7); // 101 
            break;
        case U2D_R2L://从上到下,从右到左
            regval |= (1<<5)|(1<<6)|(0<<7); // 110
            break;
        case D2U_R2L://从下到上,从右到左
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
    函数名：Lcd块选函数
    功能：选定Lcd上指定的矩形区域

    注意：xStart、yStart、Xend、Yend随着屏幕的旋转而改变，位置是矩形框的四个角
    入口参数：   xStart x方向的起始点
                ySrart y方向的起始点
                Xend   y方向的终止点
                Yend   y方向的终止点
    返回值：无
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

// 初始化lcd
// 该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
// 在其他型号的驱动芯片上没有测试!
void LCD_Init(void)
{
    vu32 i = 0;

    GPIO_InitTypeDef  GPIO_InitStructure;
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);            // 使能FSMC时钟

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

    readWriteTiming.FSMC_AddressSetupTime = 0x02;	     // 地址建立时间（ADDSET）为2个HCLK 1/168M=6ns*2=12ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	     // 地址保持时间（ADDHLD）模式A未用到
    readWriteTiming.FSMC_DataSetupTime = 0x05;		     // 数据保存时间为5个HCLK	=6*60=360ns
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A; // 模式A

    writeTiming.FSMC_AddressSetupTime = 7;	             // 地址建立时间（ADDSET）为9个HCLK =54ns
    writeTiming.FSMC_AddressHoldTime = 0x00;	         // 地址保持时间（A
    writeTiming.FSMC_DataSetupTime = 8;		             // 数据保存时间为6ns*9个HCLK=54ns
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 // 模式A

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;                          // 这里我们使用NE1。
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;        // 不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;                   // FSMC_MemoryType_SRAM;  //SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;          // 存储器数据宽度为16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;      // FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	         // 存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;              // 读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;             // 读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;                     // 写时序

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  // 使能BANK1

    delay_ms(20);
    LCD_WR_REG(0xB0);
    LCD_WR_DATA(0x04);
    lcddev.id = Read_ID();

    printf("LCD ID: %x\r\n", lcddev.id); //打印LCD ID

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
        printf("LCD初始化失败！\r\n");
        return;
    }
    LCD_Display_Dir(__DIR__);		// 默认为竖屏
    LCD_Scan_Dir(DFT_SCAN_DIR);
    LCD_Clear(BACK_COLOR);  
}

// 清屏函数
// color:要清屏的填充色
void LCD_Clear(u16 color)
{
    u32 index = 0;
    u32 totalpoint;
    totalpoint  = lcddev.width;
    totalpoint *= lcddev.height; 	        // 得到总点数
    LCD_WriteRAM_Prepare();   		        // 开始写入GRAM
    for(index = 0; index < totalpoint; index++)
    {
        LCD->LCD_RAM = color;
    }
}

// 在指定区域内填充单个颜色
// (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
// color:要填充的颜色
void LCD_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
    u16 i, j;
    u16 xlen = 0;

    xlen = ex - sx + 1;
    for(i = sy; i <= ey; i++)
    {
        LCD_SetCursor(sx, i);      	// 设置光标位置
        LCD_WriteREG_Prepare();  	// 开始写寄存器
        for(j = 0; j < xlen; j++)LCD->LCD_RAM = color;	// 显示颜色
    }
}

// 在指定区域内填充指定颜色块
// (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
// color:要填充的颜色
void LCD_Color_Fill(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
    u16 height, width;
    u16 i, j;
    width = ex - sx + 1; 			      // 得到填充的宽度
    height = ey - sy + 1;			      // 高度
    for(i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i);   	  // 设置光标位置
        LCD_WriteREG_Prepare();
        for(j = 0; j < width; j++)LCD->LCD_RAM = color[i * width + j]; // 写入数据
    }
}

// 画线
// x1,y1:起点坐标
// x2,y2:终点坐标
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;                         // 计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0)incx = 1;                   // 设置单步方向
    else if(delta_x == 0)incx = 0;            //  垂直线
    else {
        incx = -1;
        delta_x = -delta_x;
    }
    if(delta_y > 0)incy = 1;
    else if(delta_y == 0)incy = 0;            // 水平线
    else {
        incy = -1;
        delta_y = -delta_y;
    }
    if( delta_x > delta_y)distance = delta_x; // 选取基本增量坐标轴
    else distance = delta_y;
    for(t = 0; t <= distance + 1; t++ )       // 画线输出
    {
        LCD_DrawPoint(uRow, uCol);            // 画点
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

// 画矩形
// (x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

// 在指定位置画一个指定大小的圆
// (x,y):中心点
// r    :半径
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);          // 判断下个点位置的标志
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
        // 使用Bresenham算法画圆
        if(di < 0)di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

// 在指定位置显示一个字符
// x,y:起始坐标
// num:要显示的字符:" "--->"~"
// size:字体大小 12/16/24
// mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{
    u8 temp, t1, t;
    u16 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//得到字体一个字符对应点阵集所占的字节数

    num = num - ' '; // 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for(t = 0; t < csize; t++)
    {
        if(size == 12)temp = asc2_1206[num][t]; 	 	// 调用1206字体
        else if(size == 16)temp = asc2_1608[num][t];	// 调用1608字体
        else if(size == 24)temp = asc2_2412[num][t];	// 调用2412字体
        else return;								    // 没有的字库
        for(t1 = 0; t1 < 8; t1++)
        {
            if(temp & 0x80)LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if(mode == 0)LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            if(y >= lcddev.height)return;		       // 超区域了
            if((y - y0) == size)
            {
                y = y0;
                x++;
                if(x >= lcddev.width)return;	       // 超区域了
                break;
            }
        }
    }
}

// m^n函数
// 返回值:m^n次方.
u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--)result *= m;
    return result;
}

// 显示数字,高位为0,则不显示
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// color:颜色
// num:数值(0~4294967295);
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

// 显示数字,高位为0,还是显示
// x,y:起点坐标
// num:数值(0~999999999);
// len:长度(即要显示的位数)
// size:字体大小
// mode:
// [7]:0,不填充;1,填充0.
// [6:1]:保留
// [0]:0,非叠加显示;1,叠加显示.
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

// 显示字符串
// x,y:起点坐标
// width,height:区域大小
// size:字体大小
// *p:字符串起始地址
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, u8 *p)
{
    u8 x0 = x;
    width += x;
    height += y;
    while((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
    {
        if(x >= width) {
            x = x0;
            y += size;
        }
        if(y >= height)break;         // 退出
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
