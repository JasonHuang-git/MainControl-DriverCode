#ifndef __UDP_DEMO_H__
#define __UDP_DEMO_H__

#include <bsp.h>
#include <lwip_comm.h>
#include <lwip/pbuf.h>
#include <lwip/udp.h>
#include <lwip/tcp.h>

#define UDP_DEMO_RX_BUFSIZE		2048	// 定义udp最大接收数据长度 
#define UDP_DEMO_PORT			8089	// 定义udp连接的端口 

// 声明外部变量
extern uint8_t udp_demo_flag;
extern uint8_t udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];

void udp_callback_fun(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,uint16_t port);
void udp_send_data(struct udp_pcb *upcb, unsigned char *udp_sendbuf);
void udp_set_remoteip(uint8_t id_num);
void udp_connection_close(struct udp_pcb *upcb);

#endif
