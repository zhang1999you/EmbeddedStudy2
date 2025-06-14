# EmbeddedStudy
1、模板
F103模板

2、模块
两个蓝牙通讯  
## 2025.6.14
### 找到Usart标准库的模板，目前完成对Usart1和Usart3的初始化，
```
#define DEBUG_USART              USART1
#define DEBUG_USART_CLK          RCC_APB2Periph_USART1
#define DEBUG_USART_TX_GPIO_CLK  RCC_APB2Periph_GPIOA
#define DEBUG_USART_RX_GPIO_CLK  RCC_APB2Periph_GPIOA
#define DEBUG_USART_TX_GPIO_PIN  GPIO_Pin_9
#define DEBUG_USART_RX_GPIO_PIN  GPIO_Pin_10
#define DEBUG_USART_TX_GPIO_PORT GPIOA
#define DEBUG_USART_RX_GPIO_PORT GPIOA
#define DEBUG_USART_IRQn         USART1_IRQn
#define DEBUG_USART_BAUDRATE     115200
void DEBUG_USART_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);

    RCC_APB2PeriphClockCmd(DEBUG_USART_TX_GPIO_CLK | DEBUG_USART_RX_GPIO_CLK, ENABLE);


    GPIO_InitStructure.GPIO_Pin   = DEBUG_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin   = DEBUG_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);


    NVIC_InitStructure.NVIC_IRQChannel                   = DEBUG_USART_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    // ???????????
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    USART_InitStructure.USART_BaudRate            = DEBUG_USART_BAUDRATE;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(DEBUG_USART, &USART_InitStructure);


    USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);


    USART_Cmd(DEBUG_USART, ENABLE);
}
```
其中碰到一个问题就是，上述代码初始化Usart1串口打印没有问题（即Usart_SendString( USART1,"Debug串口正常\n")有输出），但是相同的代码初始化Usart3有问题，下面是发送函数定义其是polling发送的
```
/*****************  发送一个字节 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** 发送8位的数组 ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* 发送一个字节数据到USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* 等待发送完成 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
```
出现Usart3无法工作的原因在于错误地打开了 USART3 的接收中断，却没有提供对应的中断处理与清标志代码，导致 MCU 卡在伪中断里，主循环里的发送根本没机会跑。
#### 所以移植到Usart3时就要删除USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE) 或者对USART3_IRQHandler进行定义，删除标志位
```
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE))
    {
        volatile uint8_t dummy = USART_ReceiveData(USART3);
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
    // …如果还开了 IDLE、TXE 中断，也都要同样清标志…
}

```
USART1 之所以没卡在“伪中断”里，最主要的差别在于它的 RX 引脚是实实在在连在 CH340 模块的 TXD 上，并且在空闲时被拉到了一个稳定的高电平；而你一开始的 USART3 RX（PB11）要么是浮空、要么连在一个噪声很大的地方，就更容易产生“垃圾”帧，从而触发 RXNE。

### HC05引脚说明
![alt text](CA{H9HU(1BSQ8W4~DY~)NBA.png)
