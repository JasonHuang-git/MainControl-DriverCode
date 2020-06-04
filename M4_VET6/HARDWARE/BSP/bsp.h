#ifndef __BSP_H__
#define __BSP_H__


/* 标准C库头文件 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* 正点原子提供的头文件 */
#include <sys.h>
#include <delay.h>
#include <usart.h>
#include <string.h>
#include <malloc.h>

/* 用户自定义头文件 */
#include <bsp_lcd.h>
#include <bsp_led.h>
#include <bsp_key.h>
#include <bsp_nrf24l01.h>
#include <bsp_ft6236.h>
#include <bsp_touch.h>
#include <bsp_sdio_sdcard.h>
#include <diskio.h>
#include <exfuns.h>
#include <fattester.h>
#include <bsp_spi.h>
#include <bsp_w25qxx.h>
#include <text.h>
#include <fontupd.h>
#include <app.h>
#include <bsp_timer.h>
#include <stm32f4x7_eth.h>
#include <udp_demo.h>
#include <tcp_server_demo.h>
#include <tcp_client_demo.h>

// LWIP相关头文件
#include <lwip_comm.h>
#include <ethernetif.h>
#include <netif/etharp.h>
#include <lwip/dhcp.h>
#include <lwip/mem.h>
#include <lwip/memp.h>
#include <lwip/init.h>
#include <lwip/timers.h>
#include <lwip/tcp_impl.h>
#include <lwip/ip_frag.h>
#include <lwip/tcpip.h>
#include <lwip/pbuf.h>
#include <lwip/udp.h>
#include <lwip/tcp.h>
#include <lwip/debug.h>
#include <lwip/stats.h>

/* 函数声明区域 */
void bsp_init(void);

#endif
