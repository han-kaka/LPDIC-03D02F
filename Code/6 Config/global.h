/******************************************************************
** ��Ȩ:       �������ĵ��Ƽ����޹�˾
** �ļ���:     global.h
** �汾��
** ��������:   IAR 1.42
** ����:       li
** ��������:   2017.8.18
** ����:       ���ļ���Ҫ���ڽṹ�嶨�壬������ĵĺ궨���Լ��ļ�����
** ����ļ�:
** �޸���־:
*******************************************************************/
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "main.h"

#define timeout_true     1
#define timeout_false    0

/********************  Task Layer  *******************/
extern    u8        g_Maintask;        //ϵͳ������
extern    u8        ga_Subtask[8];     //ϵͳ������

#define Var_Fill(var,data)      for(u16 ii = 0; ii < sizeof(var); ++ii)\
                                *(((u8*)(&var))+ii) = data              //�����var�����data

#define NumberOfElements(x) (sizeof(x)/sizeof((x)[0]))
#define TmrSecondsToMilliseconds( n )   ( (U32) ((n) * 1000) )                                  
                                  
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))
                                  



typedef struct
{
	u8 ms_count;
	u8 status;
	u16 buff_point;
}Timeout_uart_t;



extern u32 adc_usb_zero_value ;      // usb zero calibration
#endif



