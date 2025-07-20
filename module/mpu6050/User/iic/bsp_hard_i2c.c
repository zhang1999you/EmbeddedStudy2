
#include "iic/bsp_hard_i2c.h"
#include "bsp_debug_usart.h"
#include "mpu6050/mpu6050.h"
void MPU_I2C_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_HARD_GPIO_PORT, ENABLE);   //����GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_HARD_I2C_PORT, ENABLE);    //����MPU_I2Cʱ��
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;            //���ÿ�©����ģʽ
    GPIO_InitStructure.GPIO_Pin   = I2C_SCL_PIN | I2C_SDA_PIN;    //MPU_I2C_SCL <--> PB10  MPU_I2C_SDA <--> PB11
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);
	
    I2C_DeInit(MPU_I2C);           //��MPU_I2C����Ĵ�����ʼ��Ϊ��Ĭ������ֵ
	I2C_InitStructure.I2C_Ack                   = I2C_Ack_Enable; 
    I2C_InitStructure.I2C_AcknowledgedAddress   = I2C_AcknowledgedAddress_7bit; //����Ѱַ��ַΪ7λ��ַģʽ
    I2C_InitStructure.I2C_ClockSpeed            = 400000;                       //����I2C����Ϊ���ֵ400kHz
    I2C_InitStructure.I2C_DutyCycle             = I2C_DutyCycle_2;              //���õͱȸߵ�ƽʱ��Ϊ2:1
    I2C_InitStructure.I2C_Mode                  = I2C_Mode_I2C;                 //����ΪI2Cģʽ
    I2C_InitStructure.I2C_OwnAddress1           = I2Cx_OWN_ADDRESS7;            //���������豸��ַ
    I2C_Init(MPU_I2C, &I2C_InitStructure);                                      //��ʼ��IIC
    I2C_Cmd(MPU_I2C, ENABLE);      
}



//����ʱ�˳����¼��ȴ�����
void WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t timecount=40000;
	while( I2C_CheckEvent(I2Cx,I2C_EVENT) ==  ERROR )
	{
        
		timecount--;
		if(timecount == 0)//��ʱ�ȴ��˳�
		{
			break;
		}
	}
}

void I2C_ByteWrite(uint8_t pBuffer, uint8_t WriteAddr)
{
    while( I2C_GetFlagStatus(MPU_I2C, I2C_FLAG_BUSY) );        //�ȴ����߿���
    I2C_GenerateSTART(MPU_I2C, ENABLE);
    while( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR );     //���EV5�¼�
    
    I2C_Send7bitAddress(MPU_I2C, MPU6050_SLAVE_ADDRESS, I2C_Direction_Transmitter);
    WaitEvent(MPU_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);     //���EV6�¼�
    
    I2C_SendData(MPU_I2C, WriteAddr);//���ͼĴ�����ַ
    WaitEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING);     //���EV8�¼�
    
    I2C_SendData(MPU_I2C, pBuffer);//��������
    WaitEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);     //���EV8_2�¼�
    
    I2C_GenerateSTOP(MPU_I2C, ENABLE);
    
}

void I2C_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{
    while( I2C_GetFlagStatus(MPU_I2C, I2C_FLAG_BUSY) );        //�ȴ����߿���
    I2C_GenerateSTART(MPU_I2C, ENABLE);
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
    
    I2C_Send7bitAddress(MPU_I2C, MPU6050_SLAVE_ADDRESS, I2C_Direction_Transmitter);//ָ����ַ����д��
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR);
    
    /*ͨ����������PEλ���EV6�¼� */
    I2C_Cmd(MPU_I2C, ENABLE);
    
    I2C_SendData(MPU_I2C, ReadAddr);
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR);
    
    I2C_GenerateSTART(MPU_I2C, ENABLE);
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_MODE_SELECT) == ERROR);
    
    I2C_Send7bitAddress(MPU_I2C, MPU6050_SLAVE_ADDRESS, I2C_Direction_Receiver);//ָ����ַ�������Ƕ��ģ��ڲ����Զ������Ķ�дλд�ϣ������ֶ�д��
    while ( I2C_CheckEvent(MPU_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR);//ע��EV6�¼��Ľ����Լ����������ǲ�һ���ģ���ת��������ϸ�ۿ���

    /* ��ȡNumByteToRead������*/
    while (NumByteToRead)
    {
        /*��NumByteToRead=1����ʾ�Ѿ����յ����һ�������ˣ�
        ���ͷ�Ӧ���źţ���������*/
        if (NumByteToRead == 1)
        {
            /* ���ͷ�Ӧ���ź� */
            I2C_AcknowledgeConfig(MPU_I2C, DISABLE);

            /* ����ֹͣ�ź� */
            I2C_GenerateSTOP(MPU_I2C, ENABLE);
        }

        WaitEvent(MPU_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED);
        {
            /*ͨ��I2C�����豸�ж�ȡһ���ֽڵ����� */
            *pBuffer = I2C_ReceiveData(MPU_I2C);

            /* �洢���ݵ�ָ��ָ����һ����ַ */
            pBuffer++;

            /* ���������Լ� */
            NumByteToRead--;
        }
    }


    I2C_AcknowledgeConfig(MPU_I2C, ENABLE);//����Ӧ����1���ظ�Ĭ�ϵ�ֵ���������������
    
}
