/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-MINI STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "main.h"
#include "bsp_debug_usart.h"
#define SOFT_DELAY Delay(0x0FFFFF);

void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* 1. 系统复位以及启动 HSE/PLL 等 */
    SystemInit();                 // CMSIS: 复位并配置系统时钟源到默认状态
    SystemClock_Config();         // SPL: 您自己写的 72MHz 时钟配置函数
	SysTick_Config(SystemCoreClock/1000);
	DEBUG_USART_Config();
	printf("Debug串口正常\n");
	//Usart_SendString( BLT_USART,"Debug串口正常\n");
	while (1)
	{


	}
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
         *     0?4MHz:0 WS
         *     24?8MHz:1 WS
         *     48?2MHz:2 WS)
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
