#include <app.h>

// ��������ģ���Flash����һ��SPI������Ҫע��Ƭѡ
// Flashģʽ
void flash_mode_app(void)
{
	nRF24L01_CSN_1();		// ʧ��NRF24L01��Ƭѡ��
	W25QXX_Init();			// ʹ���ⲿFlash
}

// NRF24L01ģʽ
void nrf_mode_app(void)
{
	W25QXX_CS = 1;          // ʧ��Flash
	nRF24L01_InitHard();    // ��ʼ��NRF24L01ģ��
	while (!nRF24L01_ConnectCheck())
	{
		printf("ģ������ʧ�ܣ�\r\n");
	}
}

// ��ʼ��SD������ͨ�����ڴ�ӡSD�������Ϣ���Գ���
void show_sdcard_info_app(void)
{
	while (SD_Init())               // ��ʼ��SD��
	{
		POINT_COLOR = RED;
		LCD_ShowString(30, 0, 200, 16, 16, (uint8_t *)"SD Card Error!");
		delay_ms(500);
		LCD_ShowString(30, 0, 200, 16, 16, (uint8_t *)"Please Check !");
		delay_ms(500);
		LED1 = !LED1;
	}
	// ��ӡSD������
	switch (SDCardInfo.CardType)
	{
	case SDIO_STD_CAPACITY_SD_CARD_V1_1:
		printf("Card Type:SDSC V1.1\r\n");
		break;
	case SDIO_STD_CAPACITY_SD_CARD_V2_0:
		printf("Card Type:SDSC V2.0\r\n");
		break;
	case SDIO_HIGH_CAPACITY_SD_CARD:
		printf("Card Type:SDHC V2.0\r\n");
		break;
	case SDIO_MULTIMEDIA_CARD:
		printf("Card Type:MMC Card\r\n");
		break;
	}
	printf("Card ManufacturerID:%d\r\n", SDCardInfo.SD_cid.ManufacturerID);	// ������ID
	printf("Card RCA:%d\r\n", SDCardInfo.RCA);								// ����Ե�ַ
	printf("Card Capacity:%d MB\r\n", (u32)(SDCardInfo.CardCapacity >> 20));// ��ʾ����
	printf("Card BlockSize:%d\r\n\r\n", SDCardInfo.CardBlockSize);			// ��ʾ���С

	POINT_COLOR = BLUE;	                                                    // ��������Ϊ��ɫ
	// ���SD���ɹ�
	LCD_ShowString(30, 0, 200, 16, 16, (uint8_t *)"SD Card OK    ");
	LCD_ShowString(30, 20, 200, 16, 16, (uint8_t *)"SD Card Size:      MB");
	LCD_ShowNum(13 * 8 + 30, 20, SDCardInfo.CardCapacity >> 20, 5, 16);     // ��ʾSD������
}

// ���ڴ�ӡ��ʾ�ڴ�ռ����
// ������memx��ʾҪ�鿴ռ���ʵ��ڴ�飬��malloc.h���ж��壬�ֱ�ΪSRAMIN��SRAMCCM
void show_memory_occupancy_app(uint8_t memx)
{
	uint8_t occupancy;
	occupancy = my_mem_perused(memx);
	printf("�ڴ�ռ����Ϊ��%d%%", occupancy);
}

// ����SD�����ⲿFlash
void fatfs_mount_app(void)
{
	uint32_t total, free;
	uint8_t res = 0;

	exfuns_init();			// Ϊfatfs��ر��������ڴ�
	f_mount(fs[0], "0:", 1);// ����SD��
	f_mount(fs[1], "1", 1);	// �����ⲿFlashǰ12M�ռ�
	if (res == 0X0D)			// FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		LCD_ShowString(30, 150, 200, 16, 16, "Flash Disk Formatting...");		// ��ʽ��FLASH
		res = f_mkfs("1:", 1, 4096);											// ��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if (res == 0)
		{
			f_setlabel((const TCHAR *)"1:veis");							    // ����Flash���̵�����Ϊ��veis
			LCD_ShowString(30, 150, 200, 16, 16, "Flash Disk Format Finish");	// ��ʽ�����
		} else LCD_ShowString(30, 150, 200, 16, 16, "Flash Disk Format Error ");// ��ʽ��ʧ��
		delay_ms(1000);
	}
	while (exf_getfree("0", &total, &free))									   // �õ�SD������������ʣ������
	{
		Show_Str(30, 40, 200, 16, "SD ERROR!", 16, 0);
		delay_ms(200);
		LCD_Fill(30, 40, 240, 150 + 16, WHITE);								    // �����ʾ
		delay_ms(200);
		LED1 = !LED1;
	}
	POINT_COLOR = RED;														    // ��������Ϊ��ɫ
	Show_Str(30, 40, 200, 16, "FATFS OK", 16, 0);
	Show_Str(30, 60, 200, 16, "SD total size:      MB", 16, 0);
	Show_Str(30, 80, 200, 16, "SD free size:      MB", 16, 0);
	LCD_ShowNum(30 + 8 * 14, 60, total >> 10, 5, 16);						    // ��ʾSD�������� MB
	LCD_ShowNum(30 + 8 * 14, 80, free >> 10, 5, 16);						    // ��ʾSD��ʣ������ MB
}

void lwip_config_app(void)
{
	uint8_t speed;
	uint8_t buf[30];

	TIM3_Int_Init(999, 839);        // ��lwip�ṩʱ�ӻ�׼��100khz
    
	LCD_ShowString(30, 110, 200, 16, 16, "lwIP Initing...");
	printf("lwIP Initing...\r\n");
	while (lwip_comm_init() != 0)
	{
		LCD_ShowString(30, 110, 200, 16, 16, "lwIP Init failed!");
		printf("lwIP Init failed!");
		delay_ms(1200);
		LCD_Fill(30, 110, 230, 110 + 16, WHITE); // �����ʾ3
		LCD_ShowString(30, 110, 200, 16, 16, "Retrying...");
		printf("Retrying...");
		delay_ms(100);
	}
	LCD_ShowString(30, 110, 200, 16, 16, "lwIP Init Successed");
	printf("lwIP Init Successed\r\n");

#if LWIP_DHCP
	// �ȴ�DHCP��ȡ
	LCD_ShowString(30, 130, 200, 16, 16, "DHCP IP configing...");
	printf("DHCP IP configing...\r\n");
	while ((lwipdev.dhcpstatus != 2) && (lwipdev.dhcpstatus != 0XFF)) // �ȴ�DHCP��ȡ�ɹ�/��ʱ���
	{
		lwip_periodic_handle();
	}
	LCD_Fill(30, 110, lcddev.width, lcddev.height, WHITE);	// �����ʾ
	LCD_ShowString(30, 110, 200, 16, 16, "DHCP Init Successed");
	printf("DHCP Init Successed\r\n");
#endif
	if (lwipdev.dhcpstatus == 2)
	{
		sprintf((char*)buf, "DHCP IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);   // ��ӡ��̬IP��ַ
		printf("DHCP IP:%d.%d.%d.%d\r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
	}
	else
	{
		sprintf((char*)buf, "Static IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]); // ��ӡ��̬IP��ַ
		printf("Static IP:%d.%d.%d.%d\r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
	}
	LCD_ShowString(30, 130, 210, 16, 16, buf);
	speed = LAN8720_Get_Speed();                            // �õ�����
	if (speed & 1 << 1)
	{
		LCD_ShowString(30, 150, 200, 16, 16, "Ethernet Speed:100M");
		printf("Ethernet Speed:100M\r\n");
	}
	else
	{
		LCD_ShowString(30, 150, 200, 16, 16, "Ethernet Speed:10M");
		printf("Ethernet Speed:10M\r\n");
	}
}
// TCP���Գ���
void tcp_server_test_app(void)
{ 
	err_t err;
	struct tcp_pcb *tcppcbnew;  	// ����һ��TCP���������ƿ�
	struct tcp_pcb *tcppcbconn;  	// ����һ��TCP���������ƿ�

	uint8_t *tbuf;
	uint8_t key;
	uint8_t res = 0;
    // �����͵�����
    char str[] = "123456789";
    // ���Ҫ���͵����ݻ�����
    tcp_server_sendbuf = str;
    
	LCD_Clear(WHITE);	    // ����
	POINT_COLOR = RED; 	    // ��ɫ����

	tbuf = mymalloc(SRAMIN, 200);	// �����ڴ�
	if (tbuf == NULL)
        return ;		    // �ڴ�����ʧ����,ֱ���˳�
	sprintf((char*)tbuf, "Server IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]); // ������IP
	LCD_ShowString(30, 130, 210, 16, 16, tbuf);
	sprintf((char*)tbuf, "Server Port:%d", TCP_SERVER_PORT);        // �������˿ں�
	LCD_ShowString(30, 150, 210, 16, 16, tbuf);
	tcppcbnew = tcp_new();	// ����һ���µ�pcb
	if (tcppcbnew)			// �����ɹ�                                                      
	{
		err = tcp_bind(tcppcbnew, IP_ADDR_ANY, TCP_SERVER_PORT);    // ������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
		if (err == ERR_OK)	// �����
		{
			tcppcbconn = tcp_listen(tcppcbnew); 	    // ����tcppcb�������״̬
			tcp_accept(tcppcbconn, tcp_server_accept); 	// ��ʼ��LWIP��tcp_accept�Ļص�����
		}
        else res = 1;
	} 
    else res = 1;
	POINT_COLOR = BLUE;     // ��ɫ����
	while (res == 0)
	{
		key = key_scan(0);
		if (key == WKUP_PRES)
        {
            break;
        }
		if (key == KEY0_PRES)               // KEY0������,��������
		{
			tcp_server_flag |= 1 << 7;      // ���Ҫ��������
		}
		if (tcp_server_flag & 1 << 6)       // �Ƿ��յ�����?
		{
			LCD_Fill(30, 210, lcddev.width - 1, lcddev.height - 1, WHITE); // ����һ������
			LCD_ShowString(30, 210, lcddev.width - 30, lcddev.height - 210, 16, tcp_server_recvbuf); // ��ʾ���յ�������
			tcp_server_flag &= ~(1 << 6);   // ��������Ѿ���������.
		}
		if (tcp_server_flag & 1 << 5)       // �Ƿ�������?
		{
			sprintf((char*)tbuf, "Client IP:%d.%d.%d.%d", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]); //�ͻ���IP
			LCD_ShowString(30, 170, 230, 16, 16, tbuf);
			POINT_COLOR = RED;
			LCD_ShowString(30, 190, lcddev.width - 30, lcddev.height - 190, 16, "Receive Data:");   // ��ʾ��Ϣ
			POINT_COLOR = BLUE;         // ��ɫ����
		} 
        else if ((tcp_server_flag & 1 << 5) == 0)
		{
			LCD_Fill(30, 170, lcddev.width - 1, lcddev.height - 1, WHITE); // ����
		}
		lwip_periodic_handle();
	}
	tcp_server_connection_close(tcppcbnew, 0);  // �ر�TCP Server����
	tcp_server_connection_close(tcppcbconn, 0); // �ر�TCP Server����
	tcp_server_remove_timewait();
	memset(tcppcbnew, 0, sizeof(struct tcp_pcb));
	memset(tcppcbconn, 0, sizeof(struct tcp_pcb));
	myfree(SRAMIN, tbuf);
    
	LCD_Clear(WHITE);
	POINT_COLOR = BLUE;
	LCD_ShowString(30, 90, 200, 16, 16, "Connect break��");
    printf("Connect break!\r\n");
	LCD_ShowString(30, 110, 200, 16, 16, "KEY1:Connect");
}


// UDP���Գ���
void udp_test_app(void)
{
	err_t err;
	struct udp_pcb * udppcb;    // ����һ��UDP���������ƿ�
	struct ip_addr rmtipaddr;  	// Զ��ip��ַ

	uint8_t *tbuf;
	uint8_t res = 0;
	uint8_t key = 0;
	uint8_t *s_buf = (uint8_t *)"UDP����ʵ��!";
    
    LCD_Clear(WHITE);	    // ����
	POINT_COLOR = RED; 	    // ��ɫ����

	// Զ��IP 192.168.1.125��ǰ�漸λͨ��DHCP��ȡ������ͨ����ʼ��ʱ���õľ�̬IP��192.168.1.30
	udp_set_remoteip(125);
	// �����ڴ�
	tbuf = mymalloc(SRAMIN, 200);
	if (NULL == tbuf)
	{
		return;
	}
	sprintf((char*)tbuf, "Local IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);						   // ������IP
	printf("Local IP:%d.%d.%d.%d\r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
	LCD_ShowString(30, 150, 210, 16, 16, tbuf);

	sprintf((char*)tbuf, "Remote IP:%d.%d.%d.%d", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]); // Զ��IP
	printf("Remote IP:%d.%d.%d.%d\r\n", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);
	LCD_ShowString(30, 170, 210, 16, 16, tbuf);

	sprintf((char*)tbuf, "Remote Port:%d", UDP_DEMO_PORT);		            // �ͻ��˶˿ں�
	printf("Remote Port:%d\r\n", UDP_DEMO_PORT);
	LCD_ShowString(30, 190, 210, 16, 16, tbuf);

	POINT_COLOR = BLUE;
	LCD_ShowString(30, 210, 210, 16, 16, "STATUS:Disconnected\r\n");

	// ����һ��UDP���ƿ�
	udppcb = udp_new();

	if (udppcb)
	{
		IP4_ADDR(&rmtipaddr, lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);
		err = udp_connect(udppcb, &rmtipaddr, UDP_DEMO_PORT); 				 // UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if (err == ERR_OK)
		{
			err = udp_bind(udppcb, IP_ADDR_ANY, UDP_DEMO_PORT); 			 // �󶨱���IP��ַ��˿ں�
			if (err == ERR_OK)												 // �����
			{
				udp_recv(udppcb, udp_callback_fun, NULL); 					 // ע����ջص�����
				LCD_ShowString(30, 210, 210, 16, 16, "STATUS:Connected   "); // �����������(UDP�Ƿǿɿ�����,���������ʾ����UDP�Ѿ�׼����)
				printf("STATUS:Connected\r\n");
				udp_demo_flag |= 1 << 5;									 // ����Ѿ�������
				POINT_COLOR = RED;
				LCD_ShowString(30, 230, lcddev.width - 30, lcddev.height - 190, 16, "Receive Data:"); // ��ʾ��Ϣ
				POINT_COLOR = BLUE;                                          // ��ɫ����
			}
			else res = 1;
		}
		else res = 1;
	}
	else res = 1;

	while (res == 0)
	{
		key = key_scan(0);
		if (key == WKUP_PRES)
		{
			break;
		}
		if (key == KEY0_PRES) 		// KEY0������,��������
		{
			udp_send_data(udppcb, s_buf);
		}
		if (udp_demo_flag & 1 << 6) // �Ƿ��յ�����
		{
			LCD_Fill(30, 250, lcddev.width - 1, lcddev.height - 1, WHITE);  	// ����һ������
			LCD_ShowString(30, 250, lcddev.width - 30, lcddev.height - 230, 16, udp_demo_recvbuf); // ��ʾ���յ�������
			printf("%s\n", udp_demo_recvbuf);
			udp_demo_flag &= ~(1 << 6);										    // ��������Ѿ���������.
		}
		lwip_periodic_handle();
		delay_ms(2);
	}
	LCD_Clear(WHITE);
	udp_connection_close(udppcb);
	myfree(SRAMIN, tbuf);
}

// TCP Client ����
void tcp_client_test_app(void)
{
	struct tcp_pcb *tcppcb;  	// ����һ��TCP���������ƿ�
	struct ip_addr rmtipaddr;  	// Զ��ip��ַ

	uint8_t *tbuf;
	uint8_t key;
	uint8_t res = 0;
	uint8_t t = 0;
    
    // �����͵�����
    uint8_t str[] = "123456789";
    // ���Ҫ���͵����ݻ�����
    tcp_client_sendbuf = str;
    
	// ����Զ�˷�����IP��ǰ��λ��DHCP��ȡ��192.168.1.125
	tcp_client_set_remoteip(125);

	LCD_Clear(WHITE);	// ����
	POINT_COLOR = RED; 	// ��ɫ����

	tbuf = mymalloc(SRAMIN, 200);	// �����ڴ�
	if (tbuf == NULL)return ;		// �ڴ�����ʧ����,ֱ���˳�
	sprintf((char*)tbuf, "Local IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]); // ������IP
	LCD_ShowString(30, 150, 210, 16, 16, tbuf);
	sprintf((char*)tbuf, "Remote IP:%d.%d.%d.%d", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]); //Զ��IP
	LCD_ShowString(30, 170, 210, 16, 16, tbuf);
	sprintf((char*)tbuf, "Remotewo Port:%d", TCP_CLIENT_PORT); // �ͻ��˶˿ں�
	LCD_ShowString(30, 190, 210, 16, 16, tbuf);
	POINT_COLOR = BLUE;
	LCD_ShowString(30, 210, 210, 16, 16, "STATUS:Disconnected");
	tcppcb = tcp_new();	// ����һ���µ�pcb
	if (tcppcb)			// �����ɹ�
	{
		IP4_ADDR(&rmtipaddr, lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);
		tcp_connect(tcppcb, &rmtipaddr, TCP_CLIENT_PORT, tcp_client_connected); // ���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
	} else res = 1;
	while (res == 0)
	{
		key = key_scan(0);
		if (key == WKUP_PRES)break;
		if (key == KEY0_PRES)               // KEY0������,��������
		{
			tcp_client_flag |= 1 << 7;      // ���Ҫ��������
		}
		if (tcp_client_flag & 1 << 6)       // �Ƿ��յ�����?
		{
			LCD_Fill(30, 250, lcddev.width - 1, lcddev.height - 1, WHITE); // ����һ������
			LCD_ShowString(30, 250, lcddev.width - 30, lcddev.height - 230, 16, tcp_client_recvbuf);    // ��ʾ���յ�������
			tcp_client_flag &= ~(1 << 6);   // ��������Ѿ���������.
		}
		if (tcp_client_flag & 1 << 5)       // �Ƿ�������?
		{
			LCD_ShowString(30, 210, lcddev.width - 30, lcddev.height - 190, 16, "STATUS:Connected   "); // ��ʾ��Ϣ
			POINT_COLOR = RED;
			LCD_ShowString(30, 230, lcddev.width - 30, lcddev.height - 190, 16, "Receive Data:");       // ��ʾ��Ϣ
			POINT_COLOR = BLUE;             // ��ɫ����
		} 
        else if ((tcp_client_flag & 1 << 5) == 0)
		{
			LCD_ShowString(30, 210, 190, 16, 16, "STATUS:Disconnected");
			LCD_Fill(30, 230, lcddev.width - 1, lcddev.height - 1, WHITE); //����
		}
		lwip_periodic_handle();
		delay_ms(2);
		t++;
		if (t == 200)
		{
			if ((tcp_client_flag & 1 << 5) == 0)        // δ������,��������
			{
				tcp_client_connection_close(tcppcb, 0); // �ر�����
				tcppcb = tcp_new();	                    // ����һ���µ�pcb
				if (tcppcb)			                    // �����ɹ�
				{
					tcp_connect(tcppcb, &rmtipaddr, TCP_CLIENT_PORT, tcp_client_connected); // ���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
				}
			}
			t = 0;
            LED1 = !LED1;
		}
	}
	tcp_client_connection_close(tcppcb, 0); // �ر�TCP Client����
	LCD_Clear(WHITE);
	POINT_COLOR = BLUE;
	LCD_ShowString(30, 90, 200, 16, 16, "Connect break��");
	LCD_ShowString(30, 110, 200, 16, 16, "KEY1:Connect");
    printf("Connect break��\r\n");
	myfree(SRAMIN, tbuf);
}

