#include "5 task_measure.h"

u8 Measure_Task(u8 prio)
{
    u8 m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio])
    {
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        {
			case MEASURE_USB_ZERO:
				{
					ADC_USB_Zero_Calibration(0);
					if(TASK_State.task_measure_usb_zero_flag == 1)
					{
					  	TASK_State.task_measure_usb_zero_flag = 0;
						Set_Task(CONTROL,CONTROL_CHARGE_CLOSE);
					}
			  	}
		  		break;
            case MEASURE_USB_INSERT:
                {
                    /** 判断USB拔出时间是否大于60S **/
					if(min_time_count >= outage_time_count)
			  		{
						u8 data;
                        remaining_time_count = 0;
                        outage_time_count = 0;
                        Write_Data((u8*)(&remaining_time_count),ADDR_REMAINING_TIME,2);
                        Write_Data((u8*)(&outage_time_count),ADDR_OUTAGE_TIME,2); 
                        Set_Task(CONTROL,CONTROL_CHARGE_CLOSE);
					  	TASK_State.task_measure_usb_zero_flag = 1;
						TASK_State.task_measure_usb_insert_flag = 0;
						TASK_State.task_measure_usb_pullout_flag = 0;
						System_Flag.flag_need_charge = 0;
                        mode_status = MODE_NO_CHARGE;
						data = 0x00;
						Write_Data(&data, ADDR_CHARGE, 1);
						GPRS_Comm_Event_Set(COMM_Event_REPORTED);
                        
					}
					else
						ADC_USB_Insert_Check();     //检测USB是否插入
                }
                break;
			case MEASURE_USB_PULLOUT:
			 	{
                    /** 判断USB充电时间是否大于3600S **/
					if(hour_time_count >= remaining_time_count)
			  		{
						u8 data;
                        remaining_time_count = 0;
                        outage_time_count = 0;
                        Write_Data((u8*)(&remaining_time_count),ADDR_REMAINING_TIME,2);
                        Write_Data((u8*)(&outage_time_count),ADDR_OUTAGE_TIME,2); 
						Set_Task(CONTROL,CONTROL_CHARGE_CLOSE);
						TASK_State.task_measure_usb_insert_flag = 0;
						TASK_State.task_measure_usb_pullout_flag = 0;
						System_Flag.flag_need_charge = 0;
                        mode_status = MODE_NO_CHARGE;
						data = 0x00;
						Write_Data(&data,ADDR_CHARGE,1);
					}
					else
				  		ADC_USB_Pullout_Check();    //检测USB是否拔出
			  	}
		  		break;
            default:
                break;
        }
        if(clear_task(prio,m_SYS_SubTask_prio) == TRUE)
        {
            return TRUE;
        }
    }
    return FALSE;
}
