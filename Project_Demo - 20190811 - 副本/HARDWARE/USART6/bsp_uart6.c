#include <bsp_uart6.h>

/**
 * @name	  usart6_init
 * @brief     初始化串口6，使能收发
 * @param	  bound：波特率
 * @return    空
 * @DateTime  2019-7-20
 */
void usart6_init(uint32_t bound)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(USARTx_GPIO_CLK, ENABLE);  	// 使能GPIOC时钟
	RCC_APB2PeriphClockCmd(USARTx_AF_CLK, ENABLE); 		// 使能USART6时钟

	// 串口6对应引脚复用映射
	GPIO_PinAFConfig(USARTx_TXD_PORT, USARTx_TXD_SOURCE, GPIO_AF_USARTx);  // GPIOC6复用为USART6
	GPIO_PinAFConfig(USARTx_RXD_PORT, USARTx_RXD_SOURCE, GPIO_AF_USARTx);  // GPIOC7复用为USART6

	// USART6端口配置
	// PC6 ---> TXD
	// PC7 ---> RXD
	GPIO_InitStructure.GPIO_Pin = USARTx_TXD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    // 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // 上拉
	GPIO_Init(USARTx_TXD_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = USARTx_RXD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    // 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // 上拉
	GPIO_Init(USARTx_RXD_PORT, &GPIO_InitStructure);   
	
	// USART6 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 无硬件数据流控制	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式
	USART_Init(USARTx, &USART_InitStructure); 										// 初始化串口6
	
	USART_Cmd(USARTx, ENABLE);  // 使能串口x
	
	usart6_nvic_init();		    // 使能接收中断
	
}

/**
 * @name	  uart6_nvic_init
 * @brief     串口6中断优先级设置
 * @param	  空
 * @return    空
 * @DateTime  2019-7-20
 */
void usart6_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 使能串口x接收中断
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);				// 开启相关中断

	// Usart6 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;			// 串口x中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 	// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								// 根据指定的参数初始化VIC寄存器、
}

/**
 * @name	  USART6_IRQHandler
 * @brief     串口6中断服务函数
 * @param	  空
 * @return    空
 * @DateTime  2019-7-20
 */
void USART6_IRQHandler(void)
{
	uint8_t rel;
	if(USART_GetITStatus( USARTx, USART_IT_RXNE ) != RESET)
	{		
		rel = USART_ReceiveData(USARTx);
		USART_SendData(USARTx, rel);
	} 
}
