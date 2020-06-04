#ifndef __SPI_H
#define __SPI_H

#include <bsp.h>

#define SPI_NVIC

// 宏定义
#define SPIx_CLK	 		RCC_AHB1Periph_GPIOB
#define SPIx_PerClk 		RCC_APB1Periph_SPI2
#define SPIx				SPI2
#define SPIx_IRQn			SPI2_IRQn
#define GPIO_AF_SPIx        GPIO_AF_SPI2
#define SPI_BaudRate        SPI_BaudRatePrescaler_8

#define SPIx_PORT			GPIOB
#define SPI_CS				GPIO_Pin_12			// 备用
#define SPI_SCK_PIN			GPIO_Pin_13
#define SPI_MISO_PIN		GPIO_Pin_14
#define SPI_MOSI_PIN		GPIO_Pin_15

#define SPI_SCK_AF_PIN		GPIO_PinSource13
#define SPI_MOSI_AF_PIN		GPIO_PinSource14
#define SPI_MISO_AF_PIN		GPIO_PinSource15


// 用户外部调用函数
void spi_init(void);			 		 // 初始化SPI2口
void spi_set_speed(uint8_t speed_set);	 // 设置SPI2速度   
void spi_write_byte(uint8_t tx_data);	 // SPI2总线写一个字节
uint8_t spi_read_byte(void);			 // SPI2总线读一个字节

#endif
