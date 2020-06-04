#include <bsp_mb1504.h>

/**
 * @name	  mb1504_init
 * @brief     ��ʼ����Ƶģ��MB1504�Ĺܽ�״̬
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void mb1504_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(MB1504_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = CLK_PIN | DATA_PIN | LE_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(MB1504_GPIO_PORT, &GPIO_InitStructure);
	SendReferF();			// ����Ԥ��Ƶ�ο�Ƶ��
}

/**
 * @name	  Send16Bit
 * @brief     ��׼��Ƶ����д�룬16Bit
 * @param	  SendData��д��ķ�Ƶ�ο���������ֵ
 * @return    ��
 * @DateTime  2019-7-17
 */
static void Send16Bit(uint16_t SendData)
{
	uint8_t i;
	uint16_t SendMiddle;
	SendMiddle = SendData;
	LE_0;
	CLK_0;
	for (i = 0; i < 16; i++)	// ��λ��д��
	{
		if ((SendMiddle & 0x8000) == 0x8000)
		{
			DATA_1;
		}
		else
		{
			DATA_0;
		}
		delay_us(2);
		CLK_1;					// д������
		delay_us(2);
		CLK_0;
		SendMiddle = SendMiddle << 1;
	}
	LE_1;
	delay_us(2);
	LE_0;
}

/**
 * @name	  Send11Bit
 * @brief     ��Ƶ��11λ����д��
 * @param	  SendData��д��ķ�Ƶ�ο���������ֵ
 * @return    ��
 * @DateTime  2019-7-17
 */
static void Send11Bit(uint16_t SendData)
{
	uint8_t i;
	LE_0;
	CLK_0;
	for ( i = 0; i < 11; i ++ )
	{
		if ((SendData & 0x8000) == 0x8000)
		{
			DATA_1;
		}
		else
		{
			DATA_0;
		}
		delay_us (2);
		CLK_1;
		delay_us (2);
		CLK_0;
		SendData = SendData << 1;
	}
}

/**
 * @name	  Send8Bit
 * @brief     ��Ƶ��8λ����д��
 * @param	  SendData��д��ķ�Ƶ�ο���������ֵ
 * @return    ��
 * @DateTime  2019-7-17
 */
static void Send8Bit(uint8_t SendData)   
{
	uint8_t i;
	for ( i = 0; i < 8; i ++ )
	{
		if ((SendData & 0x80) == 0x80)
		{
			DATA_1;
		}
		else
		{
			DATA_0;
		}
		delay_us(2);
		CLK_1;
		delay_us(2);
		CLK_0;
		SendData = SendData << 1;
	}
	LE_1;
	delay_us(2);
	LE_0;
}

/**
 * @name	  SendReferF
 * @brief     ��׼��Ƶ��Ĭ��10kʱ�����
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void SendReferF(void)
{	
	// ��Բ���12M����
	Send16Bit(0x8961); 	// ��׼��Ƶ��Ĭ��10kʱ�����
}

/**
 * @name	  FrequenceToSend
 * @brief     ��Ƶ��8λ����д��
 * @param	  FrequenceD�����÷�Ƶϵ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void FrequenceToSend(uint16_t FrequenceD)
{
	uint8_t AD = 0;
	uint16_t  ND = 0;
	uint16_t MiddleF = 2000;

	MiddleF = FrequenceD;   			 // ��Ƶϵ��PΪ32
	ND = (uint16_t)(MiddleF / 32);   	 // ��Ƶֵ=32*ND+AD;
	AD = (uint8_t)(MiddleF % 32);
	ND = ND << 5;

	AD = AD << 1;       				 // ���λΪ����λ��
	AD = AD & 0xfe;						 // ����λ�õ�;
	Send11Bit(ND);
	Send8Bit(AD);
}
