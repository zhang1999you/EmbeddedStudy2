/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ²âÊÔled
  ******************************************************************************
  * @attention
  *
  * ÊµÑéÆ½Ì¨:Ò°»ð F103-MINI STM32 ¿ª·¢°å 
  * ÂÛÌ³    :http://www.firebbs.cn
  * ÌÔ±¦    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "main.h"
#include "bsp_debug_usart.h"
#include "bsp_usart_blt.h"

#define SOFT_DELAY Delay(0x0FFFFF);
void USART1_SendChar(uint8_t ch) {
    /* µÈ´ý·¢ËÍ¼Ä´æÆ÷¿Õ */
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    /* ·¢ËÍµ¥¸ö×Ö½Ú */
    USART_SendData(USART1, ch);
}
void Delay(__IO u32 nCount); 

/**
  * @brief  Ö÷º¯Êý
  * @param  ÎÞ  
  * @retval ÎÞ
  */
int main(void)
{	
	SystemClock_Config();
	DEBUG_USART_Config();
	BLT_USART_Config();
	USART_SendData(USART1, (uint16_t)'1');
	printf("\r\n À¶ÑÀÄ£¿é-ATÖ¸Áî²âÊÔÀý³Ì  \r\n"); 
	
	while (1)
	{


	}
}

void Delay(__IO uint32_t nCount)	 //¼òµ¥µÄÑÓÊ±º¯Êý
{
	for(; nCount != 0; nCount--);
}

void SystemClock_Config(void)
{
    ErrorStatus HSEStartUpStatus;

    /* 1. ?? RCC ????????,????????????? */
    RCC_DeInit();

    /* 2. ?? HSE */
    RCC_HSEConfig(RCC_HSE_ON);

    /* 3. ?? HSE ?? */
    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    if (HSEStartUpStatus == SUCCESS)
    {
        /* 4. ?? Flash ?????? Flash ????? 2 ????? 
         *    (72MHz ???????,??????????:
         *     0–24MHz:0 WS
         *     24–48MHz:1 WS
         *     48–72MHz:2 WS)
         */
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);

        /* 5. ?? AHB(HCLK) = SYSCLK / 1 */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);

        /* 6. ?? APB2(PCLK2) = HCLK / 1 */
        RCC_PCLK2Config(RCC_HCLK_Div1);

        /* 7. ?? APB1(PCLK1) = HCLK / 2 
         *    (APB1 ?? 36MHz,?? 72MHz ???? /2)
         */
        RCC_PCLK1Config(RCC_HCLK_Div2);

        /* 8. ?? PLL:
         *    - PLL ???:HSE / 1 (???? HSE/2 ???,?? RCC_PLLSource_HSE_Div2)
         *    - PLL ????:9 ? ?? 8MHz * 9 = 72MHz
         */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

        /* 9. ?? PLL */
        RCC_PLLCmd(ENABLE);

        /* 10. ?? PLL ?? */
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

        /* 11. ?? PLL ??????? */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

        /* 12. ??????,?????????? PLL */
        while (RCC_GetSYSCLKSource() != 0x08);  
        // 0x08 ?? PLL ??? SYSCLK;????? RCC_SYSCLKSource_PLLCLK<<2 ????
    }
    else
    {
        /* ?? HSE ????,??????????????? */
        while (1) {;}
    }
}




/*********************************************END OF FILE**********************/
