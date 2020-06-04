#include <bsp_esp8266.h>

// 定义接收缓冲区，并初始化为0
STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };

__IO uint8_t ucTcpClosedFlag = 0;

// 私有函数内部声明
static char * itoa( int value, char * string, int radix );

/**
 * @name	  ESP8266_GPIO_Config
 * @brief     初始化ESP8266用到的GPIO引脚
 * @param	  空
 * @return    空
 * @DateTime  2019-7-17
**/
static void ESP8266_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// 初始化GPIO
	RCC_AHB1PeriphClockCmd(ESP8266_RST_GPIO_ClK | ESP8266_USARTx_GPIO_ClK, ENABLE);

	GPIO_PinAFConfig(ESP8266_USARTx_Port, ESP8266_TXD_SOURCE, ESP8266_AF_USARTx);  
	GPIO_PinAFConfig(ESP8266_USARTx_Port, ESP8266_RXD_SOURCE, ESP8266_AF_USARTx);  
	// 串口外设功能GPIO配置
	// PB10 ---> TXD
	// PB11 ---> RXD
	GPIO_InitStructure.GPIO_Pin   = ESP8266_USARTx_Tx_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(ESP8266_USARTx_Port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = ESP8266_USARTx_Rx_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(ESP8266_USARTx_Port, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin   = ESP8266_RST_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(ESP8266_RST_PORT, &GPIO_InitStructure);
	// 设置复位引脚输入低电平，复位ESP8266
	GPIO_WriteBit(ESP8266_RST_PORT, ESP8266_RST_PIN, Bit_RESET);
}

/**
 * @name	  ESP8266_NVIC_Init
 * @brief     对ESP8266使用到的串口中断优先级进行设置
 * @param	  空
 * @return    空
 * @DateTime  2019-7-22
**/
static void ESP8266_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 使能串口x接收中断
	USART_ITConfig(ESP8266_USARTx, USART_IT_RXNE, ENABLE);		// 开启相关中断
	USART_ITConfig(ESP8266_USARTx, USART_IT_IDLE, ENABLE);		// 开启空闲中断
	// Usart3 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = ESP8266_USARTx_IRQn;	// 串口x中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 	// 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			// 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);								// 根据指定的参数初始化VIC寄存器、
}

/**
 * @name	  ESP8266_USART_Config
 * @brief     初始化ESP8266用到的 USART
 * @param	  空
 * @return    空
 * @DateTime  2019-7-22
 */
static void ESP8266_USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	// 串口外设时钟使能
	RCC_APB1PeriphClockCmd(ESP8266_USART_RCC_CLK, ENABLE);

	ESP8266_GPIO_Config();

	// USARTx 初始化设置
	USART_InitStructure.USART_BaudRate = ESP8266_USARTx_BAUDRATE;					// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 无硬件数据流控制	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 收发模式
	USART_Init(ESP8266_USARTx, &USART_InitStructure); 								// 初始化串口6
	
	ESP8266_NVIC_Init();		    	// 使能接收中断，并设置中断优先级
	
	USART_Cmd(ESP8266_USARTx, ENABLE);  // 使能串口x
}

/**
  * @name	   USART_printf
  * @brief     格式化输出，类似于C库中的printf，但这里没有用到C库，
			   因为C库中的printf已经重定向到调试串口1了，自定义了一个printf。
	典型应用：
			   USART3_printf( USART3, "\r\n this is a demo \r\n" );
			   USART3_printf( USART3, "\r\n %d \r\n", i );
			   USART3_printf( USART3, "\r\n %s \r\n", j );

  * @param	   USARTx_：串口通道，这里只用到了串口3，即USART3
 		       Data：要发送到串口的内容的指针
  			   ...：其他参数
  * @return    空
  * @DateTime  2019-7-22
**/
void USART_printf(USART_TypeDef * USARTx_, char * Data, ... )
{
	const char *s;
	int d;
	char buf[16];
	uint8_t txdata;

	// 需要包含stdarg.h
	va_list ap;
	va_start(ap, Data);
	while(*Data != 0 )     	 // 判断是否到达字符串结束符
	{
		if(*Data == 0x5c)  	 // 判断是否收到了转移字符'\'
		{
			switch ( *++Data )
			{
			case 'r':							          // 发送回车符
				txdata = 0x0d;
				USART_SendData(USARTx_, txdata);
				Data ++;
				break;
			case 'n':							          // 发送换行符
				txdata = 0x0a;
				USART_SendData(USARTx_, txdata);
				Data ++;
				break;
			default:
				Data ++;
				break;
			}
		}
		else if(*Data == '%')
		{
			switch ( *++Data )
			{
			case 's':									  // 字符串
				s = va_arg(ap, const char *);
				for ( ; *s; s++)
				{
					USART_SendData(USARTx_, *s);
					while (USART_GetFlagStatus(USARTx_, USART_FLAG_TXE) == RESET);
				}
				Data++;
				break;
			case 'd':
				//十进制
				d = va_arg(ap, int);
				itoa(d, buf, 10);
				for (s = buf; *s; s++)
				{
					USART_SendData(USARTx_, *s);
					while (USART_GetFlagStatus(USARTx_, USART_FLAG_TXE) == RESET);
				}
				Data++;
				break;
			default:
				Data++;
				break;
			}
		}
		else
		{
			USART_SendData(USARTx_, *Data);
			Data++;
			while (USART_GetFlagStatus(USARTx_, USART_FLAG_TXE) == RESET);
		}
	}
}

/**
  * @name	   itoa
  * @brief     将整形数据转换成字符串
  * @param	   value：要转换的整形数
			   string：转换后的字符串指针
			   radix：传入参数的数据类型，暂时只支持10进制，radix =10 表示10进制，其他结果为0
  * @return    返回转换后字符串的首地址
  * @DateTime  2019-7-22
**/
static char * itoa( int value, char *string, int radix)
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;
	/* This implementation only works for decimal numbers. */
	if(radix != 10)
	{
		*ptr = 0;
		return string;
	}
	if(!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}
	/* if this is a negative value insert the minus sign. */
	if(value < 0)
	{
		*ptr++ = '-';
		/* Make the value positive. */
		value *= -1;
	}
	for(i = 10000; i > 0; i /= 10)
	{
		d = value / i;
		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}
	/* Null terminate the string. */
	*ptr = 0;
	return string;
}

/**
  * @name	   ESP8266_Init
  * @brief     ESP8266初始化函数
  * @param	   空
  * @return    空
  * @DateTime  2019-7-22
**/
void ESP8266_Init(void)
{
	ESP8266_GPIO_Config();
	ESP8266_USART_Config();
}

/**
  * @name	   ESP8266_stop
  * @brief     停止使用ESP8266
  * @param	   空
  * @return    空
  * @DateTime  2019-7-22
**/
void  ESP8266_stop( void )
{
	ESP8266_RST_LOW();

	USART_Cmd(ESP8266_USARTx, DISABLE);
	USART_DeInit(ESP8266_USARTx);
}

/**
  * @name	   ESP8266_Rst
  * @brief     重启ESP8266模块
  * @param	   空
  * @return    空
  * @DateTime  2019-7-22
**/
void ESP8266_Rst ( void )
{
#if 0
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );
#else
	ESP8266_RST_LOW();
	delay_ms(500);
	ESP8266_RST_HIGH();
#endif
}

/**
  * @name	  ESP8266_Cmd
  * @brief    对ESP8266模块发送AT指令
  * @param	  cmd：待发送的指令
			  reply1：reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
			  waittime：等待响应的时间
  * @return   1：指令发送成功
			  0：指令发送失败
  * @DateTime 2019-7-22
**/
bool ESP8266_Cmd(char * cmd, char * reply1, char * reply2, uint32_t waittime)
{
	// 从新开始接收新的数据包
	strEsp8266_Fram_Record.InfBit .FramLength = 0;
	// 向ESP8266发送指令
	ESP8266_Usart ( "%s\r\n", cmd );
	// 不需要接收回复，则直接返回
	if ((reply1 == 0) && (reply2 == 0))
	{
		return true;
	}
	// 延时等待
	delay_ms(waittime); 
	// 判断接收是否完成，即通过接收缓冲区最后一个值是否是字符串的结尾'\0'实现
	strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength]  = '\0';
	// 把接收到的字符串输出到电脑端串口调试助手上查看，PC_Usart为printf的宏定义，
	// 而printf重定向指向了串口1。
	PC_Usart("%s", strEsp8266_Fram_Record.Data_RX_BUF);
	
	// 通过C标准库函数strstr 查找是否接收到了需要回复的字符串，是则返回1，反之返回0
	if((reply1 != 0) && (reply2 != 0))
	{
		return ((bool)strstr(strEsp8266_Fram_Record.Data_RX_BUF, reply1) ||
		         (bool)strstr(strEsp8266_Fram_Record.Data_RX_BUF, reply2));
	}
	else if(reply1 != 0)
	{
		return ((bool)strstr(strEsp8266_Fram_Record.Data_RX_BUF, reply1));
	}
	else
	{
		return ((bool)strstr(strEsp8266_Fram_Record.Data_RX_BUF, reply2));
	}

}

/**
  * @name	  ESP8266_AT_Test
  * @brief    对ESP8266模块进行AT测试启动
  * @param	  空
  * @return   1：选择成功
			  0：选择失败
  * @DateTime 2019-7-22
**/
bool ESP8266_AT_Test(void)
{
	char count = 0;
	// 拉高复位引脚，退出复位
	ESP8266_RST_HIGH();
	delay_ms(1000);
	while (count < 10)
	{
		// 发送AT测试是否收到ESP8266的响应，如果没收到回复，则重测机制为尝试十次
		if (ESP8266_Cmd("AT", "OK", NULL, 1000))
		{
			return 1;
		}
		ESP8266_Rst();
		delay_ms(1000);
		++count;
	}
	return 0;
}

/**
  * @name	  ESP8266_Net_Mode_Choose
  * @brief    选择ESP8266模块的工作模式 
  * @param	  类型为对应头文件中定义的枚举类型，有如下三种：
			  1-   STA
			  2-   AP
			  3-   STA_AP
  * @return   1：选择成功
			  0：选择失败
  * @DateTime 2019-7-22
**/
bool ESP8266_Net_Mode_Choose (ENUM_Net_ModeTypeDef enumMode)
{
	bool result = 0;
	char count = 0;
	// 请求次数为10次
	while (count < 10)
	{
		switch(enumMode)
		{
		case STA:
			result = ESP8266_Cmd("AT+CWMODE=1", "OK", "no change", 2500);
			break;
		case AP:
			result = ESP8266_Cmd("AT+CWMODE=2", "OK", "no change", 2500);
			break;
		case STA_AP:
			result = ESP8266_Cmd("AT+CWMODE=3", "OK", "no change", 2500);
			break;
		default:
			result = false;
			break;
		}
		if(result) return result;
		++count;
	}
	return 0;
}

/**
  * @name	  ESP8266_JoinAP
  * @brief    ESP8266模块连接外部WiFi 
  * @param	  pSSID：    WiFi名称字符串
			  pPassWord：WiFi密码字符串
  * @return   1：连接成功
			  0：连接失败
  * @DateTime 2019-7-22
**/
bool ESP8266_JoinAP(char * pSSID, char * pPassWord)
{
	char cCmd [120];
	char count = 0;
	// 把命令+SSID+PASSWORD三者拼接成要发送的命令字符串
	sprintf(cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord);
	// 请求10次
	while (count < 10)
	{
		// 判断是否配置成功
		if (ESP8266_Cmd(cCmd, "OK", NULL, 5000))
		{
			return 1;
		}
		++count;
	}
	return 0;
}

/**
  * @name	  ESP8266_BuildAP
  * @brief    ESP8266模块创建WiFi热点 
  * @param	  pSSID：    WiFi名称字符串
			  pPassWord：WiFi密码字符串
  * @return   1，配置成功
			  0，配置失败
  * @DateTime 2019-7-22
**/
bool ESP8266_BuildAP(char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode)
{
	char cCmd [120];
	char count = 0;
	// 把命令+SSID+PASSWORD三者拼接成要发送的命令字符串
	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%d", pSSID, pPassWord, enunPsdMode );
	// 请求10次
	while (count < 10)
	{
		// 判断AP是否配置成功
		if (ESP8266_Cmd(cCmd, "OK", 0, 1000))
		{
			return 1;
		}
		++count;
	}
	return 0;
}

/**
  * @name	  ESP8266_Enable_MultipleId
  * @brief    ESP8266模块启动多连接 
  * @param	  enumEnUnvarnishTx：配置是否多连接
		      取值：ENABLE or DISABLE  
  * @return   1：配置成功
			  0：配置失败
  * @DateTime 2019-7-22
**/
bool ESP8266_Enable_MultipleId(FunctionalState enumEnUnvarnishTx)
{
	char cStr [20];
	char count = 0;
	sprintf(cStr, "AT+CIPMUX=%d", (enumEnUnvarnishTx ? 1 : 0));
	while (count < 10)
	{
		if (ESP8266_Cmd(cStr, "OK", 0, 500))return 1;
		++count;
	}
	return 0;
}

/**
  * @name	  ESP8266_Link_Server
  * @brief    ESP8266模块连接外部服务器 
  * @param	  enumE： 网络协议，TCP/UDP
		      ip：	  服务器IP字符串
              Port：服务器端口字符串
		      id：    模块连接服务器的ID
  * @return   1，连接成功
			  0，连接失败
  * @DateTime 2019-7-22
**/
bool ESP8266_Link_Server(ENUM_NetPro_TypeDef enumE, char * ip, char * Port, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = {0}, cCmd [120];

	switch(enumE)
	{
	case enumTCP:
		sprintf(cStr, "\"%s\",\"%s\",%s", "TCP", ip, Port);	// 拼接要发送的TCP命令字
		break;
	case enumUDP:
		sprintf(cStr, "\"%s\",\"%s\",%s", "UDP", ip, Port);	// 拼接要发送的UDP命令字
		break;
	default:
		break;
	}
	if(id < 5)
	{
		sprintf(cCmd, "AT+CIPSTART=%d,%s", id, cStr);
	}
	else
	{
		sprintf(cCmd, "AT+CIPSTART=%s", cStr);
	}
	// 发送指令
	return ESP8266_Cmd(cCmd, "OK", "ALREAY CONNECT", 4000);
}

/**
  * @name	   ESP8266_StartOrShutServer
  * @brief     ESP8266模块开启或关闭服务器模式 
  * @param	   enumMode： 开启（ENABLE）/关闭（DISABLE）
               pPortNum： 服务器端口号字符串
		       pTimeOver：服务器超时时间字符串，单位：秒
  * @return    1：操作成功
			   0：操作失败
  * @DateTime  2019-7-22
**/
bool ESP8266_StartOrShutServer(FunctionalState enumMode, char * pPortNum, char * pTimeOver)
{
	char cCmd1 [120], cCmd2 [120];

	if(enumMode)
	{
		// 设置服务器端口号
		sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum);
		// 设置服务器超时时间
		sprintf(cCmd2, "AT+CIPSTO=%s", pTimeOver);			

		return (ESP8266_Cmd (cCmd1, "OK", 0, 500) && ESP8266_Cmd (cCmd2, "OK", 0, 500));
	}
	else
	{
		 // 关闭服务器
		sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum);

		return ESP8266_Cmd(cCmd1, "OK", 0, 500);
	}
}

/**
  * @name	   ESP8266_Get_LinkStatus
  * @brief     获取ESP8266 的连接状态，较适合单端口时使用 
  * @param	   空
  * @return    2：获得ip
               3：建立连接
		       4：失去连接
               0：获取状态失败
  * @DateTime  2019-7-22
**/
uint8_t ESP8266_Get_LinkStatus(void)
{
	if(ESP8266_Cmd("AT+CIPSTATUS", "OK", 0, 500))
	{
		if(strstr(strEsp8266_Fram_Record.Data_RX_BUF, "STATUS:2\r\n"))
		{
			return 2;
		}
		else if(strstr(strEsp8266_Fram_Record.Data_RX_BUF, "STATUS:3\r\n"))
		{
			return 3;
		}
		else if(strstr(strEsp8266_Fram_Record.Data_RX_BUF, "STATUS:4\r\n"))
		{
			return 4;
		}
	}
	return 0;
}

/**
  * @name	   ESP8266_Get_LinkStatus
  * @brief     获取ESP8266 的端口（Id）连接状态，较适合多端口时使用
  * @param	   空
  * @return    端口（Id）的连接状态，低5位为有效位，分别对应Id5~0，某位若置1表该Id建立了连接，
			   若被清0表该Id未建立连接。
  * @DateTime  2019-7-22
**/
uint8_t ESP8266_Get_IdLinkStatus(void)
{
	uint8_t ucIdLinkStatus = 0x00;

	if(ESP8266_Cmd("AT+CIPSTATUS", "OK", 0, 500))
	{
		if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:0,"))
			ucIdLinkStatus |= 0x01;
		else
			ucIdLinkStatus &= ~ 0x01;

		if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:1,"))
			ucIdLinkStatus |= 0x02;
		else
			ucIdLinkStatus &= ~ 0x02;

		if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:2,"))
			ucIdLinkStatus |= 0x04;
		else
			ucIdLinkStatus &= ~ 0x04;

		if ( strstr(strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:3,"))
			ucIdLinkStatus |= 0x08;
		else
			ucIdLinkStatus &= ~ 0x08;

		if ( strstr(strEsp8266_Fram_Record .Data_RX_BUF, "+CIPSTATUS:4,"))
			ucIdLinkStatus |= 0x10;
		else
			ucIdLinkStatus &= ~ 0x10;
	}
	return ucIdLinkStatus;
}

/**
  * @name	   ESP8266_CIPAP
  * @brief     设置模块的 AP IP
  * @param	   pApIp：模块的 AP IP
  * @return    1：设置成功
			   0：设置失败
  * @DateTime  2019-7-22
**/
uint8_t ESP8266_CIPAP(char * pApIp)
{
	char cCmd [ 30 ];
		
	sprintf(cCmd, "AT+CIPAP=\"%s\"", pApIp);
	
	if(ESP8266_Cmd(cCmd, "OK", 0, 5000))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
  * @name	   ESP8266_Inquire_ApIp
  * @brief     获取ESP8266 的 AP IP
  * @param	   pApIp：		  存放 AP IP 的数组的首地址
  			   ucArrayLength：存放 AP IP 的数组的长度
  * @return    1：获取成功
  			0：获取失败
  * @DateTime  2019-7-22
**/
uint8_t ESP8266_Inquire_ApIp(char * pApIp, uint8_t ucArrayLength)
{
	char uc;
	char * pCh;

	ESP8266_Cmd("AT+CIFSR", "OK", 0, 500);
	pCh = strstr(strEsp8266_Fram_Record .Data_RX_BUF, "APIP,\"");
	if(pCh)
	{
		pCh += 6;
	}
	else
	{
		return 0;
	}
	for(uc = 0; uc < ucArrayLength; uc++)
	{
		pApIp[uc] = *(pCh + uc);
		
		if(pApIp[uc] == '\"')
		{
			pApIp[uc] = '\0';
			break;
		}
	}
	return 1;
}

/**
  * @name	  ESP8266_UnvarnishSend
  * @brief    配置ESP8266模块进入透传发送
  * @param	  空
  * @return   1：为单连接
              0：非单连接
  * @DateTime 2019-7-22
**/
bool ESP8266_UnvarnishSend(void)
{
	// 判断是否为单连接模式
	if (!ESP8266_Cmd("AT+CIPMODE=1", "OK", 0, 1000))
	{
		return false;
	}
	return  ESP8266_Cmd("AT+CIPSEND", "OK", ">", 1000);
}

/**
  * @name	   ESP8266_ExitUnvarnishSend
  * @brief     配置ESP8266模块退出透传模式
  * @param	   空
  * @return    空
  * @DateTime  2019-7-22
**/
void ESP8266_ExitUnvarnishSend(void)
{
	delay_ms(1000);
	ESP8266_Usart("+++");
	delay_ms(500);
}

/**
 * @name	  ESP8266_SendString
 * @brief     ESP8266模块发送字符串
 * @param	  enumEnUnvarnishTx：声明是否已使能了透传模式
              pStr：			 要发送的字符串
              ucId：			 哪个ID发送的字符串
              ulStrLength： 	 要发送的字符串的字节数
 * @return    1：发送成功
              0：发送失败
 * @DateTime  2019-7-22
**/
bool ESP8266_SendString(FunctionalState enumEnUnvarnishTx, char * pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId)
{
	char cStr [20];
	bool bRet = false;

	if(enumEnUnvarnishTx)	// 透传模式
	{
		ESP8266_Usart("%s", pStr);
		bRet = true;
	}
	else					
	{
		if(ucId < 5)		// 多连接模式
		{
			sprintf(cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2);
		}
		else			    // 单连接模式
		{
			sprintf(cStr, "AT+CIPSEND=%d", ulStrLength + 2);
		}

		ESP8266_Cmd(cStr, "> ", 0, 1000);
		bRet = ESP8266_Cmd(pStr, "SEND OK", 0, 1000);
	}
	return bRet;
}

/**
 * @name	  ESP8266_ReceiveString
 * @brief     ESP8266模块接收字符串
 * @param	  enumEnUnvarnishTx：是否已使能了透传模式
 * @return    接收到的字符串首地址
 * @DateTime  2019-7-22
**/
char * ESP8266_ReceiveString(FunctionalState enumEnUnvarnishTx)
{
	char * pRecStr = 0;
	// 清楚帧长度和帧接收完成标志位
	strEsp8266_Fram_Record.InfBit.FramLength = 0;
	strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;
	// 等待帧标志位被置位
	while(!strEsp8266_Fram_Record.InfBit.FramFinishFlag);
	// 给收到的字符串添加结束符'\0'
	strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength] = '\0';

	if(enumEnUnvarnishTx)
	{
		pRecStr = strEsp8266_Fram_Record.Data_RX_BUF;
	}
	else
	{
		if(strstr(strEsp8266_Fram_Record.Data_RX_BUF, "+IPD"))
		{
			pRecStr = strEsp8266_Fram_Record.Data_RX_BUF;
		}
	}
	return pRecStr;
}

/**
 * @name	  ESP8266_Clear_Buffer
 * @brief     ESP8266 清除接收缓冲区
 * @param	  空
 * @return    空
 * @DateTime  2019-7-22
**/
void ESP8266_Clear_Buffer(void)
{
  uint16_t i;
  strEsp8266_Fram_Record.InfBit.FramLength = 0;
  strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;
  for(i = 0; i < RX_BUF_MAX_LEN; ++i)
  {
    strEsp8266_Fram_Record.Data_RX_BUF[i]  = 0;
  }
}

/**
 * @name	  ESP8266_USARTx_IRQHANDLER
 * @brief     ESP8266 串口中断服务函数
 * @param	  空
 * @return    空
 * @DateTime  2019-7-22
**/
void ESP8266_USARTx_IRQHANDLER(void)
{
	uint8_t esp8266_rxdata;
	
	// 判断是否是接收中断
	if(USART_GetITStatus(ESP8266_USARTx, USART_IT_RXNE) != RESET)		    // 接收一个字节数据
	{
		// 接收一个字节
		esp8266_rxdata = USART_ReceiveData(ESP8266_USARTx);
		// 判断接收缓冲区是否溢出，否则把收到的字节存入接收缓冲区
		if(strEsp8266_Fram_Record.InfBit.FramLength < (RX_BUF_MAX_LEN-1))   // 预留1个字节写结束符  
		{		
			strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength++] = esp8266_rxdata;
		}
	}
	// 帧中断
	if(USART_GetITStatus(ESP8266_USARTx, USART_IT_IDLE) == SET)      		// 判断一帧数据数据接收完成
	{
		strEsp8266_Fram_Record.InfBit.FramFinishFlag = 1;
		esp8266_rxdata = USART_ReceiveData(ESP8266_USARTx);
		ucTcpClosedFlag = strstr(strEsp8266_Fram_Record.Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
	}
}
