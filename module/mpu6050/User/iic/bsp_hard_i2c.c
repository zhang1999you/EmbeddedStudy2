
#include "iic/bsp_hard_i2c.h"
#include "bsp_debug_usart.h"
#include "mpu6050/mpu6050.h"
void MPU_I2C_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_HARD_GPIO_PORT, ENABLE);   //开启GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_HARD_I2C_PORT, ENABLE);    //开启MPU_I2C时钟
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;            //设置开漏复用模式
    GPIO_InitStructure.GPIO_Pin   = I2C_SCL_PIN | I2C_SDA_PIN;    //MPU_I2C_SCL <--> PB10  MPU_I2C_SDA <--> PB11
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);
	
    I2C_DeInit(MPU_I2C);           //将MPU_I2C外设寄存器初始化为其默认重置值
	I2C_InitStructure.I2C_Ack                   = I2C_Ack_Enable; 
    I2C_InitStructure.I2C_AcknowledgedAddress   = I2C_AcknowledgedAddress_7bit; //设置寻址地址为7位地址模式
    I2C_InitStructure.I2C_ClockSpeed            = 400000;                       //设置I2C速率为最大值400kHz
    I2C_InitStructure.I2C_DutyCycle             = I2C_DutyCycle_2;              //设置低比高电平时间为2:1
    I2C_InitStructure.I2C_Mode                  = I2C_Mode_I2C;                 //设置为I2C模式
    I2C_InitStructure.I2C_OwnAddress1           = I2Cx_OWN_ADDRESS7;            //设置自身设备地址
    I2C_Init(MPU_I2C, &I2C_InitStructure);                                      //初始化IIC
    I2C_Cmd(MPU_I2C, ENABLE);      
}



//带超时退出的事件等待函数
void WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t timecount=40000;
	while( I2C_CheckEvent(I2Cx,I2C_EVENT) ==  ERROR )
	{
        
		timecount--;
		if(timecount == 0)//超时等待退出
		{
			break;
		}
	}
}

void I2C_ByteWrite(uint8_t pBuffer, uint8_t WriteAddr)
{
    while( I2C_GetFlagStatus(MPU_I2C, I2C_FLAG_BUSY) );        //等待总线空闲
    I2C_GenerateSTART(MPU_I2C, ENABLE);
    while( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR );     //检测EV5事件
    
    I2C_Send7bitAddress(MPU_I2C, MPU6050_SLAVE_ADDRESS, I2C_Direction_Transmitter);
    WaitEvent(MPU_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);     //检测EV6事件
    
    I2C_SendData(MPU_I2C, WriteAddr);//发送寄存器地址
    WaitEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING);     //检测EV8事件
    
    I2C_SendData(MPU_I2C, pBuffer);//发送数据
    WaitEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);     //检测EV8_2事件
    
    I2C_GenerateSTOP(MPU_I2C, ENABLE);
    
}

void I2C_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
    while( I2C_GetFlagStatus(MPU_I2C, I2C_FLAG_BUSY) );        //等待总线空闲
    I2C_GenerateSTART(MPU_I2C, ENABLE);
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
    
    I2C_Send7bitAddress(MPU_I2C, MPU6050_SLAVE_ADDRESS, I2C_Direction_Transmitter);//指定地址，是写的
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
    
    /*通过重新设置PE位清除EV6事件 */
    I2C_Cmd(MPU_I2C, ENABLE);
    
    I2C_SendData(MPU_I2C, ReadAddr);
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
    
    I2C_GenerateSTART(MPU_I2C, ENABLE);
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
    
    I2C_Send7bitAddress(MPU_I2C, MPU6050_SLAVE_ADDRESS, I2C_Direction_Receiver);//指定地址，不过是读的，内部会自动把最后的读写位写上，不用手动写。
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR);//注意EV6事件的接收以及发送两者是不一样的，跳转到定义仔细观看。

    /* 读取NumByteToRead个数据*/
    while (NumByteToRead)
    {
        /*若NumByteToRead=1，表示已经接收到最后一个数据了，
        发送非应答信号，结束传输*/
        if (NumByteToRead == 1)
        {
            /* 发送非应答信号 */
            I2C_AcknowledgeConfig(MPU_I2C, DISABLE);

            /* 发送停止信号 */
            I2C_GenerateSTOP(MPU_I2C, ENABLE);
        }

        WaitEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED);
        {
            /*通过I2C，从设备中读取一个字节的数据 */
            *pBuffer = I2C_ReceiveData(MPU_I2C);

            /* 存储数据的指针指向下一个地址 */
            pBuffer++;

            /* 接收数据自减 */
            NumByteToRead--;
        }
    }


    I2C_AcknowledgeConfig(MPU_I2C, ENABLE);//最后把应答置1，回复默认的值，方便后续的运用
    
}
