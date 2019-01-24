#ifndef __COMMON_TASK_H
#define __COMMON_TASK_H

#include "global.h"

#define TASK0                         SG_Task
#define TASK1                         Contrl_Task
#define TASK2                         MEM_Read_Task
#define TASK3                         MEM_Write_Task
#define TASK4                         Measure_Task
#define TASK5                         COMM_Task
#define TASK6                         Net_Conn_Task
#define TASK7                         Interaction_Task

#define SG                            0     //系统主任务0

#define CONTROL                       1
#define CONTROL_CHARGE_OPEN           0
#define CONTROL_CHARGE_CLOSE          1

#define MEM_READ                      2      //主任务-存储数据读任务
#define MEM_FIRST_POR_DATA_RECOVER    0
#define MEM_SYS_PAR_INFO_RECOVER      1
#define MEM_VALVE_STATE_RECOVER       2
#define MEM_RUN_STATE_RECOVER         3
#define MEM_SYS_TEMP_AND_HUD_RECOVER  4
#define MEM_SYS_FAULT_STATE_RECOVER   5
#define MEM_SYS_RTC_RECOVER           6

#define MEM_WRITE                     3      //主任务-存储数据写任务
#define MEM_STORE_SOLID_ROMDATA       0

#define MEASURE                       4       //wangw140928主任务-测量任务
#define MEASURE_USB_ZERO			  0
#define MEASURE_USB_INSERT            1
#define MEASURE_USB_PULLOUT			  2

#define COMM                          5                                 //系统主任务1
#define COMM_CMD_PROC                 0
#define COMM_STATE_PROC               1
#define COMM_DECODE                   2
#define COMM_SSLFLIE                  3
#define COMM_PC_DECODE                4
#define COMM_PC_ACTION		          5
#define COMM_PC_RETURN  		      6


#define NET_CONN                      6
#define NET_NETWORK_CONN              0
#define NET_SOCKET_CONN               1

#define INTERACTION                   7       //主任务-其它任务
#define LED_TIPS                      0       //LED指示灯控制子任务


//#define SOCKET_CONN                  4

typedef struct
{
    u8 (*function)(u8 m_Event);                                     //带参数的函数指针 用于执行任务函数
}Task_Type;

extern Task_Type  Task_Struct[8];
extern const u8  ga_TaskMapTable[256];


#endif


