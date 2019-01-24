#include "2 task_control.h"

u8 Contrl_Task(u8 prio)
{
    u8 m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio])
    {
        m_SYS_SubTask_prio= ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        {
            case CONTROL_CHARGE_OPEN:
                {
                    CHARGE_OPEN();       //open charge
                    Set_Task(MEASURE,MEASURE_USB_ZERO);
                    hour_time_count = 0;
                    min_time_count  = 0;
                    TASK_State.task_measure_usb_insert_flag = 1;
                    TASK_State.task_control_charge_open_flag = 0;
                }
                break;
            case CONTROL_CHARGE_CLOSE:
                {
                    CHARGE_CLOSE();      //close charge
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
