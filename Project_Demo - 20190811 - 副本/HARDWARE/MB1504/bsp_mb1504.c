#include <bsp_mb1504.h>

/**
 * @name	  mb1504_init
 * @brief     初始化分频模块MB1504的管脚状态
 * @param	  空
 * @return    空
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
	SendReferF();			// 设置预分频参考频率
}

/**
 * @name	  Send16Bit
 * @brief     基准分频数据写入，16Bit
 * @param	  SendData：写入的分频参考计数器的值
 * @return    空
 * @DateTime  2019-7-17
 */
static void Send16Bit(uint16_t SendData)
{
	uint8_t i;
	uint16_t SendMiddle;
	SendMiddle = SendData;
	LE_0;
	CLK_0;
	for (i = 0; i < 16; i++)	// 高位先写入
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
		CLK_1;					// 写入数据
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
 * @brief     分频高11位数据写入
 * @param	  SendData：写入的分频参考计数器的值
 * @return    空
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
 * @brief     分频低8位数据写入
 * @param	  SendData：写入的分频参考计数器的值
 * @return    空
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
 * @brief     基准分频，默认10k时钟输出
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
 */
void SendReferF(void)
{	
	// 针对采用12M晶振
	Send16Bit(0x8961); 	// 基准分频，默认10k时钟输出
}

/**
 * @name	  FrequenceToSend
 * @brief     分频低8位数据写入
 * @param	  FrequenceD：设置分频系数
 * @return    空
 * @DateTime  2019-7-17
 */
void FrequenceToSend(uint16_t FrequenceD)
{
	uint8_t AD = 0;
	uint16_t  ND = 0;
	uint16_t MiddleF = 2000;

	MiddleF = FrequenceD;   			 // 分频系数P为32
	ND = (uint16_t)(MiddleF / 32);   	 // 分频值=32*ND+AD;
	AD = (uint8_t)(MiddleF % 32);
	ND = ND << 5;

	AD = AD << 1;       				 // 最低位为控制位，
	AD = AD & 0xfe;						 // 控制位置低;
	Send11Bit(ND);
	Send8Bit(AD);
}
