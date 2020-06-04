#ifndef  __BSP_ESP8266_H__
#define	 __BSP_ESP8266_H__

#include <bsp.h>

/*
TCP/IP���������ATָ��
		����   					����  
		AT+ CIPSTATUS   		�������״̬  
		AT+CIPSTART   			���� TCP ���ӻ�ע�� UDP �˿ں�  
		AT+CIPSEND   			��������  
		AT+CIPCLOSE   			�ر� TCP �� UDP  
		AT+CIFSR   				��ȡ���� IP ��ַ  
		AT+CIPMUX   			����������  
		AT+CIPSERVER   			����Ϊ������  
		AT+CIPMODE   			����ģ�鴫��ģʽ  
		AT+CIPSTO   			���÷�������ʱʱ��  
		AT+CIUPDATE   			���������̼�  
		AT+PING   				PING ���� 
*/

// ����WIFI��غ궨��
#define   macUser_ESP8266_ApSsid                  "DGUT-8B110-2.4G"      // Ҫ���ӵ��ȵ������
#define   macUser_ESP8266_ApPwd                   "DGUT8B110"            // Ҫ���ӵ��ȵ����Կ
											      
#define   macUser_ESP8266_BulitApSsid             "veis"     	 		 // Ҫ�������ȵ������
#define   macUser_ESP8266_BulitApEcn              OPEN               	 // Ҫ�������ȵ�ļ��ܷ�ʽ
#define   macUser_ESP8266_BulitApPwd              "12345678"         	 // Ҫ�������ȵ����Կ
											      
#define   macUser_ESP8266_TcpServer_IP            "192.168.23.169"       // ������������IP��ַ
#define   macUser_ESP8266_TcpServer_Port          "8080"             	 // �����������Ķ˿�   
#define   macUser_ESP8266_TcpServer_OverTime      "1800"             	 // ��������ʱʱ�䣨��λ���룩


// ���������__CCARM�������������ṹ�����ϵ�֧��
#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/******************************* ESP8266����������Ͷ��� ***************************/
// ESP8266 ������ʽö��
typedef enum{
	STA,		// Station�������ն�ģʽ����һ������������ն˶�
	AP,			// SoftAP�����߽���㣬��һ��������������Ľڵ�
	STA_AP  	// ��������ģʽ����
} ENUM_Net_ModeTypeDef;

// ESP8266 TCP/UDP����Э��ö��
typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;

// ESP8266 ������/������ID��ö��
typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;

// ESP8266 ���ܷ�ʽö��
typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;

//�����ջ����ֽ���
#define RX_BUF_MAX_LEN     1024

// ��������֡�Ĵ���ṹ��
typedef struct                                    					
{
	char  Data_RX_BUF[RX_BUF_MAX_LEN];
	union {
			__IO uint16_t InfAll;
	struct {
			// λ�ζ���
			__IO uint16_t FramLength       :15;     // 14:0 
			__IO uint16_t FramFinishFlag   :1;      // 15 
	  } InfBit;
  }; 	
} STRUCT_USARTx_Fram;

/* �궨�� --------------------------------------------------------------------*/

/******************************** ESP8266 �������Ŷ��� ***********************************/
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
             
/********************************* ESP8266 �����궨�� *******************************************/
#define ESP8266_Usart( fmt, ... )                      USART_printf (ESP8266_USARTx, fmt, ##__VA_ARGS__ ) 
#define PC_Usart( fmt, ... )                           printf( fmt, ##__VA_ARGS__ )


// �ⲿ����
extern STRUCT_USARTx_Fram strEsp8266_Fram_Record;
extern uint8_t esp8266_rxdata;

/* �ⲿ���ú������� ------------------------------------------------------------------*/
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
// �Զ���ʵ��printf����
void    USART_printf(USART_TypeDef * USARTx_, char * Data, ... );

#endif

