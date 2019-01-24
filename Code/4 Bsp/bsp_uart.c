#include "bsp_uart.h"
#include <stdio.h>

u8 G510_RX_BUFF[G510_BUFFSIZE];
u8 G510_TX_BUFF[G510_BUFFSIZE];

u8 PC_RX_BUFF[PC_BUFFSIZE];
u8 PC_TX_BUFF[PC_BUFFSIZE+FE_NUM];

/*******************************************************************************
**�������ƣ�void UART1_Init(s32 int baudrate)
**������������ʼ��USARTģ��
**��ڲ�����s32 int baudrate  -> ���ô��ڲ�����
**�������
*******************************************************************************/
void UART1_Init(u32 baudrate)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1 , ENABLE);  //ʹ��USART1ʱ��
  GPIO_Init(GPIOC , GPIO_Pin_2 , GPIO_Mode_In_PU_No_IT);
  GPIO_Init(GPIOC , GPIO_Pin_3 , GPIO_Mode_Out_PP_High_Fast);
  USART_Init(USART1,                //����USART1
            baudrate,               //����������
            USART_WordLength_8b,    //���ݳ�����Ϊ8λ
            USART_StopBits_1,       //1λֹͣλ
            USART_Parity_No,        //��У��
            (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));  //����Ϊ���ͽ���˫ģʽ
  //ʹ�ܽ����ж�
  USART_ITConfig(USART1, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART1 , ENABLE);   //ʹ��USART1ģ��
}

void Uart1_SendData(u8  data)
{
    while(USART_GetFlagStatus(USART1 , USART_FLAG_TXE) == RESET);        //�жϷ������ݼĴ����Ƿ�Ϊ��
    USART_SendData8(USART1 , (u8)data);                     //���ͼĴ���д������
}

void Uart2_SendData(u8  data)
{
    while(USART_GetFlagStatus(USART2 , USART_FLAG_TXE) == RESET);        //�жϷ������ݼĴ����Ƿ�Ϊ��
    USART_SendData8(USART2 , (u8)data);                     //���ͼĴ���д������
}

void Uart3_SendData(u8  data)
{
    while(USART_GetFlagStatus(USART3 , USART_FLAG_TXE) == RESET);        //�жϷ������ݼĴ����Ƿ�Ϊ��
    USART_SendData8(USART3 , (u8)data);                     //���ͼĴ���д������
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
    /*������*/
    char index[]="0123456789ABCDEF";
    s32 unum;/*�м����*/
    s32 i=0,j,k;
    /*ȷ��unum��ֵ*/
    if(radix==10&&num<0)/*ʮ���Ƹ���*/
    {
        unum=(s32)-num;
        str[i++]='-';
    }
    else unum=(s32)num;/*�������*/
    /*ת��*/
    do{
        str[i++]=index[unum%(s32)radix];
        unum/=radix;
    }while(unum);
    str[i]='\0';
    /*����*/
    if(str[0]=='-')k=1;/*ʮ���Ƹ���*/
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
  CLK_PeripheralClockConfig(CLK_Peripheral_USART3 , ENABLE);  //ʹ��USART3ʱ��
  GPIO_Init(GPIOG , GPIO_Pin_0 , GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOG , GPIO_Pin_1 , GPIO_Mode_Out_PP_High_Fast);
  USART_Init(USART3,                //����USART3
            baudrate,               //����������
            USART_WordLength_8b,    //���ݳ�����Ϊ8λ
            USART_StopBits_1,       //1λֹͣλ
            USART_Parity_No,        //��У��
            (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));  //����Ϊ���ͽ���˫ģʽ
  //ʹ�ܽ����ж�
  USART_ITConfig(USART3, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART3 , ENABLE);   //ʹ��USART3ģ��
}

void UART2_Init(u32 baudrate)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_USART2 , ENABLE);  //ʹ��USART2ʱ��
  GPIO_Init(GPIOE , GPIO_Pin_3 , GPIO_Mode_In_FL_No_IT);
  GPIO_Init(GPIOE , GPIO_Pin_4 , GPIO_Mode_Out_PP_High_Fast);
  USART_Init(USART2,                //����USART2
            baudrate,               //����������
            USART_WordLength_8b,    //���ݳ�����Ϊ8λ
            USART_StopBits_1,       //1λֹͣλ
            USART_Parity_No,        //��У��
            (USART_Mode_TypeDef)(USART_Mode_Rx | USART_Mode_Tx));  //����Ϊ���ͽ���˫ģʽ
  //ʹ�ܽ����ж�
  USART_ITConfig(USART2, USART_IT_RXNE , ENABLE);
  USART_Cmd(USART2 , ENABLE);   //ʹ��USART2ģ��
}
             
////////////////////////////  ֡��� ///////////////////////////////////////////  
void Bsp_Uart_Data(void)
{
    if(System_Flag.flag_frame_time_en)
    {
       Frame_Time_Cnt++;
       if(Frame_Time_Cnt>=3) //��֡����ʱ�������3*100ms�������ж�Frame_Time_Cnt==1���п�������UART�ж��иոս��յ�0x68->Frame_Time_EN=1�����������յ�0x16����ͽ����˴˴�����������Frame_Time_Cnt=1������UART_RxByteCnt=0���������½��յ������ַ���WIRED_COMM_Rx_Buff[0]��ʼ�������´���
       {                        //2400/3.3/10=72.2���ֽڣ�����300ms�ڿɽ���72.2���ֽڣ�/3.3->300ms,,/10->8+��ʼ+ֹͣ��
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

