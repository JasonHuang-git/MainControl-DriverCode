#ifndef __BSP_UART6_H
#define __BSP_UART6_H

#include <bsp.h>

// 修改为其他串口时注意修改挂载的总线函数
#define USARTx			 	USART6
#define USARTx_IRQn		 	USART6_IRQn
#define GPIO_AF_USARTx   	GPIO_AF_USART6
#define USARTx_AF_CLK     	RCC_APB2Periph_USART6
#define USARTx_GPIO_CLK   	RCC_AHB1Periph_GPIOC

#define USARTx_TXD_PORT	 	GPIOC
#define USARTx_TXD_PIN		GPIO_Pin_6
#define USARTx_TXD_SOURCE 	GPIO_PinSource6

#define USARTx_RXD_PORT	 	GPIOC
#define USARTx_RXD_PIN		GPIO_Pin_7
#define USARTx_RXD_SOURCE 	GPIO_PinSource7

// 外部调用函数
void usart6_init(uint32_t bound);
void usart6_nvic_init(void);
void usart6_sendstr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void usart6_sendstring( USART_TypeDef * pUSARTx, uint8_t *str);

#endif /* __BSP_UART6_H */
