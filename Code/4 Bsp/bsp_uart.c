#include "bsp_uart.h"
#include <stdio.h>

u8 G510_RX_BUFF[G510_BUFFSIZE];
u8 G510_TX_BUFF[G510_BUFFSIZE];

u8 PC_RX_BUFF[PC_BUFFSIZE];
u8 PC_TX_BUFF[PC_BUFFSIZE+FE_NUM];

/*******************************************************************************
**函数名称：void UART1_Init(s32 int baudrate)
**功能描述：初始化USART模块
**入口参数：s32 int baudrate  -> 设置串口波特率
**输出：无
*******************************************************************************/
void UART1_Init(u32 baudrate)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , ENABLE);  //使能USART1时钟
  GPIO_Init(GPIOC , GPIO_Pin_2 , GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOC , GPIO_Pin_3 , GPIO_Mode_Out_PP_High_Fast);
  USART_Init(USART1,                //设置USART1
            baudrate,               //流特率设置
            USART_WordLength_8b,    //数据长度设为8位
            USART_StopBits_1,       //1位停止位
            USART_Parity_No,        //无校验
            (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));  //设置为发送接收双模式
  //使能接收中断
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART1 , ENABLE);   //使能USART1模块
}

void Uart1_SendData(u8  data)
{
    while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);        //判断发送数据寄存器是否为空
    USART_SendData8(USART1 , (u8)data);                     //向发送寄存器写入数据
}

void Uart2_SendData(u8  data)
{
    while(USART_GetFlagStatus(USART2 , USART_FLAG_TXE) == RESET);        //判断发送数据寄存器是否为空
    USART_SendData8(USART2 , (u8)data);                     //向发送寄存器写入数据
}

void Uart3_SendData(u8  data)
{
    while(USART_GetFlagStatus(USART3 , USART_FLAG_TXE) == RESET);        //判断发送数据寄存器是否为空
    USART_SendData8(USART3 , (u8)data);                     //向发送寄存器写入数据
}

void BSP_LOG(char *data )
{
  s32  datalen;
  u16 len=0;
  char *p_buf=data;
  len=strlen(p_buf);
  for(datalen = 0 ; datalen < len ; datalen++)
  {
    Uart1_SendData(data[datalen]);
  }
}

char* itoa(s32 num,char* str,s32 radix)
{
    /*索引表*/
    char index[]="0123456789ABCDEF";
    s32 unum;/*中间变量*/
    s32 i=0,j,k;
    /*确定unum的值*/
    if(radix==10&&num<0)/*十进制负数*/
    {
        unum=(s32)-num;
        str[i++]='-';
    }
    else unum=(s32)num;/*其他情况*/
    /*转换*/
    do{
        str[i++]=index[unum%(s32)radix];
        unum/=radix;
    }while(unum);
    str[i]='\0';
    /*逆序*/
    if(str[0]=='-')k=1;/*十进制负数*/
    else
      k=0;
    char temp;
    for(j=k;j<=(i-1)/2;j++)
    {
        temp=str[j];
        str[j]=str[i-1+k-j];
        str[i-1+k-j]=temp;
    }
    return str;
}

void BSP_LOG_Value(s32 data)
{
    s32 datalen;
    char buff[11];
    u16 len=0;
    itoa(data,buff,10);
    len=strlen(buff);
    for(datalen = 0 ; datalen < len ; datalen++)
    {
        Uart1_SendData(buff[datalen]);
    }
}

void UART3_Init(u32 baudrate)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART3 , ENABLE);  //使能USART3时钟
  GPIO_Init(GPIOG , GPIO_Pin_0 , GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOG , GPIO_Pin_1 , GPIO_Mode_Out_PP_High_Fast);
  USART_Init(USART3,                //设置USART3
            baudrate,               //流特率设置
            USART_WordLength_8b,    //数据长度设为8位
            USART_StopBits_1,       //1位停止位
            USART_Parity_No,        //无校验
            (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));  //设置为发送接收双模式
  //使能接收中断
  USART_ITConfig(USART3, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART3 , ENABLE);   //使能USART3模块
}

void UART2_Init(u32 baudrate)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2 , ENABLE);  //使能USART2时钟
  GPIO_Init(GPIOE , GPIO_Pin_3 , GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOE , GPIO_Pin_4 , GPIO_Mode_Out_PP_High_Fast);
  USART_Init(USART2,                //设置USART2
            baudrate,               //流特率设置
            USART_WordLength_8b,    //数据长度设为8位
            USART_StopBits_1,       //1位停止位
            USART_Parity_No,        //无校验
            (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));  //设置为发送接收双模式
  //使能接收中断
  USART_ITConfig(USART2, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART2 , ENABLE);   //使能USART2模块
}
             
////////////////////////////  帧检测 ///////////////////////////////////////////  
void Bsp_Uart_Data(void)
{
    if(System_Flag.flag_frame_time_en)
    {
       Frame_Time_Cnt++;
       if(Frame_Time_Cnt>=3) //将帧检测的时间控制在3*100ms（若是判断Frame_Time_Cnt==1，有可能是在UART中断中刚刚接收到0x68->Frame_Time_EN=1，来不及接收到0x16程序就进入了此处，这样马上Frame_Time_Cnt=1，进而UART_RxByteCnt=0，这样导致接收的数据又放在WIRED_COMM_Rx_Buff[0]开始处，导致错误）
       {                        //2400/3.3/10=72.2个字节，即在300ms内可接受72.2个字节（/3.3->300ms,,/10->8+起始+停止）
          Frame_Time_Cnt=0;
          UART_RxByteCnt = 0;
          System_Flag.flag_frame_time_en=0;
       }
    }
}


void BSP_UART_Transmit(U8* pTxBuff, U8 usLen)
{
    unsigned int datalen;
    
    for(datalen = 0 ; datalen < usLen ; datalen++)
    {
       Uart2_SendData(pTxBuff[datalen]);
    }
}

