#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include "global.h"

#define CHECK_NET_TIME         4

#define MS_TO_S                 (10)
#define RE_OPEN_STA_PROC_TIME   (300UL)

typedef struct 
{
    //uint16_t task_fail_again_count;		
    //uint16_t gps_data_check_count;
    //uint8_t  second_count;
    //uint16_t trs_time_count;
    uint16_t receive_count;
    //uint32_t trs_sear_gps_count;
    //uint16_t trs_heartbeat_count;
    //uint16_t trs_reg_timeout_count;
    //uint16_t trs_data_timeout_count;
    //uint16_t trs_net_timeout_count;		
}Tim_1s_Type;

typedef struct
{
   u8  count_100ms;
   u8  count_sec;
   u8  count_min;
   u8  count_net_check;
   u8  count_heartbeat;
   U16 reOpenStaProcCount;
}Time_Count_TypeDef;

extern Time_Count_TypeDef Time_Count;
extern Tim_1s_Type Tim_1s_Struct;
extern U16 s_MsCount;

void Tim2_Init(void);
void Tim2_Start(void);
void Tim3_Init(void);
void Tim3_Delay_Init(void);
void Tim3_Delay_Ms(u16 ms);
void Tim5_Init(void);
void Sys_RTC(void);
void Bsp_Time_Update(void);
#endif







