#ifndef __SPI_H
#define __SPI_H

#include <bsp.h>

#define SPI_NVIC

// �궨��
#define SPIx_CLK	 		RCC_AHB1Periph_GPIOB
#define SPIx_PerClk 		RCC_APB1Periph_SPI2
#define SPIx				SPI2
#define SPIx_IRQn			SPI2_IRQn
#define GPIO_AF_SPIx        GPIO_AF_SPI2
#define SPI_BaudRate        SPI_BaudRatePrescaler_8

#define SPIx_PORT			GPIOB
#define SPI_CS				GPIO_Pin_12			// ����
#define SPI_SCK_PIN			GPIO_Pin_13
#define SPI_MISO_PIN		GPIO_Pin_14
#define SPI_MOSI_PIN		GPIO_Pin_15

#define SPI_SCK_AF_PIN		GPIO_PinSource13
#define SPI_MOSI_AF_PIN		GPIO_PinSource14
#define SPI_MISO_AF_PIN		GPIO_PinSource15


// �û��ⲿ���ú���
void spi_init(void);			 		 // ��ʼ��SPI2��
void spi_set_speed(uint8_t speed_set);	 // ����SPI2�ٶ�   
void spi_write_byte(uint8_t tx_data);	 // SPI2����дһ���ֽ�
uint8_t spi_read_byte(void);			 // SPI2���߶�һ���ֽ�

#endif
