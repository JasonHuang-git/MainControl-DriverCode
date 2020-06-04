#include <bsp_ch9121.h>

// 配置的时候返回的值
uint8_t return_val = 0;

void init_ch9121(void)
{

}

// 选择ch9121的工作模式
// mode：0->TCP Server
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

// 进入配置模式
// 对于内部读写网口需要先调用此函数进入配置模式
static void ch9121_config_mode(void)
{
	uint8_t i = 10;
	uint8_t data = 0;

	// 发送命令字进入配置模式
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
	printf("---------------------------进入配置模式---------------------------\r\n");
}
static void write_mode()
{

}
// 配置为TCP Server模式
static void ch9121_TCP_Server(void)
{
	// 芯片进入配置模式
	ch9121_config_mode();
	USART_SendData();

}
// 配置为TCP Client模式
static void ch9121_TCP_Client(void)
{
	// 芯片进入配置模式
	ch9121_config_mode();
}

// 配置为UDP Client模式
static void ch9121_UDP(void)
{
	// 芯片进入配置模式
	ch9121_config_mode();
}

// 配置为UDP Server模式
static void ch9121_UDP_Server(void)
{

}

// 接收中断服务函数
// 实际使用时需要在别的文件初始化对于串口的模式
void M_UART_IRQHANDLER(void)
{
	if(USART_GetITStatus( UARTx, USART_IT_RXNE ) != RESET)
	{		
		return_val = USART_ReceiveData(M_UARTX);
	}
}
