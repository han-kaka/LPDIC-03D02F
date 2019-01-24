#include "bsp_tim.h"

U16 s_MsCount=0;
u8 stateChange = 0;
Tim_1s_Type Tim_1s_Struct;

Time_Count_TypeDef Time_Count=
{
    .count_100ms  = 0,
    .count_sec = 0,
    .count_min = 0,
    .count_net_check = 0,
    .count_heartbeat = 0, 
    .reOpenStaProcCount = 0,
};

void Tim5_Init(void)
{
 	CLK_PeripheralClockConfig(CLK_Peripheral_TIM5 , ENABLE);              //ʹ�ܶ�ʱ��5ʱ��
  	TIM5_TimeBaseInit(TIM5_Prescaler_128 , TIM5_CounterMode_Up , 16000);    //���ö�ʱ��5Ϊ128��Ƶ�����ϼ���������ֵΪ16000��Ϊ7.8125��ļ���ֵ
  	TIM5_ITConfig(TIM5_IT_Update , ENABLE);     //ʹ�����ϼ�������ж�
  	TIM5_ARRPreloadConfig(ENABLE);  //ʹ�ܶ�ʱ��5�Զ����ع���
  	TIM5_Cmd(ENABLE);               //������ʱ��5��ʼ����
}

void Tim2_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2 , ENABLE);              //ʹ�ܶ�ʱ��2ʱ��
    TIM2_TimeBaseInit(TIM2_Prescaler_16 , TIM2_CounterMode_Up , 10000);    //���ö�ʱ��2Ϊ16��Ƶ�����ϼ���������ֵΪ10000��Ϊ10����ļ���ֵ
    TIM2_ITConfig(TIM2_IT_Update , ENABLE);     //ʹ�����ϼ�������ж�
    TIM2_ARRPreloadConfig(ENABLE);  //ʹ�ܶ�ʱ��2�Զ����ع���
    TIM2_Cmd(ENABLE);               //������ʱ��2��ʼ����
}

void Tim2_Start(void)
{
    TIM2_Cmd(ENABLE);               //������ʱ��2��ʼ����
}

void Tim3_Delay_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3 , ENABLE);              //ʹ�ܶ�ʱ��3ʱ��
    TIM3_TimeBaseInit(TIM3_Prescaler_1 , TIM3_CounterMode_Up , 16000);    //���ö�ʱ��3Ϊ1��Ƶ�����ϼ���������ֵΪ16000��Ϊ1����ļ���ֵ
}

void Tim3_Delay_Ms(u16 ms)
{
    TIM3_ARRPreloadConfig(ENABLE);  //ʹ�ܶ�ʱ��2�Զ����ع���
    TIM3_Cmd(ENABLE);               //������ʱ��2��ʼ����
    while(ms--)
    {
        while( TIM3_GetFlagStatus(TIM3_FLAG_Update) == RESET); //�ȴ������Ƿ�ﵽ1����
        TIM3_ClearFlag(TIM3_FLAG_Update);  //�������1���룬�����Ӧ�ı�־
    }
    TIM3_Cmd(DISABLE);                   //��ʱȫ���������رն�ʱ��2
}

void Bsp_Time_Update(void)
{

    if(Time_Count.count_100ms <9)
    {
        Time_Count.count_100ms++;
        Set_Task(INTERACTION,LED_TIPS);
    }
    else
    {      
        Time_Count.count_100ms = 0;
        
        #if 0
        Time_Count.count_heartbeat++;
        if(Time_Count.count_heartbeat >= 60)
        {
            Time_Count.count_heartbeat = 0;
            if(System_Flag.flag_ymode_file == 1)
            {
                if(TASK_State.task_close_sta_proc_flag == DISABLE)
                {
                    if((System_Flag.flag_need_charge == 1) || (stateChange == 1))
                    {
                        if(System_Flag.flag_need_charge == 1)
                        {
                            stateChange = 1;
                        }
                        else
                        {
                            stateChange = 0;
                        }
                        GPRS_Comm_Event_Set(COMM_Event_DEV);
                        Set_Task(COMM, COMM_STATE_PROC);     /*����GPRS״̬��������*/
                    }
                }
                

            }
        }
        #endif
        if(Time_Count.count_sec < 59)
        {
            Time_Count.count_sec++;
        }
        else
        {
            Time_Count.count_sec = 0;
            Time_Count.count_min++;
            Time_Count.count_net_check++;
        #if 1  
            if(System_Flag.flag_ymode_file == 1)
            {
                if(TASK_State.task_close_sta_proc_flag == DISABLE)
                {
                    if((System_Flag.flag_need_charge == 1) || (stateChange == 1))
                    {
                        if(System_Flag.flag_need_charge == 1)
                        {
                            stateChange = 1;
                        }
                        else
                        {
                            stateChange = 0;
                        }
                        GPRS_Comm_Event_Set(COMM_Event_DEV);
                        Set_Task(COMM, COMM_STATE_PROC);     /*����GPRS״̬��������*/
                    }
                }
            }
        #endif
        }
        
        if(TASK_State.task_close_sta_proc_flag != DISABLE) //���´�GPRS״̬�������
        {
              Time_Count.reOpenStaProcCount++;
              if(Time_Count.reOpenStaProcCount >= RE_OPEN_STA_PROC_TIME)
              {
                   Time_Count.reOpenStaProcCount = 0;
                   if(System_Flag.flag_ymode_file == 1)
                   {
                        TASK_State.task_close_sta_proc_flag = DISABLE;
                        G510_Power_EN();
                   }
              }
        }
    }
    
    Bsp_Task_Control();
    Bsp_Uart_Data();
    // BSP_LOG("time");
}