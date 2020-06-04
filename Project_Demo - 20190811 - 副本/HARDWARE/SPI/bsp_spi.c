#include <bsp_spi.h>

/**
 * @name	  spi_init
 * @brief     ��ʼ��SPI��GPIO�Լ�����ģʽΪ���иߵ�ƽ���ڶ�ʱ�ӱ��ش���
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructre;

	RCC_AHB1PeriphClockCmd(SPIx_CLK, ENABLE);  		// ʹ��GPIOʱ��
	RCC_APB1PeriphClockCmd(SPIx_PerClk, ENABLE);    // ʹ������SPIʱ��

	// ��ʼ���ڲ�����SPI
	GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN | SPI_MISO_PIN | SPI_MOSI_PIN;	// PB13~15���ù������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// ���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								// �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;							// 100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// ����
	GPIO_Init(SPIx_PORT, &GPIO_InitStructure);									// ��ʼ��

	GPIO_PinAFConfig(SPIx_PORT, SPI_SCK_AF_PIN, GPIO_AF_SPIx); 
	GPIO_PinAFConfig(SPIx_PORT, SPI_MOSI_AF_PIN, GPIO_AF_SPIx);
	GPIO_PinAFConfig(SPIx_PORT, SPI_MISO_AF_PIN, GPIO_AF_SPIx);

	// ����ֻ���SPI�ڳ�ʼ��
	RCC_APB2PeriphResetCmd(SPIx_PerClk, ENABLE);	 // ��λSPI
	RCC_APB2PeriphResetCmd(SPIx_PerClk, DISABLE);    // ֹͣ��λSPI

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;  // ����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;					// ����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;					// ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;					// ����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;					// ����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;						// NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRate;  					// ���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;					// ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;								// CRCֵ����Ķ���ʽ
	SPI_Init(SPIx, &SPI_InitStructure);											// ����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	SPI_Cmd(SPIx, ENABLE); 														// ʹ��SPI����
	spi_write_byte(0xff);														// ȫ��SPI�����ߣ���������
	
	// ����SPI�����ж�
	SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
	// ���ò�ʹ���ж����ȼ��Լ��ж�Դ
	NVIC_InitStructre.NVIC_IRQChannel 					= SPIx_IRQn;
	NVIC_InitStructre.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructre.NVIC_IRQChannelSubPriority 		= 3;
	NVIC_InitStructre.NVIC_IRQChannelCmd 				= ENABLE;
	NVIC_Init(&NVIC_InitStructre);
}

/**
 * @name	  spi_set_speed
 * @brief     ����SPI2���ٶȣ�SPI2�ٶ�=fAPB1/2^(SpeedSet+1)��fAPB1ʱ��һ��Ϊ42Mhz
 * @param	  speed_set��0~7
 * @return    ��
 * @DateTime  2019-7-17
 */
void spi_set_speed(uint8_t speed_set)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(speed_set));
	SPIx->CR1 &= 0XFFC7;
	SPIx->CR1 |= speed_set;	// ����SPI1�ٶ�
	SPI_Cmd(SPIx, ENABLE);
}

/**
 * @name	  spi_write_byte
 * @brief     дһ���ֽ�
 * @param	  tx_data��Ҫд����ֽ�
 * @return    ��
 * @DateTime  2019-7-17
 */
void spi_write_byte(uint8_t tx_data)
{
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET); 	// �ȴ���������

	SPI_I2S_SendData(SPIx, tx_data);									// ͨ������SPIx����һ��byte����
}

/**
 * @name	  spi_read_byte
 * @brief     ��ȡһ���ֽ�
 * @param	  ��
 * @return    ����һ���ֽ�����
 * @DateTime  2019-7-17
 */
uint8_t spi_read_byte(void)
{
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET); 	// �ȴ�������һ��byte

	return SPI_I2S_ReceiveData(SPIx);
}

/**
 * @name	  SPI2_IRQHandler
 * @brief     SPI�����жϷ�����
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
 */
void SPI2_IRQHandler(void)
{
	if(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) != RESET)
	{
		uint8_t data;
		data = spi_read_byte();
		printf("0x%x\r\n", data);
	}
}
