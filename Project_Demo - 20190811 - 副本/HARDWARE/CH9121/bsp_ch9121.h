// ������������CH9121ͨ�ŵĻ������ݽṹ�������ýṹ��������
#ifndef __BSP_CH9121_H__
#define __BSP_CH9121_H__

#include <bsp.h>

// ��������Ĭ��ģ����Ϊ�ͻ���
#define M_UARTX				UART4

#define WRITE_CMD           {Usartx_SendByte(M_UARTX, 0x57);Usartx_SendByte(M_UARTX, 0xab);}


// ������պͷ��ͻ��������ö�̬�����С���������С��1024byte
extern uint8_t net_tx_buf[1024];
extern uint8_t net_rx_buf[1024];
extern uint8_t overflow_flag;
// ��������״̬����
typedef enum _ConnectStatus
{
	disconnect = 0,
	connect_ok
} ConnectStatus;

// У�鷽ʽ��������
typedef enum _Serial_Check
{
	Even_Check = 0,
	Odd_Check,
	Mark,
	Space,
	None
} Serial_Check;

// ����ģʽ����
typedef enum _Net_Mode
{
	TCP_Server_mode = 0,
	TCP_Client_mode,
	UDP_Server_mode,
	UDP_Client_mode
} Net_Mode;

// ������/�ͻ���ͨ����ص�����
typedef struct _Net_CommunitionType
{
	uint16_t port_num;				// �˿ں�
	uint8_t ip_address[4];			// IP��ַ
	uint8_t subnet_mask[4];			// ��������
	uint8_t gateway[4];				// Ĭ������
    uint16_t tcp_retry_count;		// TCP���Դ���
	uint8_t mac_address[4];			// MAC��ַ
	ConnectStatus status;			// ����״̬
    Net_Mode net_mode;
} Net_CommunitionType;
extern Net_CommunitionType net_communition_Client;
extern Net_CommunitionType net_communition_DstServer;
extern Net_CommunitionType net_communition_Server1;

// ���������������
typedef struct _Serial
{
	uint32_t bound;					// ������
	Serial_Check serial_check;		// У������
	uint8_t data_bit;				// ����λ��
	uint8_t stop_bit;				// ֹͣλ
	uint32_t timeout;				// ��ʱʱ��
} Serial;
extern Serial serial;


// д�����룬��ʽ��0x57 0xab + ������ + ���ݣ�
#define RESET_CHIP				0x02		// ��λ���оƬ�������� 
#define UPDATE_CONFIG			0x0d		// �������ò����� EEPROM 
#define CMD_EXECUTION			0x0e		// ����ִ��
#define SET_CHIP_MODE			0x10		// ����ģʽ
#define SET_CHIP_IP				0x11		// ����оƬ IP ��ַ
#define SET_CHIP_SUBNET_MASK	0x12		// ����оƬ���� 
#define SET_CHIP_GATEWAY		0x13		// ����оƬ���� 
#define SET_LOCAL_PORT			0x14		// ����оƬ���ض˿� 
#define SET_DESTINATION_IP		0x15		// ����оƬĿ�� IP ��ַ
#define SET_DESTINATION_PORT	0x16		// ����оƬĿ�Ķ˿�
#define SET_SERIAL_BOUND		0x21		// ���ô��ڲ����� 
#define SET_SERIAL_DATA_FORMAT	0x22		// ���ô���У��λ����λֹͣλ
#define EXIT_SERIAL_CONFIG		0x5e		// �˳���������ģʽ

// �������룬��ʽ��0x57 0xab + �����룩
#define READ_CHIP_MODE			0x60		// ��ȡоƬ����ģʽ������ 1 �ֽ� 
#define READ_CHIP_IP			0x61		// ��ȡоƬ IP ��ַ������ 4 �ֽ� 
#define READ_CHIP_SUBNET_MASK	0x62		// ��ȡоƬ���룬���� 4 �ֽ� 
#define READ_CHIP_GATEWAY		0X63		// ��ȡоƬ���أ����� 4 �ֽ� 
#define READ_SOURCE_PORT	    0x64		// ��ȡоƬԴ�˿ںţ����� 2 �ֽ�
#define READ_DESTINATION_IP 	0x65		// ��ȡоƬĿ�� IP ��ַ������ 4 �ֽ�
#define READ_DESTINATION_PORT	0x66		// ��ȡоƬĿ�Ķ˿ںţ����� 2 �ֽ� 
#define READ_RETRY_TIMES		0x67		// ��ȡ TCP ���Դ��������� 1 �ֽ� 
#define READ_SERIAL_BOUND		0x71		// ��ȡ���ڲ����ʣ����� 4 �ֽ� 
#define READ_SERIAL_DATA_FORMAT 0x72		// ��ȡ����У��λ����λֹͣλ������ 3 �ֽ�
#define READ_SERIAL_OVERTIME	0x73		// ��ȡ���ڳ�ʱʱ�䣬���� 1 �ֽ� 
#define READ_MAC_ADDRESS		0x81		// ��ȡ MAC ��ַ������ 6 �ֽ� 
#define READ_TCP_STATUS			0x03		// ��ȡ TCP ����״̬��TCP CLIENT ģʽ�£������� 1 �ֽڣ�1�����ӣ�0���Ͽ��� 


// ���к�������
void init_ch9121(void);
void send_netdata(uint8_t * t_buf);
uint8_t read_WorkMode(void);
uint16_t read_source_port(void);
void read_chip_ip(uint8_t *ip_addr);
uint16_t read_distination_port(void);
void read_distination_ip(uint8_t *ip_addr);
void reset_chip(void);
#endif	
