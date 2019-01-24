#ifndef __BSP_UART_H
#define __BSP_UART_H

#include "global.h"

#define PC_BUFFSIZE                        270
#define G510_BUFFSIZE                      256

extern u8 G510_RX_BUFF[G510_BUFFSIZE];
extern u8 G510_TX_BUFF[G510_BUFFSIZE];

extern u8 PC_RX_BUFF[PC_BUFFSIZE];
extern u8 PC_TX_BUFF[PC_BUFFSIZE+FE_NUM];


void UART1_Init(u32 baudrate);
void BSP_LOG(char *data );
void BSP_LOG_Value(s32 data);
void UART1_Init(u32 baudrate);
void UART2_Init(u32 baudrate);
void UART3_Init(u32 baudrate);
void Uart1_SendData(u8  data);
void Uart2_SendData(u8  data);
void Uart3_SendData(u8  data);
void uart_rx_complete_callback(uint8_t *p_buf,uint16_t len);
void Disable_Timer_G510_Ack_Timeout(void);

void Bsp_Uart_Data(void);

void BSP_UART_Transmit(U8* pTxBuff, U8 usLen);
#endif