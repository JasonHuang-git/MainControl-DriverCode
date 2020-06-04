// 定义了与网络CH9121通信的基本数据结构，和配置结构，参数等
#ifndef __BSP_CH9121_H__
#define __BSP_CH9121_H__

#include <bsp.h>

// 本程序中默认模块做为客户端
#define M_UARTX				UART4

#define WRITE_CMD           {Usartx_SendByte(M_UARTX, 0x57);Usartx_SendByte(M_UARTX, 0xab);}


// 定义接收和发送缓冲区，用动态申请大小，打包长度小于1024byte
extern uint8_t net_tx_buf[1024];
extern uint8_t net_rx_buf[1024];
extern uint8_t overflow_flag;
// 网络连接状态类型
typedef enum _ConnectStatus
{
	disconnect = 0,
	connect_ok
} ConnectStatus;

// 校验方式数据类型
typedef enum _Serial_Check
{
	Even_Check = 0,
	Odd_Check,
	Mark,
	Space,
	None
} Serial_Check;

// 工作模式类型
typedef enum _Net_Mode
{
	TCP_Server_mode = 0,
	TCP_Client_mode,
	UDP_Server_mode,
	UDP_Client_mode
} Net_Mode;

// 服务器/客户端通信相关的类型
typedef struct _Net_CommunitionType
{
	uint16_t port_num;				// 端口号
	uint8_t ip_address[4];			// IP地址
	uint8_t subnet_mask[4];			// 子网掩码
	uint8_t gateway[4];				// 默认网关
    uint16_t tcp_retry_count;		// TCP重试次数
	uint8_t mac_address[4];			// MAC地址
	ConnectStatus status;			// 连接状态
    Net_Mode net_mode;
} Net_CommunitionType;
extern Net_CommunitionType net_communition_Client;
extern Net_CommunitionType net_communition_DstServer;
extern Net_CommunitionType net_communition_Server1;

// 串口相关数据类型
typedef struct _Serial
{
	uint32_t bound;					// 波特率
	Serial_Check serial_check;		// 校验类型
	uint8_t data_bit;				// 数据位数
	uint8_t stop_bit;				// 停止位
	uint32_t timeout;				// 超时时间
} Serial;
extern Serial serial;


// 写命令码，格式（0x57 0xab + 命令码 + 数据）
#define RESET_CHIP				0x02		// 复位命令，芯片重新运行 
#define UPDATE_CONFIG			0x0d		// 更新配置参数至 EEPROM 
#define CMD_EXECUTION			0x0e		// 命令执行
#define SET_CHIP_MODE			0x10		// 设置模式
#define SET_CHIP_IP				0x11		// 设置芯片 IP 地址
#define SET_CHIP_SUBNET_MASK	0x12		// 设置芯片掩码 
#define SET_CHIP_GATEWAY		0x13		// 设置芯片网关 
#define SET_LOCAL_PORT			0x14		// 设置芯片本地端口 
#define SET_DESTINATION_IP		0x15		// 设置芯片目的 IP 地址
#define SET_DESTINATION_PORT	0x16		// 设置芯片目的端口
#define SET_SERIAL_BOUND		0x21		// 设置串口波特率 
#define SET_SERIAL_DATA_FORMAT	0x22		// 设置串口校验位数据位停止位
#define EXIT_SERIAL_CONFIG		0x5e		// 退出串口配置模式

// 读命令码，格式（0x57 0xab + 命令码）
#define READ_CHIP_MODE			0x60		// 读取芯片工作模式，返回 1 字节 
#define READ_CHIP_IP			0x61		// 读取芯片 IP 地址，返回 4 字节 
#define READ_CHIP_SUBNET_MASK	0x62		// 读取芯片掩码，返回 4 字节 
#define READ_CHIP_GATEWAY		0X63		// 读取芯片网关，返回 4 字节 
#define READ_SOURCE_PORT	    0x64		// 读取芯片源端口号，返回 2 字节
#define READ_DESTINATION_IP 	0x65		// 读取芯片目的 IP 地址，返回 4 字节
#define READ_DESTINATION_PORT	0x66		// 读取芯片目的端口号，返回 2 字节 
#define READ_RETRY_TIMES		0x67		// 读取 TCP 重试次数，返回 1 字节 
#define READ_SERIAL_BOUND		0x71		// 读取串口波特率，返回 4 字节 
#define READ_SERIAL_DATA_FORMAT 0x72		// 读取串口校验位数据位停止位，返回 3 字节
#define READ_SERIAL_OVERTIME	0x73		// 读取串口超时时间，返回 1 字节 
#define READ_MAC_ADDRESS		0x81		// 读取 MAC 地址，返回 6 字节 
#define READ_TCP_STATUS			0x03		// 读取 TCP 连接状态（TCP CLIENT 模式下），返回 1 字节，1：连接，0：断开。 


// 公有函数区域
void init_ch9121(void);
void send_netdata(uint8_t * t_buf);
uint8_t read_WorkMode(void);
uint16_t read_source_port(void);
void read_chip_ip(uint8_t *ip_addr);
uint16_t read_distination_port(void);
void read_distination_ip(uint8_t *ip_addr);
void reset_chip(void);
#endif	
