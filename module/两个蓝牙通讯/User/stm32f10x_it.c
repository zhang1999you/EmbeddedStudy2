/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTI
  
  AL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "./usart/bsp_usart_blt.h"
#include "bsp_debug_usart.h"
extern volatile uint32_t TimingDelay;
extern ReceiveData BLT_USART_ReceiveData;
extern ReceiveData DEBUG_USART_ReceiveData;
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    if (TimingDelay != 0)
        TimingDelay--;
}


void USART3_IRQHandler(void)
{
	
    uint8_t uch;
    // 1) �����жϣ�RXNE
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        // �����ݼĴ�����ȡһ���ֽ�
        uch = (uint8_t)USART_ReceiveData(USART3);

        // ���뻺������Ԥ�� 1 �ֽڷŽ�����
        if (BLT_USART_ReceiveData.datanum < (UART_BUFF_SIZE - 1))
        {
            BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum++] = uch;
        }

        // ��� RXNE �жϹ���λ
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }

    // 2) �����жϣ�IDLE
    if (USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
    {
        // ����ѽ������
        BLT_USART_ReceiveData.receive_data_flag = 1;

        // ��� IDLE ��־����ȡ SR �ٶ� DR
        (void)USART3->SR;
        (void)USART3->DR;

        // ������������������Ҳ���Ե��ã�
        // USART_ClearFlag(USART3, USART_FLAG_IDLE);
    }
}



void USART1_IRQHandler(void)
{
    uint8_t uch;
    /* 1) �����ж�: RXNE */
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* ��ȡ DR �Ĵ��������һ���ֽ� */
        uch = (uint8_t)USART_ReceiveData(USART1);
        
        /* ���������δ��������˵����У�0x0A���ͻس���0x0D������� */
        if (DEBUG_USART_ReceiveData.datanum < UART_BUFF_SIZE)
        {
            if (uch != '\n' && uch != '\r')
            {
                DEBUG_USART_ReceiveData.uart_buff[ DEBUG_USART_ReceiveData.datanum++ ] = uch;
            }
        }
        
        /* ��� RXNE �жϹ���λ */
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
    
    /* 2) �����ж�: IDLE ������ ����֡������� */
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
		Usart_SendString( USART1,"Debug1\n");
        /* ����ѽ������ */
        DEBUG_USART_ReceiveData.receive_data_flag = 1;
        
        /* ��� IDLE ��־����ȡ SR �ٶ� DR */
        (void)USART1->SR;
        (void)USART1->DR;
        
        /* ������ÿ⺯����Ҳ�ɳ��ԣ�
         * USART_ClearFlag(USART1, USART_FLAG_IDLE);
         */
    }
}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
