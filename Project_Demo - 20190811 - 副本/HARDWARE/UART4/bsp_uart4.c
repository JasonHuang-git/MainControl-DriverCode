#include <bsp_uart4.h>

extern uint8_t return_val;

/**
 * @name	  uart4_init
 * @brief     初始化串口4，使能收发
 * @param	  bound：波特率
 * @return    空
 * @DateTime  2019-7-20
 */
void uart4_init(uint32_t bound)
{
	// GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(UARTx_GPIO_CLK, ENABLE);  	// 使能GPIOA时钟
	RCC_APB1PeriphClockCmd(UARTx_AF_CLK, ENABLE); 		// 使能USART4时钟

	// 串口4对应引脚复用映射
	GPIO_PinAFConfig(UARTx_TXD_PORT, UARTx_TXD_SOURCE, GPIO_AF_UARTx);  // GPIOA0复用为USART4
	GPIO_PinAFConfig(UARTx_RXD_PORT, UARTx_RXD_SOURCE, GPIO_AF_UARTx);  // GPIOA1复用为USART4

	// USART4端口配置
	// PA0 ---> TXD
	// PA1 ---> RXD
	GPIO_InitStructure.GPIO_Pin = UARTx_TXD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    // 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // 上拉
	GPIO_Init(UARTx_TXD_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = UARTx_RXD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    // 速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // 上拉
	GPIO_Init(UARTx_RXD_PORT, &GPIO_InitStructure);   
	
	// USART4 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 无硬件数据流控制	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式
	USART_Init(UARTx, &USART_InitStructure); 										// 初始化串口4
	
	USART_Cmd(UARTx, ENABLE);  // 使能串口x
	
	uart4_nvic_init();		   // 使能接收中断
	
}

/**
 * @name	  uart4_nvic_init
 * @brief     串口4中断优先级设置
 * @param	  空
 * @return    空
 * @DateTime  2019-7-20
 */
void uart4_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 使能串口x接收中断
	USART_ITConfig(UARTx, USART_IT_RXNE, ENABLE);				// 开启相关中断

	// Usart4 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UARTx_IRQn;			// 串口x中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	// 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			// 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								// 根据指定的参数初始化VIC寄存器、
}


