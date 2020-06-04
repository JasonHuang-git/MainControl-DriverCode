#include <bsp_uart4.h>

extern uint8_t return_val;

/**
 * @name	  uart4_init
 * @brief     ��ʼ������4��ʹ���շ�
 * @param	  bound��������
 * @return    ��
 * @DateTime  2019-7-20
 */
void uart4_init(uint32_t bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(UARTx_GPIO_CLK, ENABLE);  	// ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(UARTx_AF_CLK, ENABLE); 		// ʹ��USART4ʱ��

	// ����4��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(UARTx_TXD_PORT, UARTx_TXD_SOURCE, GPIO_AF_UARTx);  // GPIOA0����ΪUSART4
	GPIO_PinAFConfig(UARTx_RXD_PORT, UARTx_RXD_SOURCE, GPIO_AF_UARTx);  // GPIOA1����ΪUSART4

	// USART4�˿�����
	// PA0 ---> TXD
	// PA1 ---> RXD
	GPIO_InitStructure.GPIO_Pin = UARTx_TXD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    // �ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // ����
	GPIO_Init(UARTx_TXD_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = UARTx_RXD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    // �ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // ����
	GPIO_Init(UARTx_RXD_PORT, &GPIO_InitStructure);   
	
	// USART4 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;										// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// ��Ӳ������������	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ
	USART_Init(UARTx, &USART_InitStructure); 										// ��ʼ������4
	
	USART_Cmd(UARTx, ENABLE);  // ʹ�ܴ���x
	
	uart4_nvic_init();		   // ʹ�ܽ����ж�
	
}

/**
 * @name	  uart4_nvic_init
 * @brief     ����4�ж����ȼ�����
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-20
 */
void uart4_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// ʹ�ܴ���x�����ж�
	USART_ITConfig(UARTx, USART_IT_RXNE, ENABLE);				// ��������ж�

	// Usart4 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UARTx_IRQn;			// ����x�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	// ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			// �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								// ����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}


