#include <bsp_ch9121.h>

// ���õ�ʱ�򷵻ص�ֵ
uint8_t return_val = 0;

void init_ch9121(void)
{

}

// ѡ��ch9121�Ĺ���ģʽ
// mode��0->TCP Server
//		 1->TCP Client
//       2->UDP
//       3->UDP Server
static void ch9121_mode_select(uint8_t mode)
{
	switch(mode)
	{
	case 0:
		ch9121_TCP_Server();
		break;
	case 1:
		ch9121_TCP_Client();
		break;
	case 2:
		ch9121_UDP();
		break;
	case 3:
		ch9121_UDP_Server();
		break;
	}
}

// ��������ģʽ
// �����ڲ���д������Ҫ�ȵ��ô˺�����������ģʽ
static void ch9121_config_mode(void)
{
	uint8_t i = 10;
	uint8_t data = 0;

	// ���������ֽ�������ģʽ
	USART_SendData(0x55);
	USART_SendData(0xaa);
	USART_SendData(0x5a);
	while(i--)
	{
		if(return_val = 0xa5;) break;
		delay_ms(5);
	}
	USART_SendData(0xa5);
	while(i--)
	{
		if(return_val = 0xa5;) break;
		delay_ms(5);
	}
	printf("---------------------------��������ģʽ---------------------------\r\n");
}
static void write_mode()
{

}
// ����ΪTCP Serverģʽ
static void ch9121_TCP_Server(void)
{
	// оƬ��������ģʽ
	ch9121_config_mode();
	USART_SendData();

}
// ����ΪTCP Clientģʽ
static void ch9121_TCP_Client(void)
{
	// оƬ��������ģʽ
	ch9121_config_mode();
}

// ����ΪUDP Clientģʽ
static void ch9121_UDP(void)
{
	// оƬ��������ģʽ
	ch9121_config_mode();
}

// ����ΪUDP Serverģʽ
static void ch9121_UDP_Server(void)
{

}

// �����жϷ�����
// ʵ��ʹ��ʱ��Ҫ�ڱ���ļ���ʼ�����ڴ��ڵ�ģʽ
void M_UART_IRQHANDLER(void)
{
	if(USART_GetITStatus( UARTx, USART_IT_RXNE ) != RESET)
	{		
		return_val = USART_ReceiveData(M_UARTX);
	}
}
