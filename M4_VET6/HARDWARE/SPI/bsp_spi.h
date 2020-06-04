#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include <bsp.h>			    
 	
#define SPI_RCC_CLK          RCC_AHB1Periph_GPIOB
#define SPI_AF_CLK           RCC_APB2Periph_SPI1
#define SPIX                 SPI1

#define SPI_PORT             GPIOB
#define SCK_PIN              GPIO_Pin_3
#define MISO_PIN             GPIO_Pin_4
#define MOSI_PIN             GPIO_Pin_5
#define SCK_SOURCE_PIN       GPIO_PinSource3
#define MISO_SOURCE_PIN      GPIO_PinSource4
#define MOSI_SOURCE_PIN      GPIO_PinSource5




void SPI1_Init(void);			              // 初始化SPI1口
void SPI1_SetSpeed(uint8_t SpeedSet);         // 设置SPI1速度   
uint8_t SPI1_ReadWriteByte(uint8_t TxData);   // SPI1总线读写一个字节
		 
#endif

