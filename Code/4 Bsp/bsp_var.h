#ifndef  __BSP_VAR_H
#define  __BSP_VAR_H

#include "global.h"

typedef struct
{
   u8      flag_need_charge       :1;
   u8      flag_usb_insert        :1;
   u8      flag_usb_pullout       :1;
   u8      flag_return_com_result :1;
   u8      flag_return_ota_result :1;
   u8      flag_ymode_file        :1;
   u8      flag_pc_first_word     :1;
   u8      flag_frame_time_en     :1;
   
   u8      flag_ymode_time_en     :1; 
}System_Flag_TypeDef;

typedef struct
{
   u8      task_control_charge_open_flag    :1;
   u8      task_interaction_key_flag    	:1;
   u8      task_measure_usb_insert_flag 	:1;
   u8      task_measure_usb_pullout_flag    :1;
   u8      task_measure_usb_zero_flag       :1;
   u8      task_sg_v220_detect_flag    	 	:1;
   u8      task_close_sta_proc_flag         :1;
   u8      task_at_reday_flag               :1;
}TASK_State_TypeDef;

extern NO_INIT TASK_State_TypeDef TASK_State;
extern NO_INIT System_Flag_TypeDef System_Flag;
extern u16 hour_time_count;
extern u16  min_time_count;
extern u16 remaining_time_count;
extern u16 outage_time_count;

void Bsp_SYS_Var_Init(void);
void BSP_Task_Flag_Init(void);
#endif
