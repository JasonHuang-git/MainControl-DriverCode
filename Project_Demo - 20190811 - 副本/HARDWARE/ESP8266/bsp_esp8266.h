#ifndef  __BSP_ESP8266_H__
#define	 __BSP_ESP8266_H__

#include <bsp.h>

/*
TCP/IP工具箱相关AT指令
		命令   					描述  
		AT+ CIPSTATUS   		获得连接状态  
		AT+CIPSTART   			建立 TCP 连接或注册 UDP 端口号  
		AT+CIPSEND   			发送数据  
		AT+CIPCLOSE   			关闭 TCP 或 UDP  
		AT+CIFSR   				获取本地 IP 地址  
		AT+CIPMUX   			启动多连接  
		AT+CIPSERVER   			配置为服务器  
		AT+CIPMODE   			设置模块传输模式  
		AT+CIPSTO   			设置服务器超时时间  
		AT+CIUPDATE   			网络升级固件  
		AT+PING   				PING 命令 
*/

// 测试WIFI相关宏定义
#define   macUser_ESP8266_ApSsid                  "DGUT-8B110-2.4G"      // 要连接的热点的名称
#define   macUser_ESP8266_ApPwd                   "DGUT8B110"            // 要连接的热点的密钥
											      
#define   macUser_ESP8266_BulitApSsid             "veis"     	 		 // 要建立的热点的名称
#define   macUser_ESP8266_BulitApEcn              OPEN               	 // 要建立的热点的加密方式
#define   macUser_ESP8266_BulitApPwd              "12345678"         	 // 要建立的热点的密钥
											      
#define   macUser_ESP8266_TcpServer_IP            "192.168.23.169"       // 服务器开启的IP地址
#define   macUser_ESP8266_TcpServer_Port          "8080"             	 // 服务器开启的端口   
#define   macUser_ESP8266_TcpServer_OverTime      "1800"             	 // 服务器超时时间（单位：秒）


// 如果定义了__CCARM，启动对匿名结构和联合的支持
#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/******************************* ESP8266相关数据类型定义 ***************************/
// ESP8266 组网方式枚举
typedef enum{
	STA,		// Station：无线终端模式，是一个无线网络的终端端
	AP,			// SoftAP：无线接入点，是一个无线网络的中心节点
	STA_AP  	// 上述两种模式共存
} ENUM_Net_ModeTypeDef;

// ESP8266 TCP/UDP网络协议枚举
typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;

// ESP8266 单连接/多连接ID号枚举
typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;

// ESP8266 加密方式枚举
typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;

//最大接收缓存字节数
#define RX_BUF_MAX_LEN     1024

// 串口数据帧的处理结构体
typedef struct                                    					
{
	char  Data_RX_BUF[RX_BUF_MAX_LEN];
	union {
			__IO uint16_t InfAll;
	struct {
			// 位段定义
			__IO uint16_t FramLength       :15;     // 14:0 
			__IO uint16_t FramFinishFlag   :1;      // 15 
	  } InfBit;
  }; 	
} STRUCT_USARTx_Fram;

/* 宏定义 --------------------------------------------------------------------*/

/******************************** ESP8266 连接引脚定义 ***********************************/
#define ESP8266_USARTx                                 USART3
#define ESP8266_USARTx_BAUDRATE                        115200
#define ESP8266_USART_RCC_CLK						   RCC_APB1Periph_USART3                 
#define ESP8266_AF_USARTx							   GPIO_AF_USART3

#define ESP8266_USARTx_GPIO_ClK                        RCC_AHB1Periph_GPIOB
#define ESP8266_USARTx_Port                            GPIOB
#define ESP8266_USARTx_Tx_PIN                          GPIO_Pin_10
#define ESP8266_USARTx_Rx_PIN                          GPIO_Pin_11
#define ESP8266_TXD_SOURCE							   GPIO_PinSource10
#define ESP8266_RXD_SOURCE							   GPIO_PinSource11
					
#define ESP8266_USARTx_IRQHANDLER                      USART3_IRQHandler
#define ESP8266_USARTx_IRQn                            USART3_IRQn

#define ESP8266_RST_GPIO_ClK                  		   RCC_AHB1Periph_GPIOD
#define ESP8266_RST_PORT                               GPIOD
#define ESP8266_RST_PIN                                GPIO_Pin_2
#define ESP8266_RST_HIGH()                             GPIO_WriteBit(ESP8266_RST_PORT,ESP8266_RST_PIN,Bit_SET)
#define ESP8266_RST_LOW()                              GPIO_WriteBit(ESP8266_RST_PORT,ESP8266_RST_PIN,Bit_RESET)
             
/********************************* ESP8266 函数宏定义 *******************************************/
#define ESP8266_Usart( fmt, ... )                      USART_printf (ESP8266_USARTx, fmt, ##__VA_ARGS__ ) 
#define PC_Usart( fmt, ... )                           printf( fmt, ##__VA_ARGS__ )


// 外部变量
extern STRUCT_USARTx_Fram strEsp8266_Fram_Record;
extern uint8_t esp8266_rxdata;

/* 外部调用函数声明 ------------------------------------------------------------------*/
void    ESP8266_Init(void);
void    ESP8266_stop(void);
void    ESP8266_Rst(void);
bool    ESP8266_Cmd(char * cmd, char * reply1, char * reply2, uint32_t waittime);
bool    ESP8266_AT_Test(void);
bool    ESP8266_Net_Mode_Choose( ENUM_Net_ModeTypeDef enumMode);
bool    ESP8266_JoinAP(char * pSSID, char * pPassWord);
bool    ESP8266_BuildAP(char * pSSID, char * pPassWord, ENUM_AP_PsdMode_TypeDef enunPsdMode);
bool    ESP8266_Enable_MultipleId( FunctionalState enumEnUnvarnishTx);
bool    ESP8266_Link_Server(ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool    ESP8266_StartOrShutServer(FunctionalState enumMode, char * pPortNum, char * pTimeOver);
uint8_t ESP8266_Get_LinkStatus(void);
uint8_t ESP8266_Get_IdLinkStatus(void);
uint8_t ESP8266_Inquire_ApIp(char * pApIp, uint8_t ucArrayLength);
uint8_t ESP8266_CIPAP(char * pApIp);
bool    ESP8266_UnvarnishSend(void);
void    ESP8266_ExitUnvarnishSend(void);
bool    ESP8266_SendString(FunctionalState enumEnUnvarnishTx, char * pStr, uint32_t ulStrLength, ENUM_ID_NO_TypeDef ucId);
char *  ESP8266_ReceiveString(FunctionalState enumEnUnvarnishTx);
void 	ESP8266_Clear_Buffer(void);
// 自定义实现printf函数
void    USART_printf(USART_TypeDef * USARTx_, char * Data, ... );

#endif

