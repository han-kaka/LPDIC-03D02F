/******************************************************************
** 版权:       杭州利拍档科技有限公司
** 文件名:     global.h
** 版本：
** 工作环境:   IAR 1.42
** 作者:       li
** 生成日期:   2017.8.18
** 功能:       本文件主要用于结构体定义，不需更改的宏定义以及文件包含
** 相关文件:
** 修改日志:
*******************************************************************/
#ifndef __GLOBAL_H
#define __GLOBAL_H

#include "main.h"

#define timeout_true     1
#define timeout_false    0

/********************  Task Layer  *******************/
extern    u8        g_Maintask;        //系统主任务
extern    u8        ga_Subtask[8];     //系统子任务

#define Var_Fill(var,data)      for(u16 ii = 0; ii < sizeof(var); ++ii)\
                                *(((u8*)(&var))+ii) = data              //向变量var中填充data

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



