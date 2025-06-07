/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2016-xx-xx
  * @brief   ʹ�ô���1���ض���c��printf������usart�˿ڣ��жϽ���ģʽ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� STM32 F103 ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
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
                               StdPeriph �棺����/���պ���
==================================================================================================*/

/**
  * @brief  �� StdPeriph ����ָ�� USART ����һ���ֽڣ�������ʽ����
  * @param  USARTx: Ҫ���͵� USART ���裨�� USART1��USART2������
  * @param  ch:     Ҫ���͵ĵ��ֽ����ݡ�
  * @retval ��
  */
static void Usart_SendByte(USART_TypeDef *USARTx, uint8_t ch)
{
    /* �ȴ� TXE = 1����ʾ��һ�η�����ɣ�DR ��������λ�Ĵ��� */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
    {
        ;
    }
    /* �� ch д�� DR���������η��� */
    USART_SendData(USARTx, ch);
    /* �ȴ� TXE ��λ����֤ DR �ѱ�Ӳ������ */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
    {
        ;
    }
}

/**
  * @brief  �� StdPeriph ����ָ�� USART ���͹̶����ȵ��ֽ����飨������ʽ����
  * @param  USARTx:  Ҫ���͵� USART ����ָ�루�� USART1��USART2������
  * @param  pData:  ָ������ͻ������׵�ַ��
  * @param  length: Ҫ���͵��ֽ�����
  * @retval ��
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
  * @brief  �� StdPeriph ����ָ�� USART ������ '\0' ��β���ַ�����������ʽ����
  * @param  USARTx: Ҫ���͵� USART ����ָ�루�� USART1��USART2������
  * @param  pStr:   �� '\0' ��β���ַ������׵�ַ��
  * @retval ��
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
  * @brief  �ض��� C �⺯�� printf ��ָ���� StdPeriph ���ڣ�֧�� printf("��")��
  * @param  ch: Ҫ������ַ���ASCII����
  * @param  f:  �ļ�ָ�룬ͨ�� scanf/printf �ӿ��ò������ɺ��ԡ�
  * @retval ����ͬһ���ַ� ch����ʾ����ɹ���
  *
  * @note  ʹ��ǰ���ȵ��� DEBUG_USART_Config()��RCC/GPIO/NVIC/USART ���������ò�ʹ�ܣ���
  */
int fputc(int ch, FILE *f)
{
    /* �ȴ� TXE ��λ��DR ���У� */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET)
    {
        ;
    }
    /* д DR �������ڷ��� */
    USART_SendData(DEBUG_USART, (uint8_t)ch);
    /* �ٴεȴ� TXE��ȷ�������ֽ��������� */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET)
    {
        ;
    }
    return ch;
}

/**
  * @brief  �ض��� C �⺯�� scanf/getchar ��ָ���� StdPeriph ���ڣ�֧�� scanf��getchar��
  * @param  f: �ļ�ָ�룬�ɺ��ԡ�
  * @retval ���شӴ����յ���һ���ַ���int����
  *
  * @note  ʹ��ǰ���ȵ��� DEBUG_USART_Config()��RCC/GPIO/NVIC/USART ���������ò�ʹ�ܣ���
  */
int fgetc(FILE *f)
{
    /* �����ȴ� RXNE = 1�����յ�һ���ֽڣ� */
    while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET)
    {
        ;
    }
    /* �� DR �ж�ȡ���ݣ�ͬʱ��� RXNE ��־ */
    return (int)USART_ReceiveData(DEBUG_USART);
}



/*********************************************END OF FILE**********************/
