#include <bsp_pe4302.h>

/**
 * @name	  pe4302_gpio_init
 * @brief     初始化两片PE4302的控制IO口
 * @param	  空
 * @return    空
 * @DateTime  2019-7-18
 */
void pe4302_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(A_PE4302_AHxCLK | B_PE4302_AHxCLK, ENABLE);	 // 使能I/O端口时钟

	// 初始化A组PE4302控制线，以及A、B组芯片片选线
	GPIO_InitStructure.GPIO_Pin = A_PE4302_DATA_PIN | A_PE4302_CLK_PIN | A_PE4302_CS_PIN | B_PE4302_CS_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 	 // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		 // IO口速度为50MHz
	GPIO_Init(A_PE4302_PORT, &GPIO_InitStructure);			 // 根据设定参数初始化GPIOE
	GPIO_SetBits(A_PE4302_PORT, A_PE4302_DATA_PIN | A_PE4302_CLK_PIN | A_PE4302_CS_PIN | B_PE4302_CS_PIN); // PBE 输出高
	
	// 初始化B组PE4302控制线
	GPIO_InitStructure.GPIO_Pin = B_PE4302_DATA_PIN | B_PE4302_CLK_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 	 // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		 // IO口速度为50MHz
	GPIO_Init(A_PE4302_PORT, &GPIO_InitStructure);			 // 根据设定参数初始化GPIO
	GPIO_SetBits(B_PE4302_PORT, B_PE4302_DATA_PIN | B_PE4302_CLK_PIN);
}

/**
 * @name	  pe4302_a_set
 * @brief     A组PE4302增益设置
 * @param	  需要设置的分贝值，0.5db步进，最大31.5dB
 * @return    空
 * @DateTime  2019-7-18
 */
void pe4302_a_set(uint8_t db)
{
	uint8_t i;
	uint8_t w_db;
	db = db&0X3F;
		
	A_PE_LE_DIS;
	delay_us(500);
	A_PE_CLK_L;
	delay_us(500);
	A_PE_LE_EN;
	delay_us(500);
	for(i = 0; i < 6;i++)
	{
		w_db = (db>>5);
		if(w_db == 1)
		{
			A_PE_DAT_H;
		}
		else
		{
			A_PE_DAT_L;
		}
		delay_us(500);
		A_PE_CLK_H;
		delay_us(500);
		db = (db << 1)&0X3F;
		A_PE_CLK_L;
		delay_us(500);
	}
	A_PE_LE_DIS;
	delay_us(500);
	A_PE_DAT_L;
}

/**
 * @name	  pe4302_b_set
 * @brief     B组PE4302增益设置
 * @param	  需要设置的分贝值，0.5db步进，最大31.5dB
 * @return    空
 * @DateTime  2019-7-18
 */
void pe4302_b_set(uint8_t db)
{
	uint8_t i;
	uint8_t w_db;

	db = db * 2;
	db = db & 0X3F;
		
	B_PE_LE_DIS;
	delay_us(500);
	B_PE_CLK_L;
	delay_us(500);
	B_PE_LE_EN;
	delay_us(500);
	for(i = 0; i < 6;i++)
	{
		w_db = (db>>5);
		if(w_db == 1)
		{
			B_PE_DAT_H;
		}
		else
		{
			B_PE_DAT_L;
		}
		delay_us(500);
		B_PE_CLK_H;
		delay_us(500);
		db = (db << 1)&0X3F;
		B_PE_CLK_L;
		delay_us(500);
	}
	B_PE_LE_DIS;
	delay_us(500);
	B_PE_DAT_L;
}
