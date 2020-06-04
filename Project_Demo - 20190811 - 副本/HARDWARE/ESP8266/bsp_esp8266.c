#include <bsp_esp8266.h>

// ������ջ�����������ʼ��Ϊ0
STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };

__IO uint8_t ucTcpClosedFlag = 0;

// ˽�к����ڲ�����
static char * itoa( int value, char * string, int radix );

/**
 * @name	  ESP8266_GPIO_Config
 * @brief     ��ʼ��ESP8266�õ���GPIO����
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-17
**/
static void ESP8266_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// ��ʼ��GPIO
	RCC_AHB1PeriphClockCmd(ESP8266_RST_GPIO_ClK | ESP8266_USARTx_GPIO_ClK, ENABLE);

	GPIO_PinAFConfig(ESP8266_USARTx_Port, ESP8266_TXD_SOURCE, ESP8266_AF_USARTx);  
	GPIO_PinAFConfig(ESP8266_USARTx_Port, ESP8266_RXD_SOURCE, ESP8266_AF_USARTx);  
	// �������蹦��GPIO����
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
	// ���ø�λ��������͵�ƽ����λESP8266
	GPIO_WriteBit(ESP8266_RST_PORT, ESP8266_RST_PIN, Bit_RESET);
}

/**
 * @name	  ESP8266_NVIC_Init
 * @brief     ��ESP8266ʹ�õ��Ĵ����ж����ȼ���������
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-22
**/
static void ESP8266_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// ʹ�ܴ���x�����ж�
	USART_ITConfig(ESP8266_USARTx, USART_IT_RXNE, ENABLE);		// ��������ж�
	USART_ITConfig(ESP8266_USARTx, USART_IT_IDLE, ENABLE);		// ���������ж�
	// Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = ESP8266_USARTx_IRQn;	// ����x�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 	// ��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			// �����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);								// ����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

/**
 * @name	  ESP8266_USART_Config
 * @brief     ��ʼ��ESP8266�õ��� USART
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-22
 */
static void ESP8266_USART_Config(void)
{
	USART_InitTypeDef USART_InitStructure;
	// ��������ʱ��ʹ��
	RCC_APB1PeriphClockCmd(ESP8266_USART_RCC_CLK, ENABLE);

	ESP8266_GPIO_Config();

	// USARTx ��ʼ������
	USART_InitStructure.USART_BaudRate = ESP8266_USARTx_BAUDRATE;					// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// ��Ӳ������������	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ
	USART_Init(ESP8266_USARTx, &USART_InitStructure); 								// ��ʼ������6
	
	ESP8266_NVIC_Init();		    	// ʹ�ܽ����жϣ��������ж����ȼ�
	
	USART_Cmd(ESP8266_USARTx, ENABLE);  // ʹ�ܴ���x
}

/**
  * @name	   USART_printf
  * @brief     ��ʽ�������������C���е�printf��������û���õ�C�⣬
			   ��ΪC���е�printf�Ѿ��ض��򵽵��Դ���1�ˣ��Զ�����һ��printf��
	����Ӧ�ã�
			   USART3_printf( USART3, "\r\n this is a demo \r\n" );
			   USART3_printf( USART3, "\r\n %d \r\n", i );
			   USART3_printf( USART3, "\r\n %s \r\n", j );

  * @param	   USARTx_������ͨ��������ֻ�õ��˴���3����USART3
 		       Data��Ҫ���͵����ڵ����ݵ�ָ��
  			   ...����������
  * @return    ��
  * @DateTime  2019-7-22
**/
void USART_printf(USART_TypeDef * USARTx_, char * Data, ... )
{
	const char *s;
	int d;
	char buf[16];
	uint8_t txdata;

	// ��Ҫ����stdarg.h
	va_list ap;
	va_start(ap, Data);
	while(*Data != 0 )     	 // �ж��Ƿ񵽴��ַ���������
	{
		if(*Data == 0x5c)  	 // �ж��Ƿ��յ���ת���ַ�'\'
		{
			switch ( *++Data )
			{
			case 'r':							          // ���ͻس���
				txdata = 0x0d;
				USART_SendData(USARTx_, txdata);
				Data ++;
				break;
			case 'n':							          // ���ͻ��з�
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
			case 's':									  // �ַ���
				s = va_arg(ap, const char *);
				for ( ; *s; s++)
				{
					USART_SendData(USARTx_, *s);
					while (USART_GetFlagStatus(USARTx_, USART_FLAG_TXE) == RESET);
				}
				Data++;
				break;
			case 'd':
				//ʮ����
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
  * @brief     ����������ת�����ַ���
  * @param	   value��Ҫת����������
			   string��ת������ַ���ָ��
			   radix������������������ͣ���ʱֻ֧��10���ƣ�radix =10 ��ʾ10���ƣ��������Ϊ0
  * @return    ����ת�����ַ������׵�ַ
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
  * @brief     ESP8266��ʼ������
  * @param	   ��
  * @return    ��
  * @DateTime  2019-7-22
**/
void ESP8266_Init(void)
{
	ESP8266_GPIO_Config();
	ESP8266_USART_Config();
}

/**
  * @name	   ESP8266_stop
  * @brief     ֹͣʹ��ESP8266
  * @param	   ��
  * @return    ��
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
  * @brief     ����ESP8266ģ��
  * @param	   ��
  * @return    ��
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
  * @brief    ��ESP8266ģ�鷢��ATָ��
  * @param	  cmd�������͵�ָ��
			  reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
			  waittime���ȴ���Ӧ��ʱ��
  * @return   1��ָ��ͳɹ�
			  0��ָ���ʧ��
  * @DateTime 2019-7-22
**/
bool ESP8266_Cmd(char * cmd, char * reply1, char * reply2, uint32_t waittime)
{
	// ���¿�ʼ�����µ����ݰ�
	strEsp8266_Fram_Record.InfBit .FramLength = 0;
	// ��ESP8266����ָ��
	ESP8266_Usart ( "%s\r\n", cmd );
	// ����Ҫ���ջظ�����ֱ�ӷ���
	if ((reply1 == 0) && (reply2 == 0))
	{
		return true;
	}
	// ��ʱ�ȴ�
	delay_ms(waittime); 
	// �жϽ����Ƿ���ɣ���ͨ�����ջ��������һ��ֵ�Ƿ����ַ����Ľ�β'\0'ʵ��
	strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength]  = '\0';
	// �ѽ��յ����ַ�����������Զ˴��ڵ��������ϲ鿴��PC_UsartΪprintf�ĺ궨�壬
	// ��printf�ض���ָ���˴���1��
	PC_Usart("%s", strEsp8266_Fram_Record.Data_RX_BUF);
	
	// ͨ��C��׼�⺯��strstr �����Ƿ���յ�����Ҫ�ظ����ַ��������򷵻�1����֮����0
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
  * @brief    ��ESP8266ģ�����AT��������
  * @param	  ��
  * @return   1��ѡ��ɹ�
			  0��ѡ��ʧ��
  * @DateTime 2019-7-22
**/
bool ESP8266_AT_Test(void)
{
	char count = 0;
	// ���߸�λ���ţ��˳���λ
	ESP8266_RST_HIGH();
	delay_ms(1000);
	while (count < 10)
	{
		// ����AT�����Ƿ��յ�ESP8266����Ӧ�����û�յ��ظ������ز����Ϊ����ʮ��
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
  * @brief    ѡ��ESP8266ģ��Ĺ���ģʽ 
  * @param	  ����Ϊ��Ӧͷ�ļ��ж����ö�����ͣ����������֣�
			  1-   STA
			  2-   AP
			  3-   STA_AP
  * @return   1��ѡ��ɹ�
			  0��ѡ��ʧ��
  * @DateTime 2019-7-22
**/
bool ESP8266_Net_Mode_Choose (ENUM_Net_ModeTypeDef enumMode)
{
	bool result = 0;
	char count = 0;
	// �������Ϊ10��
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
  * @brief    ESP8266ģ�������ⲿWiFi 
  * @param	  pSSID��    WiFi�����ַ���
			  pPassWord��WiFi�����ַ���
  * @return   1�����ӳɹ�
			  0������ʧ��
  * @DateTime 2019-7-22
**/
bool ESP8266_JoinAP(char * pSSID, char * pPassWord)
{
	char cCmd [120];
	char count = 0;
	// ������+SSID+PASSWORD����ƴ�ӳ�Ҫ���͵������ַ���
	sprintf(cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord);
	// ����10��
	while (count < 10)
	{
		// �ж��Ƿ����óɹ�
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
  * @brief    ESP8266ģ�鴴��WiFi�ȵ� 
  * @param	  pSSID��    WiFi�����ַ���
			  pPassWord��WiFi�����ַ���
  * @return   1�����óɹ�
			  0������ʧ��
  * @DateTime 2019-7-22
**/
bool ESP8266_BuildAP(char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode)
{
	char cCmd [120];
	char count = 0;
	// ������+SSID+PASSWORD����ƴ�ӳ�Ҫ���͵������ַ���
	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%d", pSSID, pPassWord, enunPsdMode );
	// ����10��
	while (count < 10)
	{
		// �ж�AP�Ƿ����óɹ�
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
  * @brief    ESP8266ģ������������ 
  * @param	  enumEnUnvarnishTx�������Ƿ������
		      ȡֵ��ENABLE or DISABLE  
  * @return   1�����óɹ�
			  0������ʧ��
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
  * @brief    ESP8266ģ�������ⲿ������ 
  * @param	  enumE�� ����Э�飬TCP/UDP
		      ip��	  ������IP�ַ���
              Port���������˿��ַ���
		      id��    ģ�����ӷ�������ID
  * @return   1�����ӳɹ�
			  0������ʧ��
  * @DateTime 2019-7-22
**/
bool ESP8266_Link_Server(ENUM_NetPro_TypeDef enumE, char * ip, char * Port, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = {0}, cCmd [120];

	switch(enumE)
	{
	case enumTCP:
		sprintf(cStr, "\"%s\",\"%s\",%s", "TCP", ip, Port);	// ƴ��Ҫ���͵�TCP������
		break;
	case enumUDP:
		sprintf(cStr, "\"%s\",\"%s\",%s", "UDP", ip, Port);	// ƴ��Ҫ���͵�UDP������
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
	// ����ָ��
	return ESP8266_Cmd(cCmd, "OK", "ALREAY CONNECT", 4000);
}

/**
  * @name	   ESP8266_StartOrShutServer
  * @brief     ESP8266ģ�鿪����رշ�����ģʽ 
  * @param	   enumMode�� ������ENABLE��/�رգ�DISABLE��
               pPortNum�� �������˿ں��ַ���
		       pTimeOver����������ʱʱ���ַ�������λ����
  * @return    1�������ɹ�
			   0������ʧ��
  * @DateTime  2019-7-22
**/
bool ESP8266_StartOrShutServer(FunctionalState enumMode, char * pPortNum, char * pTimeOver)
{
	char cCmd1 [120], cCmd2 [120];

	if(enumMode)
	{
		// ���÷������˿ں�
		sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum);
		// ���÷�������ʱʱ��
		sprintf(cCmd2, "AT+CIPSTO=%s", pTimeOver);			

		return (ESP8266_Cmd (cCmd1, "OK", 0, 500) && ESP8266_Cmd (cCmd2, "OK", 0, 500));
	}
	else
	{
		 // �رշ�����
		sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum);

		return ESP8266_Cmd(cCmd1, "OK", 0, 500);
	}
}

/**
  * @name	   ESP8266_Get_LinkStatus
  * @brief     ��ȡESP8266 ������״̬�����ʺϵ��˿�ʱʹ�� 
  * @param	   ��
  * @return    2�����ip
               3����������
		       4��ʧȥ����
               0����ȡ״̬ʧ��
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
  * @brief     ��ȡESP8266 �Ķ˿ڣ�Id������״̬�����ʺ϶�˿�ʱʹ��
  * @param	   ��
  * @return    �˿ڣ�Id��������״̬����5λΪ��Чλ���ֱ��ӦId5~0��ĳλ����1���Id���������ӣ�
			   ������0���Idδ�������ӡ�
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
  * @brief     ����ģ��� AP IP
  * @param	   pApIp��ģ��� AP IP
  * @return    1�����óɹ�
			   0������ʧ��
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
  * @brief     ��ȡESP8266 �� AP IP
  * @param	   pApIp��		  ��� AP IP ��������׵�ַ
  			   ucArrayLength����� AP IP ������ĳ���
  * @return    1����ȡ�ɹ�
  			0����ȡʧ��
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
  * @brief    ����ESP8266ģ�����͸������
  * @param	  ��
  * @return   1��Ϊ������
              0���ǵ�����
  * @DateTime 2019-7-22
**/
bool ESP8266_UnvarnishSend(void)
{
	// �ж��Ƿ�Ϊ������ģʽ
	if (!ESP8266_Cmd("AT+CIPMODE=1", "OK", 0, 1000))
	{
		return false;
	}
	return  ESP8266_Cmd("AT+CIPSEND", "OK", ">", 1000);
}

/**
  * @name	   ESP8266_ExitUnvarnishSend
  * @brief     ����ESP8266ģ���˳�͸��ģʽ
  * @param	   ��
  * @return    ��
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
 * @brief     ESP8266ģ�鷢���ַ���
 * @param	  enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
              pStr��			 Ҫ���͵��ַ���
              ucId��			 �ĸ�ID���͵��ַ���
              ulStrLength�� 	 Ҫ���͵��ַ������ֽ���
 * @return    1�����ͳɹ�
              0������ʧ��
 * @DateTime  2019-7-22
**/
bool ESP8266_SendString(FunctionalState enumEnUnvarnishTx, char * pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId)
{
	char cStr [20];
	bool bRet = false;

	if(enumEnUnvarnishTx)	// ͸��ģʽ
	{
		ESP8266_Usart("%s", pStr);
		bRet = true;
	}
	else					
	{
		if(ucId < 5)		// ������ģʽ
		{
			sprintf(cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2);
		}
		else			    // ������ģʽ
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
 * @brief     ESP8266ģ������ַ���
 * @param	  enumEnUnvarnishTx���Ƿ���ʹ����͸��ģʽ
 * @return    ���յ����ַ����׵�ַ
 * @DateTime  2019-7-22
**/
char * ESP8266_ReceiveString(FunctionalState enumEnUnvarnishTx)
{
	char * pRecStr = 0;
	// ���֡���Ⱥ�֡������ɱ�־λ
	strEsp8266_Fram_Record.InfBit.FramLength = 0;
	strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;
	// �ȴ�֡��־λ����λ
	while(!strEsp8266_Fram_Record.InfBit.FramFinishFlag);
	// ���յ����ַ�����ӽ�����'\0'
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
 * @brief     ESP8266 ������ջ�����
 * @param	  ��
 * @return    ��
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
 * @brief     ESP8266 �����жϷ�����
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-22
**/
void ESP8266_USARTx_IRQHANDLER(void)
{
	uint8_t esp8266_rxdata;
	
	// �ж��Ƿ��ǽ����ж�
	if(USART_GetITStatus(ESP8266_USARTx, USART_IT_RXNE) != RESET)		    // ����һ���ֽ�����
	{
		// ����һ���ֽ�
		esp8266_rxdata = USART_ReceiveData(ESP8266_USARTx);
		// �жϽ��ջ������Ƿ������������յ����ֽڴ�����ջ�����
		if(strEsp8266_Fram_Record.InfBit.FramLength < (RX_BUF_MAX_LEN-1))   // Ԥ��1���ֽ�д������  
		{		
			strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength++] = esp8266_rxdata;
		}
	}
	// ֡�ж�
	if(USART_GetITStatus(ESP8266_USARTx, USART_IT_IDLE) == SET)      		// �ж�һ֡�������ݽ������
	{
		strEsp8266_Fram_Record.InfBit.FramFinishFlag = 1;
		esp8266_rxdata = USART_ReceiveData(ESP8266_USARTx);
		ucTcpClosedFlag = strstr(strEsp8266_Fram_Record.Data_RX_BUF, "CLOSED\r\n" ) ? 1 : 0;
	}
}
