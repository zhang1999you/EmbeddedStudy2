#include "mpu6050/mpu6050.h"
#include "iic/bsp_hard_i2c.h"
#include "delay.h"
#include "bsp_debug_usart.h"
void MPU6050_WriteReg(u8 reg_add,u8 reg_dat)
{
    I2C_ByteWrite(reg_dat,reg_add);
}
void MPU6050_ReadData(u8 reg_add,unsigned char*Read,u8 num)
{
    I2C_BufferRead(Read,reg_add,num);
}

void MPU6050_Init(void)
{
	Delay(100); 
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x00);	    //�������״̬
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	    //�����ǲ����ʣ�1KHz
	MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);	        //��ͨ�˲��������ã���ֹƵ����1K��������5K
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x00);	  //���ü��ٶȴ�����������2Gģʽ�����Լ�
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);     //�������Լ켰������Χ������ֵ��0x18(���Լ죬2000deg/s)

}

uint8_t MPU6050ReadID(void)
{
	unsigned char Re = 0;
    MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);    //��������ַ
	if(Re != 0x68)
	{
		printf("MPU6050 dectected error!\r\n��ⲻ��MPU6050ģ�飬����ģ���뿪����Ľ���");
		return 0;
	}
	else
	{
		printf("MPU6050 ID = %d\r\n",Re);
		return 1;
	}
		
}
