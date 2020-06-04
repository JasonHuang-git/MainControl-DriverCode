#ifndef __APP_H__
#define __APP_H__

#include <bsp.h>
// 应用层函数声明区域
void flash_mode_app(void);
void nrf_mode_app(void);
void show_sdcard_info_app(void);
void show_memory_occupancy_app(uint8_t memx);
void fatfs_mount_app(void);
void lwip_config_app(void);
void udp_test_app(void);                 // UDP Server测试函数
void tcp_server_test_app(void);          // TCP Server测试函数
void tcp_client_test_app(void);          // TCP Client测试函数

#endif
