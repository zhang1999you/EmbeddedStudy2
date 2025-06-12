
#include "bsp_usart_blt.h"
#include <stdarg.h>




#include "stm32f10x.h"            // StdPeriph ????,?? RCC/GPIO/USART/NVIC ????
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "misc.h"                 // NVIC_InitTypeDef ????


#define BLT_USART3                  USART3
#define BLT_USART3_CLK              RCC_APB1Periph_USART3
#define BLT_USART3_TX_GPIO_CLK      RCC_APB2Periph_GPIOB
#define BLT_USART3_RX_GPIO_CLK      RCC_APB2Periph_GPIOB
#define BLT_USART3_TX_PIN           GPIO_Pin_10
#define BLT_USART3_RX_PIN           GPIO_Pin_11
#define BLT_USART3_GPIO_PORT        GPIOB
#define BLT_USART3_IRQn             USART3_IRQn
#define BLT_USART_BAUD_RATE         9600


USART_InitTypeDef  BLT_USART_InitStruct;
USART_TypeDef     *BLT_USARTx = BLT_USART3;
extern ReceiveData BLT_USART_ReceiveData;


void BLT_USART_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    /* 1. ?? USART3 ???? */
    RCC_APB1PeriphClockCmd(BLT_USART3_CLK, ENABLE);

    /* 2. ?? GPIOB ??(PB10/TX, PB11/RX ?? GPIOB) */
    RCC_APB2PeriphClockCmd(BLT_USART3_TX_GPIO_CLK | BLT_USART3_RX_GPIO_CLK, ENABLE);

    /* 3. ?? PB10 ???????(USART3 TX) */
    GPIO_InitStructure.GPIO_Pin   = BLT_USART3_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(BLT_USART3_GPIO_PORT, &GPIO_InitStructure);

    /* 4. ?? PB11 ?????(USART3 RX) */
    GPIO_InitStructure.GPIO_Pin   = BLT_USART3_RX_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;  // ????
    GPIO_Init(BLT_USART3_GPIO_PORT, &GPIO_InitStructure);

    /* 5. ?? NVIC:?? USART3 ?? */
    NVIC_InitStructure.NVIC_IRQChannel                   = BLT_USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    // ??????????
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* 6. ?? USART3 ?? */
    BLT_USART_InitStruct.USART_BaudRate            = BLT_USART_BAUD_RATE;
    BLT_USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    BLT_USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    BLT_USART_InitStruct.USART_Parity              = USART_Parity_No ;
    BLT_USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    BLT_USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(BLT_USARTx, &BLT_USART_InitStruct);

    /* 7. ?? USART3 ????(RXNE)?????(IDLE) */
    USART_ITConfig(BLT_USARTx, USART_IT_RXNE, ENABLE);
    USART_ITConfig(BLT_USARTx, USART_IT_IDLE, ENABLE);

    /* 8. ???? USART3 ?? */
    USART_Cmd(BLT_USARTx, ENABLE);
}


//获取接收到的数据和长度
char *get_rebuff(uint16_t *len) 
{
    *len = BLT_USART_ReceiveData.datanum;
    return (char *)&BLT_USART_ReceiveData.uart_buff;
}


//清空缓冲区
void clean_rebuff(void)
{
    uint16_t i=UART_BUFF_SIZE+1;
    BLT_USART_ReceiveData.datanum = 0;
	  BLT_USART_ReceiveData.receive_data_flag = 0;
	while(i)
		BLT_USART_ReceiveData.uart_buff[--i]=0;
}




/******************* (C) COPYRIGHT 2024 WildFire Team *****END OF FILE************/
