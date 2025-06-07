/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   使用串口1，重定向c库printf函数到usart端口，中断接收模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 F103 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "bsp_debug_usart.h"



#include "stm32f10x.h"            
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"              
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

void DEBUG_USART_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* 1. ?? USART ???? */
    RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);

    /* 2. ???? GPIO ????(TX ? RX ????? GPIOA,???????) */
    RCC_APB2PeriphClockCmd(DEBUG_USART_TX_GPIO_CLK | DEBUG_USART_RX_GPIO_CLK, ENABLE);

    /* 3. ?? TX(PA9)??????? */
    GPIO_InitStructure.GPIO_Pin   = DEBUG_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

    /* 4. ?? RX(PA10)????? */
    GPIO_InitStructure.GPIO_Pin   = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);

    /* 5. ?? NVIC ???:?? USART ?? */
    NVIC_InitStructure.NVIC_IRQChannel                   = DEBUG_USART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    // ???????????
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 6. ?? USART ?? */
    USART_InitStructure.USART_BaudRate            = DEBUG_USART_BAUDRATE;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(DEBUG_USART, &USART_InitStructure);

    /* 7. ?? USART ????(RXNE)?????(IDLE) */
    USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);
    USART_ITConfig(DEBUG_USART, USART_IT_IDLE, ENABLE);

    /* 8. ???? USART ?? */
    USART_Cmd(DEBUG_USART, ENABLE);
}








/*==================================================================================================
                               StdPeriph 版：发送/接收函数
==================================================================================================*/

/**
  * @brief  用 StdPeriph 库向指定 USART 发送一个字节（阻塞方式）。
  * @param  USARTx: 要发送的 USART 外设（如 USART1、USART2…）。
  * @param  ch:     要发送的单字节数据。
  * @retval 无
  */
static void Usart_SendByte(USART_TypeDef *USARTx, uint8_t ch)
{
    /* 等待 TXE = 1，表示上一次发送完成，DR 已移入移位寄存器 */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
    {
        ;
    }
    /* 把 ch 写到 DR，启动本次发送 */
    USART_SendData(USARTx, ch);
    /* 等待 TXE 置位，保证 DR 已被硬件移走 */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
    {
        ;
    }
}

/**
  * @brief  用 StdPeriph 库向指定 USART 发送固定长度的字节数组（阻塞方式）。
  * @param  USARTx:  要发送的 USART 外设指针（如 USART1、USART2…）。
  * @param  pData:  指向待发送缓冲区首地址。
  * @param  length: 要发送的字节数。
  * @retval 无
  */
void Usart_SendStr_length(USART_TypeDef *USARTx, uint8_t *pData, uint32_t length)
{
    uint32_t idx = 0;
    for (idx = 0; idx < length; idx++)
    {
        Usart_SendByte(USARTx, pData[idx]);
    }
}

/**
  * @brief  用 StdPeriph 库向指定 USART 发送以 '\0' 结尾的字符串（阻塞方式）。
  * @param  USARTx: 要发送的 USART 外设指针（如 USART1、USART2…）。
  * @param  pStr:   以 '\0' 结尾的字符数组首地址。
  * @retval 无
  */
void Usart_SendString(USART_TypeDef *USARTx, uint8_t *pStr)
{
    uint32_t idx = 0;
    while (pStr[idx] != '\0')
    {
        Usart_SendByte(USARTx, pStr[idx]);
        idx++;
    }
}

/**
  * @brief  重定向 C 库函数 printf 到指定的 StdPeriph 串口，支持 printf("…")。
  * @param  ch: 要输出的字符（ASCII）。
  * @param  f:  文件指针，通常 scanf/printf 接口用不到，可忽略。
  * @retval 返回同一个字符 ch，表示输出成功。
  *
  * @note  使用前须先调用 DEBUG_USART_Config()（RCC/GPIO/NVIC/USART 参数已配置并使能）。
  */
int fputc(int ch, FILE *f)
{
    /* 等待 TXE 置位（DR 空闲） */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET)
    {
        ;
    }
    /* 写 DR 触发串口发送 */
    USART_SendData(DEBUG_USART, (uint8_t)ch);
    /* 再次等待 TXE，确保本次字节真正发出 */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET)
    {
        ;
    }
    return ch;
}

/**
  * @brief  重定向 C 库函数 scanf/getchar 到指定的 StdPeriph 串口，支持 scanf、getchar。
  * @param  f: 文件指针，可忽略。
  * @retval 返回从串口收到的一个字符（int）。
  *
  * @note  使用前须先调用 DEBUG_USART_Config()（RCC/GPIO/NVIC/USART 参数已配置并使能）。
  */
int fgetc(FILE *f)
{
    /* 阻塞等待 RXNE = 1（接收到一个字节） */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET)
    {
        ;
    }
    /* 从 DR 中读取数据，同时清除 RXNE 标志 */
    return (int)USART_ReceiveData(DEBUG_USART);
}



/*********************************************END OF FILE**********************/
