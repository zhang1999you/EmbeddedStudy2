#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include <stdio.h>
#include "stm32f10x.h"


#ifndef DEBUG_USART
  #define DEBUG_USART              USART1
#endif
#ifndef DEBUG_USART_CLK
  #define DEBUG_USART_CLK          RCC_APB2Periph_USART1
#endif
#ifndef DEBUG_USART_TX_GPIO_CLK
  #define DEBUG_USART_TX_GPIO_CLK  RCC_APB2Periph_GPIOA
#endif
#ifndef DEBUG_USART_RX_GPIO_CLK
  #define DEBUG_USART_RX_GPIO_CLK  RCC_APB2Periph_GPIOA
#endif
#ifndef DEBUG_USART_TX_GPIO_PIN
  #define DEBUG_USART_TX_GPIO_PIN  GPIO_Pin_9
#endif
#ifndef DEBUG_USART_RX_GPIO_PIN
  #define DEBUG_USART_RX_GPIO_PIN  GPIO_Pin_10
#endif
#ifndef DEBUG_USART_TX_GPIO_PORT
  #define DEBUG_USART_TX_GPIO_PORT GPIOA
#endif
#ifndef DEBUG_USART_RX_GPIO_PORT
  #define DEBUG_USART_RX_GPIO_PORT GPIOA
#endif
#ifndef DEBUG_USART_IRQn
  #define DEBUG_USART_IRQn         USART1_IRQn
#endif
#ifndef DEBUG_USART_BAUDRATE
  #define DEBUG_USART_BAUDRATE     115200
#endif


void DEBUG_USART_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);


#endif /* __USART1_H */
