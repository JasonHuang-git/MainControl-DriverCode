#include <udp_demo.h>

// UDP接收数据缓冲区
uint8_t udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];  // UDP接收数据缓冲区

// UDP 测试全局状态标记变量
// bit7:没有用到
// bit6:0,没有收到数据;1,收到数据了.
// bit5:0,没有连接上;1,连接上了.
// bit4~0:保留
uint8_t udp_demo_flag = 0;


// UDP回调函数
void udp_callback_fun(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, uint16_t port)
{
    uint32_t data_len = 0;
    struct pbuf *q;
    if (NULL != p)
    {
        memset(udp_demo_recvbuf, 0, UDP_DEMO_RX_BUFSIZE);   // 清空数据缓冲区
        // 遍历完整个pbuf链表
        for (q = p; q != NULL; q = q->next)
        {
            /*
                判断要拷贝到UDP_DEMO_RX_BUFSIZE中的数据是否大于UDP_DEMO_RX_BUFSIZE的剩余空间，
                如果大于的话就只拷贝UDP_DEMO_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据。
            */
            if (q->len > (UDP_DEMO_RX_BUFSIZE - data_len)) 
            {
                memcpy(udp_demo_recvbuf + data_len, q->payload, (UDP_DEMO_RX_BUFSIZE - data_len)); // 拷贝数据
            }
            else 
            {
                memcpy(udp_demo_recvbuf + data_len, q->payload, q->len);
            }
            data_len += q->len;
            if (data_len > UDP_DEMO_RX_BUFSIZE)   // 超出TCP客户端接收数组,跳出
            {
                break;                                         
            }
        }
        upcb->remote_ip = *addr;                                    // 记录远程主机的IP地址
        upcb->remote_port = port;                                   // 记录远程主机的端口号
        lwipdev.remoteip[0] = upcb->remote_ip.addr & 0xff;          // IADDR4
        lwipdev.remoteip[1] = (upcb->remote_ip.addr >> 8) & 0xff;   // IADDR3
        lwipdev.remoteip[2] = (upcb->remote_ip.addr >> 16) & 0xff;  // IADDR2
        lwipdev.remoteip[3] = (upcb->remote_ip.addr >> 24) & 0xff;  // IADDR1
        udp_demo_flag |= 1 << 6;                                    // 标记接收到数据了
        pbuf_free(p);                                               // 释放内存
    }
    else
    {
        printf("与远端主机%d.%d.%d.%d断开连接\r\n", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);
        udp_disconnect(upcb);
        udp_demo_flag &= ~(1<<5);   // 标记连接断开
    }
}

// UDP服务器发送函数
// 参数：upcb  ->  UDP 进程控制块
//       udp_sendbuf  ->  待发送的数据缓冲区
void udp_send_data(struct udp_pcb *upcb, unsigned char *udp_sendbuf)
{
    struct pbuf *ptr;
    ptr = pbuf_alloc(PBUF_TRANSPORT, strlen((char*)udp_sendbuf), PBUF_POOL);    // 申请内存
    if (ptr)
    {
        pbuf_take(ptr, (char*)udp_sendbuf, strlen((char*)udp_sendbuf)); // 将sendbuf中的数据打包进pbuf结构中
        udp_send(upcb, ptr);     // udp发送数据
        pbuf_free(ptr);          // 释放内存
    }
}

// 设置远端IP地址
void udp_set_remoteip(uint8_t id_num)
{
    uint8_t *tbuf;

    tbuf = mymalloc(SRAMIN, 100);   // 申请内存
    if (tbuf == NULL)
        return;
    // 前三个IP保持和DHCP得到的IP一致
    lwipdev.remoteip[0] = lwipdev.ip[0];
    lwipdev.remoteip[1] = lwipdev.ip[1];
    lwipdev.remoteip[2] = lwipdev.ip[2];
    lwipdev.remoteip[3] = id_num;
    sprintf((char*)tbuf, "Remote IP:%d.%d.%d.%d", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]); // 远端IP
    printf("远端IP地址：%d.%d.%d.%d\r\n", lwipdev.remoteip[0], lwipdev.remoteip[1], lwipdev.remoteip[2], lwipdev.remoteip[3]);

    myfree(SRAMIN, tbuf);
}

// 关闭UDP连接
void udp_connection_close(struct udp_pcb *upcb)
{
    udp_disconnect(upcb);
    udp_remove(upcb);            // 将一个进程控制块从链表中删除
    udp_demo_flag &= ~(1 << 5);  // 标记连接断开
    printf("UDP断开连接成功！");
}
