#include <udp_demo.h>

// UDP�������ݻ�����
uint8_t udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];  // UDP�������ݻ�����

// UDP ����ȫ��״̬��Ǳ���
// bit7:û���õ�
// bit6:0,û���յ�����;1,�յ�������.
// bit5:0,û��������;1,��������.
// bit4~0:����
uint8_t udp_demo_flag = 0;


// UDP�ص�����
void udp_callback_fun(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, uint16_t port)
{
    uint32_t data_len = 0;
    struct pbuf *q;
    if (NULL != p)
    {
        memset(udp_demo_recvbuf, 0, UDP_DEMO_RX_BUFSIZE);   // ������ݻ�����
        // ����������pbuf����
        for (q = p; q != NULL; q = q->next)
        {
            /*
                �ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬
                ������ڵĻ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е����ݡ�
            */
            if (q->len > (UDP_DEMO_RX_BUFSIZE - data_len)) 
            {
                memcpy(udp_demo_recvbuf + data_len, q->payload, (UDP_DEMO_RX_BUFSIZE - data_len)); // ��������
            }
            else 
            {
                memcpy(udp_demo_recvbuf + data_len, q->payload, q->len);
            }
            data_len += q->len;
            if (data_len > UDP_DEMO_RX_BUFSIZE)   // ����TCP�ͻ��˽�������,����
            {
                break;                                         
            }
        }
        upcb->remote_ip = *addr;                                    // ��¼Զ��������IP��ַ
        upcb->remote_port = port;                                   // ��¼Զ�������Ķ˿ں�
        lwipdev.remoteip[0] = upcb->remote_ip.addr & 0xff;          // IADDR4
        lwipdev.remoteip[1] = (upcb->remote_ip.addr >> 8) & 0xff;   // IADDR3
        lwipdev.remoteip[2] = (upcb->remote_ip.addr >> 16) & 0xff;  // IADDR2
        lwipdev.remoteip[3] = (upcb->remote_ip.addr >> 24) & 0xff;  // IADDR1
        udp_demo_flag |= 1 << 6;                                    // ��ǽ��յ�������
        pbuf_free(p);                                               // �ͷ��ڴ�
    }
    else
    {
        printf("��Զ������%d.%d.%d.%d�Ͽ�����\r\n", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);
        udp_disconnect(upcb);
        udp_demo_flag &= ~(1<<5);   // ������ӶϿ�
    }
}

// UDP���������ͺ���
// ������upcb  ->  UDP ���̿��ƿ�
//       udp_sendbuf  ->  �����͵����ݻ�����
void udp_send_data(struct udp_pcb *upcb, unsigned char *udp_sendbuf)
{
    struct pbuf *ptr;
    ptr = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)udp_sendbuf), PBUF_POOL);    // �����ڴ�
    if (ptr)
    {
        pbuf_take(ptr, (char*)udp_sendbuf, strlen((char*)udp_sendbuf)); // ��sendbuf�е����ݴ����pbuf�ṹ��
        udp_send(upcb, ptr);     // udp��������
        pbuf_free(ptr);          // �ͷ��ڴ�
    }
}

// ����Զ��IP��ַ
void udp_set_remoteip(uint8_t id_num)
{
    uint8_t *tbuf;

    tbuf = mymalloc(SRAMIN, 100);   // �����ڴ�
    if (tbuf == NULL)
        return;
    // ǰ����IP���ֺ�DHCP�õ���IPһ��
    lwipdev.remoteip[0] = lwipdev.ip[0];
    lwipdev.remoteip[1] = lwipdev.ip[1];
    lwipdev.remoteip[2] = lwipdev.ip[2];
    lwipdev.remoteip[3] = id_num;
    sprintf((char*)tbuf, "Remote IP:%d.%d.%d.%d", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]); // Զ��IP
    printf("Զ��IP��ַ��%d.%d.%d.%d\r\n", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);

    myfree(SRAMIN, tbuf);
}

// �ر�UDP����
void udp_connection_close(struct udp_pcb *upcb)
{
    udp_disconnect(upcb);
    udp_remove(upcb);            // ��һ�����̿��ƿ��������ɾ��
    udp_demo_flag &= ~(1 << 5);  // ������ӶϿ�
    printf("UDP�Ͽ����ӳɹ���");
}
