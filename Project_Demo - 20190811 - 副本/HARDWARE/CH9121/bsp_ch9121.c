#include <bsp_ch9121.h>

// ��ע������������֮����510ms
// ������պͷ��ͻ��������ö�̬�����С
uint8_t net_tx_buf[1024];
uint8_t net_rx_buf[1024];

// �����־
uint16_t overflow = 0;
uint8_t overflow_flag = 0;


// ģ�鴮�ڲ�����ʼ��
Serial serial = 
{
    921600,
    None,
    0x08,
    0x01,
    0
};

// ģ��ͻ��˽ṹ���Ա��ʼ��
Net_CommunitionType net_communition_Client = 
{
    2000,                   // �ͻ��˶˿ں�
    {192, 168, 1, 200},     // �ͻ���ip��ַ
    {255, 255, 255, 0},     // �ͻ�����������
    {192, 168, 1, 1},       // �ͻ���Ĭ������
};

// ģ�����������˽ṹ���Ա
Net_CommunitionType net_communition_Server1 = 
{
    2001,                   // �������˿ں�
    {192, 168, 1, 202},     // ������ip��ַ
    {255, 255, 255, 0},     // ��������������
    {192, 168, 1, 1},       // ������Ĭ������
};

// ���ģ����TCP clientģʽ������Ĳ��������޸�ΪĿ�ķ�������Ӧ����
Net_CommunitionType net_communition_DstServer = 
{
    1000,                   // �������˿ں�
    {192, 168, 1, 125},     // ������ip��ַ
    {255, 255, 255, 0},     // ��������������
    {192, 168, 1, 1},       // ������Ĭ������
};

// ��������ģʽ
// �����ڲ���д������Ҫ�ȵ��ô˺�����������ģʽ
static void ch9121_config_mode(void)
{
	// ���������ֽ�������ģʽ
	Usartx_SendByte(M_UARTX, 0x55);
	Usartx_SendByte(M_UARTX, 0xaa);
	Usartx_SendByte(M_UARTX, 0x5a);
    delay_ms(100);
    M_UARTX->DR = 0XA5;
}

static void ch9121_exitconfige(void)
{
	// ���������ֽ�������ģʽ
    WRITE_CMD;
	Usartx_SendByte(M_UARTX, EXIT_SERIAL_CONFIG);
}

// �������ò�����EEPROM
static void update_config(void)
{
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, UPDATE_CONFIG);
}

// ִ������
static void execution_config(void)
{
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, CMD_EXECUTION);
}

// ����ΪоƬΪTCP Serverģʽ
static void ch9121_TCP_Server(void)
{
	// оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ����Ϊ������ģʽ
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_MODE);
    Usartx_SendByte(M_UARTX, TCP_Server_mode);
    delay_ms(100);
    // ����оƬIP��ַ 192.168.1.202
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_IP);
    Usartx_SendByte(M_UARTX, net_communition_Server1.ip_address[0]);
    Usartx_SendByte(M_UARTX, net_communition_Server1.ip_address[1]);
    Usartx_SendByte(M_UARTX, net_communition_Server1.ip_address[2]);
    Usartx_SendByte(M_UARTX, net_communition_Server1.ip_address[3]);
    delay_ms(100);
    // ����оƬ�������� 255.255.255.0
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_SUBNET_MASK);
    Usartx_SendByte(M_UARTX, net_communition_Server1.subnet_mask[0]);
    Usartx_SendByte(M_UARTX, net_communition_Server1.subnet_mask[1]);    
    Usartx_SendByte(M_UARTX, net_communition_Server1.subnet_mask[2]);
    Usartx_SendByte(M_UARTX, net_communition_Server1.subnet_mask[2]);  
    delay_ms(100);    
    // ����оƬ���� 192.168.1.1
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_GATEWAY); 
    Usartx_SendByte(M_UARTX, net_communition_Server1.gateway[0]);
    Usartx_SendByte(M_UARTX, net_communition_Server1.gateway[1]);
    Usartx_SendByte(M_UARTX, net_communition_Server1.gateway[2]);
    Usartx_SendByte(M_UARTX, net_communition_Server1.gateway[3]);
    delay_ms(100);
    // ���÷������˿�2001���˿ںŷ����ֽ������ȷ����ֽڣ��ٷ����ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_LOCAL_PORT);  
    Usartx_SendByte(M_UARTX, net_communition_Server1.port_num & 0x0ff);
    Usartx_SendByte(M_UARTX, (uint8_t)(net_communition_Server1.port_num>>8));
    delay_ms(100);
    // �������õ�EEPROM
    update_config();
    delay_ms(100);
    // ִ������
    execution_config();
    delay_ms(100);
    // �˳�����ģʽ
    ch9121_exitconfige();
}

// ����ΪTCP Clientģʽ
static void ch9121_TCP_Client(void)
{
	// оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ����Ϊ�ͻ���ģʽ
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_MODE);
    Usartx_SendByte(M_UARTX, TCP_Client_mode);
    delay_ms(100);
/*
//  �Ѵ���λ���������ΪDHCPģʽ������Ҫ����IP�����غ��������룬�˿ں�����Ϊ�����
    // ����оƬIP��ַ 192.168.1.202   
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_IP);
    Usartx_SendByte(M_UARTX, net_communition_Client.ip_address[0]);
    Usartx_SendByte(M_UARTX, net_communition_Client.ip_address[1]);
    Usartx_SendByte(M_UARTX, net_communition_Client.ip_address[2]);
    Usartx_SendByte(M_UARTX, net_communition_Client.ip_address[3]);
    // ����оƬ�������� 255.255.255.0
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_SUBNET_MASK);
    Usartx_SendByte(M_UARTX, net_communition_Client.subnet_mask[0]);
    Usartx_SendByte(M_UARTX, net_communition_Client.subnet_mask[1]);    
    Usartx_SendByte(M_UARTX, net_communition_Client.subnet_mask[2]);
    Usartx_SendByte(M_UARTX, net_communition_Client.subnet_mask[2]);    
    // ����оƬ���� 192.168.1.1
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_GATEWAY); 
    Usartx_SendByte(M_UARTX, net_communition_Client.gateway[0]);
    Usartx_SendByte(M_UARTX, net_communition_Client.gateway[1]);
    Usartx_SendByte(M_UARTX, net_communition_Client.gateway[2]);
    Usartx_SendByte(M_UARTX, net_communition_Client.gateway[3]);
    // ���÷������˿�2001���˿ںŷ����ֽ������ȷ����ֽڣ��ٷ����ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_LOCAL_PORT);  
    Usartx_SendByte(M_UARTX, net_communition_Client.port_num & 0x0ff);
    Usartx_SendByte(M_UARTX, (uint8_t)net_communition_Client.port_num>>8);
*/
    // ����Ŀ�ķ�����IP��ַ
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_DESTINATION_IP);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[0]);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[1]);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[2]);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[3]);
    delay_ms(100);
    // ����Ŀ�ķ������˿�1000���˿ںŷ����ֽ������ȷ����ֽڣ��ٷ����ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_DESTINATION_PORT);  
    Usartx_SendByte(M_UARTX, net_communition_DstServer.port_num & 0x0ff);
    Usartx_SendByte(M_UARTX, (uint8_t)(net_communition_DstServer.port_num>>8));   
    delay_ms(100);    
    // �������õ�EEPROM
    update_config();
    delay_ms(100);
    // ִ������
    execution_config();
    delay_ms(100);
    // �˳�����ģʽ
    ch9121_exitconfige();
}

// ����ΪUDP Clientģʽ
static void ch9121_UDP(void)
{
	// оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ����ΪUDP�ͻ���ģʽ
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_MODE);
    Usartx_SendByte(M_UARTX, UDP_Client_mode);
    delay_ms(100);
    // ����Ŀ�ķ�����IP��ַ
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_DESTINATION_IP);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[0]);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[1]);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[2]);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[3]);
    delay_ms(100);
    // ����Ŀ�ķ������˿�1000���˿ںŷ����ֽ������ȷ����ֽڣ��ٷ����ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_DESTINATION_PORT);  
    Usartx_SendByte(M_UARTX, net_communition_DstServer.port_num & 0x0ff);
    Usartx_SendByte(M_UARTX, (uint8_t)(net_communition_DstServer.port_num>>8)); 
    delay_ms(100);
    // �������õ�EEPROM
    update_config();
    delay_ms(100);
    // ִ������
    execution_config();
    delay_ms(100);
    // �˳�����ģʽ
    ch9121_exitconfige();    
}

// ����ΪUDP Serverģʽ
static void ch9121_UDP_Server(void)
{
	// оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ����ΪUDP��������ģʽ
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_CHIP_MODE);
    Usartx_SendByte(M_UARTX, UDP_Server_mode);
    delay_ms(100);
    // ����Ŀ�ķ�����IP��ַ
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_DESTINATION_IP);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[0]);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[1]);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[2]);
    Usartx_SendByte(M_UARTX, net_communition_DstServer.ip_address[3]);
    delay_ms(100);
    // ����Ŀ�ķ������˿�1000���˿ںŷ����ֽ������ȷ����ֽڣ��ٷ����ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_DESTINATION_PORT);  
    Usartx_SendByte(M_UARTX, net_communition_DstServer.port_num & 0x0ff);
    Usartx_SendByte(M_UARTX, (uint8_t)(net_communition_DstServer.port_num>>8)); 
    // �������õ�EEPROM
    update_config();
    delay_ms(100);
    // ִ������
    execution_config();   
    delay_ms(100);    
    // �˳�����ģʽ
    ch9121_exitconfige();
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

// ���ô�����ز���
static void serial_config(void)
{
    // оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ���ô��ڲ�����926100����Ҫ�ﹻ4�ֽڣ������ֽ������ȷ����ֽڣ��ٷ����ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_SERIAL_BOUND);
    Usartx_SendByte(M_UARTX, serial.bound&0xff);
    Usartx_SendByte(M_UARTX, (serial.bound>>8)&0xff);
    Usartx_SendByte(M_UARTX, (serial.bound>>16)&0xff);
    Usartx_SendByte(M_UARTX, (serial.bound>>24)&0xff);
    delay_ms(100);
    // ���ô�������֡��ʽ��1λֹͣλ����У�飬8λ����λ
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, SET_SERIAL_DATA_FORMAT);
    Usartx_SendByte(M_UARTX, serial.stop_bit);
    Usartx_SendByte(M_UARTX, serial.serial_check);
    Usartx_SendByte(M_UARTX, serial.data_bit);
    delay_ms(100);
    // �������õ�EEPROM
    update_config();
    delay_ms(100);
    // ִ������
    execution_config();
    delay_ms(100);
    // �˳�����ģʽ
    ch9121_exitconfige();
}

// ��������
void send_netdata(uint8_t * t_buf)
{
    while(*t_buf != '\0')
    {
        Usartx_SendByte(M_UARTX, *t_buf);
        t_buf++;
    }
}


/*************************************************************************************************************************************************
                                                    ��ȡ��غ���
**************************************************************************************************************************************************/
// ��ȡоƬ����ģʽ������һ���ֽ�
uint8_t read_WorkMode(void)
{
    uint8_t netmode;
    // ÿ�ν����Ȱѻ��������±����㣬����ȡ��
    overflow = 0;
    // оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ��ȡоƬ����ģʽ������һ���ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, READ_CHIP_MODE);
    delay_ms(100);
    // �˳�����ģʽ
    ch9121_exitconfige();

    netmode = net_rx_buf[2];
    printf("%d\r\n", netmode);
    switch(netmode)
    {
    case 0:
        return TCP_Server_mode;
    case 1:
        return TCP_Client_mode;
    case 2:
        return UDP_Server_mode;
    case 3:
        return UDP_Client_mode;
    }
    return 0xff;
}

// ��ȡоƬIP
void read_chip_ip(uint8_t *ip_addr)
{
    uint8_t i;
    // ÿ�ν����Ȱѻ��������±����㣬����ȡ��
    overflow = 0;
    // оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ��ȡоƬ����ģʽ������һ���ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, READ_CHIP_IP);
    delay_ms(100);
    for(i = 0; i < 4; i++)
    {
        ip_addr[i] = net_rx_buf[i+2];
        
    }
    // �˳�����ģʽ
    ch9121_exitconfige();
    printf("%d.%d.%d.%d\r\n", ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]); 
}

// ��ȡԴ�˿�
uint16_t read_source_port(void)
{
    uint16_t _port;
    // ÿ�ν����Ȱѻ��������±����㣬����ȡ��
    overflow = 0;
    // оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ��ȡоƬ����ģʽ������һ���ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, READ_SOURCE_PORT);
    delay_ms(100);   
    // �˳�����ģʽ
    ch9121_exitconfige();
    
    _port = net_rx_buf[2];
    _port |= (net_rx_buf[3]<<8);  
    printf("%d\r\n", _port);
    return _port;
}
    
// ��ȡĿ��IP
void read_distination_ip(uint8_t *ip_addr)
{
    uint8_t i;
    // ÿ�ν����Ȱѻ��������±����㣬����ȡ��
    overflow = 0;
    // оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ��ȡоƬ����ģʽ������һ���ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, READ_DESTINATION_IP);
    delay_ms(100);
    for(i = 0; i < 4; i++)
    {
        ip_addr[i] = net_rx_buf[i+2];
        
    }
    // �˳�����ģʽ
    ch9121_exitconfige();
    printf("%d.%d.%d.%d\r\n", ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]); 
}

// ��ȡĿ�Ķ˿�
uint16_t read_distination_port(void)
{
    uint16_t _port;
    // ÿ�ν����Ȱѻ��������±����㣬����ȡ��
    overflow = 0;
    // оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    // ��ȡоƬ����ģʽ������һ���ֽ�
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, READ_DESTINATION_PORT);
    delay_ms(100);   
    // �˳�����ģʽ
    ch9121_exitconfige();
    
    _port = net_rx_buf[2];
    _port |= (net_rx_buf[3]<<8);  
    printf("%d\r\n", _port);
    return _port;
}

// ��λоƬ
void reset_chip(void)
{
    // оƬ��������ģʽ
	ch9121_config_mode();
    delay_ms(100);
    WRITE_CMD;
    Usartx_SendByte(M_UARTX, RESET_CHIP);
    delay_ms(100);
    // �˳�����ģʽ
    ch9121_exitconfige();
}

// ��ʼ�����ú���
void init_ch9121(void)
{
    // ���ù���ģʽ
    ch9121_mode_select(TCP_Client_mode);
}

/**
 * @name	  UART4_IRQHandler
 * @brief     ����4�жϷ�������Ŀǰ����CH9121����ͨ��ģ��
 * @param	  ��
 * @return    ��
 * @DateTime  2019-7-20
 */
void UART4_IRQHandler(void)
{
    uint8_t return_val = 0;
	if(USART_GetITStatus( M_UARTX, USART_IT_RXNE ) != RESET)
	{		
        return_val = USART_ReceiveData(M_UARTX);
        if(overflow > 1022)
        {
            overflow = 0;
            overflow_flag = 1;
            net_rx_buf[1023] = '\0';
        }
        else 
        {
            net_rx_buf[overflow++] = return_val;
        }
	} 
}
