#include "hc05/bsp_hc05.h"
#include <string.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "bsp_usart_blt.h"
#include "bsp_debug_usart.h"
#include "delay.h"

ReceiveData BLT_USART_ReceiveData;
uint8_t HC05_Send_CMD(char* cmd,uint8_t clean)
{	 		 
  uint8_t retry=3;
	uint32_t i;
	uint16_t len;
  char * redata;
  
	while(retry--)
	{
        GPIO_SetBits(BLT_KEY_GPIO_PORT, BLT_KEY_GPIO_PIN);
        Usart_SendString(BLT_USART,(char *)cmd);
        i=200;              //��ʼ��i����ȴ�2��
		Delay(10);
		do
        {
            if(BLT_USART_ReceiveData.receive_data_flag == 1)
            {
                BLT_USART_ReceiveData.uart_buff[BLT_USART_ReceiveData.datanum] = '\0';
                redata = get_rebuff(&len); 
                if(len>0)
                {
                    if(strstr(redata,"OK"))				
                    {
                        //��ӡ���͵�����ָ��ͷ�����Ϣ
						Usart_SendString(DEBUG_USART, "send CMD: ");
						Usart_SendString(DEBUG_USART, cmd);
						Usart_SendString(DEBUG_USART, "\r\n");
						
						Usart_SendString(DEBUG_USART, "recv back: ");
						Usart_SendString(DEBUG_USART, redata);
						Usart_SendString(DEBUG_USART, "\r\n");
                        
                        if(clean==1)
                            clean_rebuff();
                        
                        return 0; //ATָ��ɹ�
                    }
                }
            }
            
            Delay(10);
            
        }while( --i ); //�����ȴ�
        Usart_SendString(DEBUG_USART, "send CMD: ");
		Usart_SendString(DEBUG_USART, cmd);
		Usart_SendString(DEBUG_USART, "\r\n");
		
		
		Usart_SendString(DEBUG_USART, "recv back: ");
		Usart_SendString(DEBUG_USART, redata);
		Usart_SendString(DEBUG_USART, "\r\n");
        
		// ������ת��Ϊ�ַ���
		Usart_SendString(DEBUG_USART, "HC05 send CMD fail %d times");
		Usart_SendString(DEBUG_USART, (char *)retry);
		Usart_SendString(DEBUG_USART, "\r\n");
		
    
    }
	Usart_SendString(DEBUG_USART, "HC05 send CMD fail");
	if(clean==1)
		clean_rebuff();

	return 1; //ATָ��ʧ��
}





static void HC05_GPIO_Config(void)
{
	/*����GPIO
    *INT -- �������� -- �ж��Ƿ�ģ������
    *KEY -- ������� -- �����Ƿ�ģ�����ATģʽ
    */
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(BLT_INT_GPIO_CLK | BLT_KEY_GPIO_CLK, ENABLE);
    /* 2. ���� INT ���ţ��������룬�����ж�ģ���Ƿ����� */
    GPIO_InitStructure.GPIO_Pin   = BLT_INT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
    GPIO_Init(BLT_INT_GPIO_PORT, &GPIO_InitStructure);

    /* 3. ���� KEY ���ţ�������������ھ���ģ����� AT ģʽ */
    GPIO_InitStructure.GPIO_Pin  = BLT_KEY_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
    GPIO_Init(BLT_KEY_GPIO_PORT, &GPIO_InitStructure);

    /* 4. Ĭ�Ͻ� KEY �������ͣ�ģ������������ */
    //GPIO_ResetBits(BLT_KEY_GPIO_PORT, BLT_KEY_GPIO_PIN);

}

uint8_t HC05_Init(void)
{
	//������ 1����ʼ��int��key��GPIO    2����ʼ��uart��GPIO
	//1��
	HC05_GPIO_Config();//��ʼ��int��key��GPIO
	//2��
	BLT_USART_Config();
	
	
	return HC05_Send_CMD("AT\r\n",1);
}






