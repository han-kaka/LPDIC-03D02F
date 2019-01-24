#include "8 task_interaction.h"
#include "bsp_led.h"

u8 mode_status;

u8 Interaction_Task(u8 prio)
{
    u8 m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio])
    {
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        {
            case LED_TIPS:
            {
                /** ģ��״ָ̬ʾ **/
                LED_BATTERY_INDICATION(mode_status);
            }
                break;
            
            default:
                break;
        }
        if(clear_task(prio,m_SYS_SubTask_prio) == TRUE)
        {
            return TRUE;
        }                                       //ÿִ����һ������ͷ���
    }
   return FALSE;
}
