#include <bsp_uart6.h>

/**
 * @name	  usart6_init
 * @brief     ��ʼ������6��ʹ���շ�
 * @param	  bound��������
 * @return    ��
 * @DateTime  2019-7-20
 */
void usart6_init(uint32_t bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_AHB1PeriphClockCmd(USARTx_GPIO_CLK, ENABLE);  	// ʹ��GPIOCʱ��
	RCC_APB2PeriphClockCmd(USARTx_AF_CLK, ENABLE); 		// ʹ��USART6ʱ��

	// ����6��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(USARTx_TXD_PORT, USARTx_TXD_SOURCE, GPIO_AF_USARTx);  // GPIOC6����ΪUSART6
	GPIO_PinAFConfig(USARTx_RXD_PORT, USARTx_RXD_SOURCE, GPIO_AF_USARTx);  // GPIOC7����ΪUSART6

	// USART6�˿�����
	// PC6 ---> TXD
	// PC7 ---> RXD
	GPIO_InitStructure.GPIO_Pin = USARTx_TXD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    // �ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // ����
	GPIO_Init(USARTx_TXD_PORT, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = USARTx_RXD_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            // ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	    // �ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            // ����
	GPIO_Init(USARTx_RXD_PORT, &GPIO_InitStructure);   
	
	// USART6 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;										// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// ��Ӳ������������	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ
	USART_Init(USARTx, &USART_InitStructure); 										// ��ʼ������6
	
	USART_Cmd(USARTx, ENABLE);  // ʹ�ܴ���x
	
	usart6_nvic_init();		    // ʹ�ܽ����ж�
	
}

/**
 * @name	  uart6_nvic_init
 * @brief     ����6�ж����ȼ�����
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-20
 */
void usart6_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// ʹ�ܴ���x�����ж�
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);				// ��������ж�

	// Usart6 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;			// ����x�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 	// ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			// �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								// ����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

/**
 * @name	  USART6_IRQHandler
 * @brief     ����6�жϷ�����
 * @param	  ��
 * @return    ��
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
