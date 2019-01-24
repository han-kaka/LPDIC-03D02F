#ifndef __APP_COMM_PC_H
#define __APP_COMM_PC_H

#include "global.h"

#define EQUIPMENT_TYPE 0x02

typedef struct
{
    u8      MeterId[7];       //±Ìµÿ÷∑
    char    MAC[12];
    char    ICCID[20];
}SYS_ManufStruct;

extern u8 Broadcast_Flag;
extern SYS_ManufStruct     SYS_Manuf;

u8 PC_Decode(u8 *buff);
void PC_Action(u8 *buff);
u8 PC_Return(u8 *data_in, u8 *data_out, u16 length);
u8 Sum_Check(u8 *data,u8 length);
u8 Sum(u8 *data,u8 length);
void Return_Network(void);
#endif