#ifndef __TASK_COMMUNICATE_H
#define __TASK_COMMUNICATE_H

#include "global.h"



extern u8 UART_RxByteCnt; 
extern u8 Frame_Time_Cnt;

u8 COMM_Task(u8 prio);
#endif