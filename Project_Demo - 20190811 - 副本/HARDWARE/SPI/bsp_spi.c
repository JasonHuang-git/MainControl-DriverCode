#include <bsp_spi.h>

/**
 * @name	  spi_init
 * @brief     初始化SPI的GPIO以及工作模式为空闲高电平，第二时钟边沿触发
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
 */
void spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructre;

	RCC_AHB1PeriphClockCmd(SPIx_CLK, ENABLE);  		// 使能GPIO时钟
	RCC_APB1PeriphClockCmd(SPIx_PerClk, ENABLE);    // 使能外设SPI时钟

	// 初始化内部外设SPI
	GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN | SPI_MISO_PIN | SPI_MOSI_PIN;	// PB13~15复用功能输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;								// 复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;								// 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;							// 100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;								// 上拉
	GPIO_Init(SPIx_PORT, &GPIO_InitStructure);									// 初始化

	GPIO_PinAFConfig(SPIx_PORT, SPI_SCK_AF_PIN, GPIO_AF_SPIx); 
	GPIO_PinAFConfig(SPIx_PORT, SPI_MOSI_AF_PIN, GPIO_AF_SPIx);
	GPIO_PinAFConfig(SPIx_PORT, SPI_MISO_AF_PIN, GPIO_AF_SPIx);

	// 这里只针对SPI口初始化
	RCC_APB2PeriphResetCmd(SPIx_PerClk, ENABLE);	 // 复位SPI
	RCC_APB2PeriphResetCmd(SPIx_PerClk, DISABLE);    // 停止复位SPI

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;  // 设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;					// 设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;					// 设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;					// 串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;					// 串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;						// NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRate;  					// 定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;					// 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;								// CRC值计算的多项式
	SPI_Init(SPIx, &SPI_InitStructure);											// 根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

	SPI_Cmd(SPIx, ENABLE); 														// 使能SPI外设
	spi_write_byte(0xff);														// 全部SPI线拉高，启动传输
	
	// 配置SPI接收中断
	SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
	// 配置并使能中断优先级以及中断源
	NVIC_InitStructre.NVIC_IRQChannel 					= SPIx_IRQn;
	NVIC_InitStructre.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructre.NVIC_IRQChannelSubPriority 		= 3;
	NVIC_InitStructre.NVIC_IRQChannelCmd 				= ENABLE;
	NVIC_Init(&NVIC_InitStructre);
}

/**
 * @name	  spi_set_speed
 * @brief     设置SPI2的速度，SPI2速度=fAPB1/2^(SpeedSet+1)，fAPB1时钟一般为42Mhz
 * @param	  speed_set：0~7
 * @return    空
 * @DateTime  2019-7-17
 */
void spi_set_speed(uint8_t speed_set)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(speed_set));
	SPIx->CR1 &= 0XFFC7;
	SPIx->CR1 |= speed_set;	// 设置SPI1速度
	SPI_Cmd(SPIx, ENABLE);
}

/**
 * @name	  spi_write_byte
 * @brief     写一个字节
 * @param	  tx_data：要写入的字节
 * @return    空
 * @DateTime  2019-7-17
 */
void spi_write_byte(uint8_t tx_data)
{
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET); 	// 等待发送区空

	SPI_I2S_SendData(SPIx, tx_data);									// 通过外设SPIx发送一个byte数据
}

/**
 * @name	  spi_read_byte
 * @brief     读取一个字节
 * @param	  空
 * @return    返回一个字节数据
 * @DateTime  2019-7-17
 */
uint8_t spi_read_byte(void)
{
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET); 	// 等待接收完一个byte

	return SPI_I2S_ReceiveData(SPIx);
}

/**
 * @name	  SPI2_IRQHandler
 * @brief     SPI接收中断服务函数
 * @param	  空
 * @return    空
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
