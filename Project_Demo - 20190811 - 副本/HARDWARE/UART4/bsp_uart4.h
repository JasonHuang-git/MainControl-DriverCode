#ifndef __BSP_UART4_H
#define __BSP_UART4_H

#include <bsp.h>

// 修改为其他串口时注意修改挂载的总线函数
#define UARTx			 	UART4
#define UARTx_IRQn		 	UART4_IRQn
#define GPIO_AF_UARTx   	GPIO_AF_UART4
#define UARTx_AF_CLK     	RCC_APB1Periph_UART4
#define UARTx_GPIO_CLK   	RCC_AHB1Periph_GPIOA

#define UARTx_TXD_PORT	 	GPIOA
#define UARTx_TXD_PIN		GPIO_Pin_0
#define UARTx_TXD_SOURCE 	GPIO_PinSource0

#define UARTx_RXD_PORT	 	GPIOA
#define UARTx_RXD_PIN		GPIO_Pin_1
#define UARTx_RXD_SOURCE 	GPIO_PinSource1

// 外部调用函数
void uart4_init(uint32_t bound);
void uart4_nvic_init(void);
void usart4_sendstr_length( USART_TypeDef * pUSARTx, uint8_t *str,uint32_t strlen );
void usart4_sendstring( USART_TypeDef * pUSARTx, uint8_t *str);

#endif /* __BSP_UART4_H */
