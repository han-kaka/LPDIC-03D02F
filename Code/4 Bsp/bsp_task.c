#include "bsp_task.h"

static u8 backup_sec = 0;
u8 key_interval_count = 0;
void Bsp_Task_Control(void)
{
  /** 每100ms检测按键 **/ 
  
  if(TASK_State.task_control_charge_open_flag == 1)
  {
      Set_Task(CONTROL,CONTROL_CHARGE_OPEN);
  }
  if(TASK_State.task_measure_usb_zero_flag == 1)
  {
      Set_Task(MEASURE,MEASURE_USB_ZERO);
  }

  if(backup_sec != Time_Count.count_sec)
  {
      /** 充电过程中USB插入拔出检测 周期一秒**/
      if(TASK_State.task_measure_usb_insert_flag == 1)
      {
          Set_Task(MEASURE,MEASURE_USB_INSERT);
      }
      else if (TASK_State.task_measure_usb_pullout_flag == 1)
      {
          Set_Task(MEASURE,MEASURE_USB_PULLOUT);
      }
      else
      {
          
      }

      /** 充电 小时/分钟 计时 **/
      if(System_Flag.flag_usb_insert == 1)
      {
          hour_time_count++;
      }
      else if (System_Flag.flag_usb_pullout == 1)
      {
          min_time_count++;
      }
      else
      {
    
      }
  
      backup_sec = Time_Count.count_sec;
  }
}
