#include "stm32f10x_it.h"
#include "stm32f10x.h"
#include "main.h"
#include "bsp_debug_usart.h"
#include "iic/bsp_hard_i2c.h"
#include "mpu6050/mpu6050.h"
#include "delay.h"

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
	MPU_I2C_Config();
	MPU6050_Init();
    if(MPU6050ReadID() == 0)
    {
		printf("没有检测到MPU6050传感器\r\n",Re);
        while(1);	
    }
	
	while (1)
	{
		if(task_readdata_finish)
		{
			printf("\r\n加速度： %8d%8d%8d    ",Accel[0],Accel[1],Accel[2]);
			printf("陀螺仪： %8d%8d%8d    ",Gyro[0],Gyro[1],Gyro[2]);
			printf("温度： %8.2f°C",Temp);
			task_readdata_finish = 0; // 清零标志位
		}


	}
}


void SystemClock_Config(void)
{
    ErrorStatus HSEStartUpStatus;


    RCC_DeInit();


    RCC_HSEConfig(RCC_HSE_ON);


    HSEStartUpStatus = RCC_WaitForHSEStartUp();
    if (HSEStartUpStatus == SUCCESS)
    {

        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);


        RCC_HCLKConfig(RCC_SYSCLK_Div1);


        RCC_PCLK2Config(RCC_HCLK_Div1);

        RCC_PCLK1Config(RCC_HCLK_Div2);


        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);


        RCC_PLLCmd(ENABLE);


        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);


        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);


        while (RCC_GetSYSCLKSource() != 0x08);  
        
    }
    else
    {

        while (1) {;}
    }
}
/*********************************************END OF FILE**********************/
