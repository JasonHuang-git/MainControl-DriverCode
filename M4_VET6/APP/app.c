#include <app.h>

// 由于无线模块和Flash共用一组SPI，所以要注意片选
// Flash模式
void flash_mode_app(void)
{
	nRF24L01_CSN_1();		// 失能NRF24L01的片选脚
	W25QXX_Init();			// 使能外部Flash
}

// NRF24L01模式
void nrf_mode_app(void)
{
	W25QXX_CS = 1;          // 失能Flash
	nRF24L01_InitHard();    // 初始化NRF24L01模块
	while (!nRF24L01_ConnectCheck())
	{
		printf("模块连接失败！\r\n");
	}
}

// 初始化SD卡，并通过串口打印SD卡相关信息测试程序
void show_sdcard_info_app(void)
{
	while (SD_Init())               // 初始化SD卡
	{
		POINT_COLOR = RED;
		LCD_ShowString(30, 0, 200, 16, 16, (uint8_t *)"SD Card Error!");
		delay_ms(500);
		LCD_ShowString(30, 0, 200, 16, 16, (uint8_t *)"Please Check !");
		delay_ms(500);
		LED1 = !LED1;
	}
	// 打印SD卡类型
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
	printf("Card ManufacturerID:%d\r\n", SDCardInfo.SD_cid.ManufacturerID);	// 制造商ID
	printf("Card RCA:%d\r\n", SDCardInfo.RCA);								// 卡相对地址
	printf("Card Capacity:%d MB\r\n", (u32)(SDCardInfo.CardCapacity >> 20));// 显示容量
	printf("Card BlockSize:%d\r\n\r\n", SDCardInfo.CardBlockSize);			// 显示块大小

	POINT_COLOR = BLUE;	                                                    // 设置字体为蓝色
	// 检测SD卡成功
	LCD_ShowString(30, 0, 200, 16, 16, (uint8_t *)"SD Card OK    ");
	LCD_ShowString(30, 20, 200, 16, 16, (uint8_t *)"SD Card Size:      MB");
	LCD_ShowNum(13 * 8 + 30, 20, SDCardInfo.CardCapacity >> 20, 5, 16);     // 显示SD卡容量
}

// 串口打印显示内存占用率
// 参数：memx表示要查看占用率的内存块，在malloc.h中有定义，分别为SRAMIN和SRAMCCM
void show_memory_occupancy_app(uint8_t memx)
{
	uint8_t occupancy;
	occupancy = my_mem_perused(memx);
	printf("内存占用率为：%d%%", occupancy);
}

// 挂载SD卡和外部Flash
void fatfs_mount_app(void)
{
	uint32_t total, free;
	uint8_t res = 0;

	exfuns_init();			// 为fatfs相关变量申请内存
	f_mount(fs[0], "0:", 1);// 挂载SD卡
	f_mount(fs[1], "1", 1);	// 挂载外部Flash前12M空间
	if (res == 0X0D)			// FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	{
		LCD_ShowString(30, 150, 200, 16, 16, "Flash Disk Formatting...");		// 格式化FLASH
		res = f_mkfs("1:", 1, 4096);											// 格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if (res == 0)
		{
			f_setlabel((const TCHAR *)"1:veis");							    // 设置Flash磁盘的名字为：veis
			LCD_ShowString(30, 150, 200, 16, 16, "Flash Disk Format Finish");	// 格式化完成
		} else LCD_ShowString(30, 150, 200, 16, 16, "Flash Disk Format Error ");// 格式化失败
		delay_ms(1000);
	}
	while (exf_getfree("0", &total, &free))									   // 得到SD卡的总容量和剩余容量
	{
		Show_Str(30, 40, 200, 16, "SD ERROR!", 16, 0);
		delay_ms(200);
		LCD_Fill(30, 40, 240, 150 + 16, WHITE);								    // 清除显示
		delay_ms(200);
		LED1 = !LED1;
	}
	POINT_COLOR = RED;														    // 设置字体为红色
	Show_Str(30, 40, 200, 16, "FATFS OK", 16, 0);
	Show_Str(30, 60, 200, 16, "SD total size:      MB", 16, 0);
	Show_Str(30, 80, 200, 16, "SD free size:      MB", 16, 0);
	LCD_ShowNum(30 + 8 * 14, 60, total >> 10, 5, 16);						    // 显示SD卡总容量 MB
	LCD_ShowNum(30 + 8 * 14, 80, free >> 10, 5, 16);						    // 显示SD卡剩余容量 MB
}

void lwip_config_app(void)
{
	uint8_t speed;
	uint8_t buf[30];

	TIM3_Int_Init(999, 839);        // 给lwip提供时钟基准，100khz
    
	LCD_ShowString(30, 110, 200, 16, 16, "lwIP Initing...");
	printf("lwIP Initing...\r\n");
	while (lwip_comm_init() != 0)
	{
		LCD_ShowString(30, 110, 200, 16, 16, "lwIP Init failed!");
		printf("lwIP Init failed!");
		delay_ms(1200);
		LCD_Fill(30, 110, 230, 110 + 16, WHITE); // 清除显示3
		LCD_ShowString(30, 110, 200, 16, 16, "Retrying...");
		printf("Retrying...");
		delay_ms(100);
	}
	LCD_ShowString(30, 110, 200, 16, 16, "lwIP Init Successed");
	printf("lwIP Init Successed\r\n");

#if LWIP_DHCP
	// 等待DHCP获取
	LCD_ShowString(30, 130, 200, 16, 16, "DHCP IP configing...");
	printf("DHCP IP configing...\r\n");
	while ((lwipdev.dhcpstatus != 2) && (lwipdev.dhcpstatus != 0XFF)) // 等待DHCP获取成功/超时溢出
	{
		lwip_periodic_handle();
	}
	LCD_Fill(30, 110, lcddev.width, lcddev.height, WHITE);	// 清除显示
	LCD_ShowString(30, 110, 200, 16, 16, "DHCP Init Successed");
	printf("DHCP Init Successed\r\n");
#endif
	if (lwipdev.dhcpstatus == 2)
	{
		sprintf((char*)buf, "DHCP IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);   // 打印动态IP地址
		printf("DHCP IP:%d.%d.%d.%d\r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
	}
	else
	{
		sprintf((char*)buf, "Static IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]); // 打印静态IP地址
		printf("Static IP:%d.%d.%d.%d\r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
	}
	LCD_ShowString(30, 130, 210, 16, 16, buf);
	speed = LAN8720_Get_Speed();                            // 得到网速
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
// TCP测试程序
void tcp_server_test_app(void)
{ 
	err_t err;
	struct tcp_pcb *tcppcbnew;  	// 定义一个TCP服务器控制块
	struct tcp_pcb *tcppcbconn;  	// 定义一个TCP服务器控制块

	uint8_t *tbuf;
	uint8_t key;
	uint8_t res = 0;
    // 带发送的数据
    char str[] = "123456789";
    // 填充要发送的数据缓冲区
    tcp_server_sendbuf = str;
    
	LCD_Clear(WHITE);	    // 清屏
	POINT_COLOR = RED; 	    // 红色字体

	tbuf = mymalloc(SRAMIN, 200);	// 申请内存
	if (tbuf == NULL)
        return ;		    // 内存申请失败了,直接退出
	sprintf((char*)tbuf, "Server IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]); // 服务器IP
	LCD_ShowString(30, 130, 210, 16, 16, tbuf);
	sprintf((char*)tbuf, "Server Port:%d", TCP_SERVER_PORT);        // 服务器端口号
	LCD_ShowString(30, 150, 210, 16, 16, tbuf);
	tcppcbnew = tcp_new();	// 创建一个新的pcb
	if (tcppcbnew)			// 创建成功                                                      
	{
		err = tcp_bind(tcppcbnew, IP_ADDR_ANY, TCP_SERVER_PORT);    // 将本地IP与指定的端口号绑定在一起,IP_ADDR_ANY为绑定本地所有的IP地址
		if (err == ERR_OK)	// 绑定完成
		{
			tcppcbconn = tcp_listen(tcppcbnew); 	    // 设置tcppcb进入监听状态
			tcp_accept(tcppcbconn, tcp_server_accept); 	// 初始化LWIP的tcp_accept的回调函数
		}
        else res = 1;
	} 
    else res = 1;
	POINT_COLOR = BLUE;     // 蓝色字体
	while (res == 0)
	{
		key = key_scan(0);
		if (key == WKUP_PRES)
        {
            break;
        }
		if (key == KEY0_PRES)               // KEY0按下了,发送数据
		{
			tcp_server_flag |= 1 << 7;      // 标记要发送数据
		}
		if (tcp_server_flag & 1 << 6)       // 是否收到数据?
		{
			LCD_Fill(30, 210, lcddev.width - 1, lcddev.height - 1, WHITE); // 清上一次数据
			LCD_ShowString(30, 210, lcddev.width - 30, lcddev.height - 210, 16, tcp_server_recvbuf); // 显示接收到的数据
			tcp_server_flag &= ~(1 << 6);   // 标记数据已经被处理了.
		}
		if (tcp_server_flag & 1 << 5)       // 是否连接上?
		{
			sprintf((char*)tbuf, "Client IP:%d.%d.%d.%d", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]); //客户端IP
			LCD_ShowString(30, 170, 230, 16, 16, tbuf);
			POINT_COLOR = RED;
			LCD_ShowString(30, 190, lcddev.width - 30, lcddev.height - 190, 16, "Receive Data:");   // 提示消息
			POINT_COLOR = BLUE;         // 蓝色字体
		} 
        else if ((tcp_server_flag & 1 << 5) == 0)
		{
			LCD_Fill(30, 170, lcddev.width - 1, lcddev.height - 1, WHITE); // 清屏
		}
		lwip_periodic_handle();
	}
	tcp_server_connection_close(tcppcbnew, 0);  // 关闭TCP Server连接
	tcp_server_connection_close(tcppcbconn, 0); // 关闭TCP Server连接
	tcp_server_remove_timewait();
	memset(tcppcbnew, 0, sizeof(struct tcp_pcb));
	memset(tcppcbconn, 0, sizeof(struct tcp_pcb));
	myfree(SRAMIN, tbuf);
    
	LCD_Clear(WHITE);
	POINT_COLOR = BLUE;
	LCD_ShowString(30, 90, 200, 16, 16, "Connect break！");
    printf("Connect break!\r\n");
	LCD_ShowString(30, 110, 200, 16, 16, "KEY1:Connect");
}


// UDP测试程序
void udp_test_app(void)
{
	err_t err;
	struct udp_pcb * udppcb;    // 定义一个UDP服务器控制块
	struct ip_addr rmtipaddr;  	// 远端ip地址

	uint8_t *tbuf;
	uint8_t res = 0;
	uint8_t key = 0;
	uint8_t *s_buf = (uint8_t *)"UDP测试实验!";
    
    LCD_Clear(WHITE);	    // 清屏
	POINT_COLOR = RED; 	    // 红色字体

	// 远端IP 192.168.1.125，前面几位通过DHCP获取，或者通过初始化时设置的静态IP：192.168.1.30
	udp_set_remoteip(125);
	// 申请内存
	tbuf = mymalloc(SRAMIN, 200);
	if (NULL == tbuf)
	{
		return;
	}
	sprintf((char*)tbuf, "Local IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);						   // 服务器IP
	printf("Local IP:%d.%d.%d.%d\r\n", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]);
	LCD_ShowString(30, 150, 210, 16, 16, tbuf);

	sprintf((char*)tbuf, "Remote IP:%d.%d.%d.%d", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]); // 远端IP
	printf("Remote IP:%d.%d.%d.%d\r\n", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);
	LCD_ShowString(30, 170, 210, 16, 16, tbuf);

	sprintf((char*)tbuf, "Remote Port:%d", UDP_DEMO_PORT);		            // 客户端端口号
	printf("Remote Port:%d\r\n", UDP_DEMO_PORT);
	LCD_ShowString(30, 190, 210, 16, 16, tbuf);

	POINT_COLOR = BLUE;
	LCD_ShowString(30, 210, 210, 16, 16, "STATUS:Disconnected\r\n");

	// 创建一个UDP控制块
	udppcb = udp_new();

	if (udppcb)
	{
		IP4_ADDR(&rmtipaddr, lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);
		err = udp_connect(udppcb, &rmtipaddr, UDP_DEMO_PORT); 				 // UDP客户端连接到指定IP地址和端口号的服务器
		if (err == ERR_OK)
		{
			err = udp_bind(udppcb, IP_ADDR_ANY, UDP_DEMO_PORT); 			 // 绑定本地IP地址与端口号
			if (err == ERR_OK)												 // 绑定完成
			{
				udp_recv(udppcb, udp_callback_fun, NULL); 					 // 注册接收回调函数
				LCD_ShowString(30, 210, 210, 16, 16, "STATUS:Connected   "); // 标记连接上了(UDP是非可靠连接,这里仅仅表示本地UDP已经准备好)
				printf("STATUS:Connected\r\n");
				udp_demo_flag |= 1 << 5;									 // 标记已经连接上
				POINT_COLOR = RED;
				LCD_ShowString(30, 230, lcddev.width - 30, lcddev.height - 190, 16, "Receive Data:"); // 提示消息
				POINT_COLOR = BLUE;                                          // 蓝色字体
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
		if (key == KEY0_PRES) 		// KEY0按下了,发送数据
		{
			udp_send_data(udppcb, s_buf);
		}
		if (udp_demo_flag & 1 << 6) // 是否收到数据
		{
			LCD_Fill(30, 250, lcddev.width - 1, lcddev.height - 1, WHITE);  	// 清上一次数据
			LCD_ShowString(30, 250, lcddev.width - 30, lcddev.height - 230, 16, udp_demo_recvbuf); // 显示接收到的数据
			printf("%s\n", udp_demo_recvbuf);
			udp_demo_flag &= ~(1 << 6);										    // 标记数据已经被处理了.
		}
		lwip_periodic_handle();
		delay_ms(2);
	}
	LCD_Clear(WHITE);
	udp_connection_close(udppcb);
	myfree(SRAMIN, tbuf);
}

// TCP Client 测试
void tcp_client_test_app(void)
{
	struct tcp_pcb *tcppcb;  	// 定义一个TCP服务器控制块
	struct ip_addr rmtipaddr;  	// 远端ip地址

	uint8_t *tbuf;
	uint8_t key;
	uint8_t res = 0;
	uint8_t t = 0;
    
    // 带发送的数据
    uint8_t str[] = "123456789";
    // 填充要发送的数据缓冲区
    tcp_client_sendbuf = str;
    
	// 设置远端服务器IP，前三位由DHCP获取，192.168.1.125
	tcp_client_set_remoteip(125);

	LCD_Clear(WHITE);	// 清屏
	POINT_COLOR = RED; 	// 红色字体

	tbuf = mymalloc(SRAMIN, 200);	// 申请内存
	if (tbuf == NULL)return ;		// 内存申请失败了,直接退出
	sprintf((char*)tbuf, "Local IP:%d.%d.%d.%d", lwipdev.ip[0], lwipdev.ip[1], lwipdev.ip[2], lwipdev.ip[3]); // 服务器IP
	LCD_ShowString(30, 150, 210, 16, 16, tbuf);
	sprintf((char*)tbuf, "Remote IP:%d.%d.%d.%d", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]); //远端IP
	LCD_ShowString(30, 170, 210, 16, 16, tbuf);
	sprintf((char*)tbuf, "Remotewo Port:%d", TCP_CLIENT_PORT); // 客户端端口号
	LCD_ShowString(30, 190, 210, 16, 16, tbuf);
	POINT_COLOR = BLUE;
	LCD_ShowString(30, 210, 210, 16, 16, "STATUS:Disconnected");
	tcppcb = tcp_new();	// 创建一个新的pcb
	if (tcppcb)			// 创建成功
	{
		IP4_ADDR(&rmtipaddr, lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);
		tcp_connect(tcppcb, &rmtipaddr, TCP_CLIENT_PORT, tcp_client_connected); // 连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
	} else res = 1;
	while (res == 0)
	{
		key = key_scan(0);
		if (key == WKUP_PRES)break;
		if (key == KEY0_PRES)               // KEY0按下了,发送数据
		{
			tcp_client_flag |= 1 << 7;      // 标记要发送数据
		}
		if (tcp_client_flag & 1 << 6)       // 是否收到数据?
		{
			LCD_Fill(30, 250, lcddev.width - 1, lcddev.height - 1, WHITE); // 清上一次数据
			LCD_ShowString(30, 250, lcddev.width - 30, lcddev.height - 230, 16, tcp_client_recvbuf);    // 显示接收到的数据
			tcp_client_flag &= ~(1 << 6);   // 标记数据已经被处理了.
		}
		if (tcp_client_flag & 1 << 5)       // 是否连接上?
		{
			LCD_ShowString(30, 210, lcddev.width - 30, lcddev.height - 190, 16, "STATUS:Connected   "); // 提示消息
			POINT_COLOR = RED;
			LCD_ShowString(30, 230, lcddev.width - 30, lcddev.height - 190, 16, "Receive Data:");       // 提示消息
			POINT_COLOR = BLUE;             // 蓝色字体
		} 
        else if ((tcp_client_flag & 1 << 5) == 0)
		{
			LCD_ShowString(30, 210, 190, 16, 16, "STATUS:Disconnected");
			LCD_Fill(30, 230, lcddev.width - 1, lcddev.height - 1, WHITE); //清屏
		}
		lwip_periodic_handle();
		delay_ms(2);
		t++;
		if (t == 200)
		{
			if ((tcp_client_flag & 1 << 5) == 0)        // 未连接上,则尝试重连
			{
				tcp_client_connection_close(tcppcb, 0); // 关闭连接
				tcppcb = tcp_new();	                    // 创建一个新的pcb
				if (tcppcb)			                    // 创建成功
				{
					tcp_connect(tcppcb, &rmtipaddr, TCP_CLIENT_PORT, tcp_client_connected); // 连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
				}
			}
			t = 0;
            LED1 = !LED1;
		}
	}
	tcp_client_connection_close(tcppcb, 0); // 关闭TCP Client连接
	LCD_Clear(WHITE);
	POINT_COLOR = BLUE;
	LCD_ShowString(30, 90, 200, 16, 16, "Connect break！");
	LCD_ShowString(30, 110, 200, 16, 16, "KEY1:Connect");
    printf("Connect break！\r\n");
	myfree(SRAMIN, tbuf);
}

