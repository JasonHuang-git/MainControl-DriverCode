#ifndef __TCP_SERVER_DEMO_H
#define __TCP_SERVER_DEMO_H

#include <bsp.h>
#include <lwip/debug.h>
#include <lwip/stats.h>
#include <lwip/tcp.h>
#include <lwip/memp.h>
#include <lwip/mem.h>
#include <lwip_comm.h>

#define TCP_SERVER_RX_BUFSIZE	200  // ����tcp server���������ݳ���
#define TCP_SERVER_PORT			8080 // ����tcp server�Ķ˿�

// �ⲿ����
extern uint8_t tcp_server_flag;
extern uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];
extern char *tcp_server_sendbuf;

// tcp����������״̬
enum tcp_server_states
{
	ES_TCPSERVER_NONE = 0,		     // û������
	ES_TCPSERVER_ACCEPTED,		     // �пͻ����������� 
	ES_TCPSERVER_CLOSING,		     // �����ر�����
};   
     
// LWIP�ص�����ʹ�õĽṹ��    
struct tcp_server_struct     
{    
	u8 state;                        // ��ǰ����״
	struct tcp_pcb *pcb;             // ָ��ǰ��pcb
	struct pbuf *p;                  // ָ�����/�����pbuf
}; 

err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err);
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void tcp_server_error(void *arg,err_t err);
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void tcp_server_remove_timewait(void);

#endif 
