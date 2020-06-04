#include <bsp_pe4302.h>

/**
 * @name	  pe4302_gpio_init
 * @brief     ��ʼ����ƬPE4302�Ŀ���IO��
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-18
 */
void pe4302_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(A_PE4302_AHxCLK | B_PE4302_AHxCLK, ENABLE);	 // ʹ��I/O�˿�ʱ��

	// ��ʼ��A��PE4302�����ߣ��Լ�A��B��оƬƬѡ��
	GPIO_InitStructure.GPIO_Pin = A_PE4302_DATA_PIN | A_PE4302_CLK_PIN | A_PE4302_CS_PIN | B_PE4302_CS_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 	 // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		 // IO���ٶ�Ϊ50MHz
	GPIO_Init(A_PE4302_PORT, &GPIO_InitStructure);			 // �����趨������ʼ��GPIOE
	GPIO_SetBits(A_PE4302_PORT, A_PE4302_DATA_PIN | A_PE4302_CLK_PIN | A_PE4302_CS_PIN | B_PE4302_CS_PIN); // PBE �����
	
	// ��ʼ��B��PE4302������
	GPIO_InitStructure.GPIO_Pin = B_PE4302_DATA_PIN | B_PE4302_CLK_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 	 // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		 // IO���ٶ�Ϊ50MHz
	GPIO_Init(A_PE4302_PORT, &GPIO_InitStructure);			 // �����趨������ʼ��GPIO
	GPIO_SetBits(B_PE4302_PORT, B_PE4302_DATA_PIN | B_PE4302_CLK_PIN);
}

/**
 * @name	  pe4302_a_set
 * @brief     A��PE4302��������
 * @param	  ��Ҫ���õķֱ�ֵ��0.5db���������31.5dB
 * @return    ��
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
 * @brief     B��PE4302��������
 * @param	  ��Ҫ���õķֱ�ֵ��0.5db���������31.5dB
 * @return    ��
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
