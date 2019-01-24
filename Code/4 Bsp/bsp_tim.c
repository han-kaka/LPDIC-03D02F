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
 	CLK_PeripheralClockConfig(CLK_Peripheral_TIM5 , ENABLE);              //使能定时器5时钟
  	TIM5_TimeBaseInit(TIM5_Prescaler_128 , TIM5_CounterMode_Up , 16000);    //设置定时器5为128分频，向上计数，计数值为16000即为7.8125秒的计数值
  	TIM5_ITConfig(TIM5_IT_Update , ENABLE);     //使能向上计数溢出中断
  	TIM5_ARRPreloadConfig(ENABLE);  //使能定时器5自动重载功能
  	TIM5_Cmd(ENABLE);               //启动定时器5开始计数
}

void Tim2_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM2 , ENABLE);              //使能定时器2时钟
    TIM2_TimeBaseInit(TIM2_Prescaler_16 , TIM2_CounterMode_Up , 10000);    //设置定时器2为16分频，向上计数，计数值为10000即为10毫秒的计数值
    TIM2_ITConfig(TIM2_IT_Update , ENABLE);     //使能向上计数溢出中断
    TIM2_ARRPreloadConfig(ENABLE);  //使能定时器2自动重载功能
    TIM2_Cmd(ENABLE);               //启动定时器2开始计数
}

void Tim2_Start(void)
{
    TIM2_Cmd(ENABLE);               //启动定时器2开始计数
}

void Tim3_Delay_Init(void)
{
    CLK_PeripheralClockConfig(CLK_Peripheral_TIM3 , ENABLE);              //使能定时器3时钟
    TIM3_TimeBaseInit(TIM3_Prescaler_1 , TIM3_CounterMode_Up , 16000);    //设置定时器3为1分频，向上计数，计数值为16000即为1毫秒的计数值
}

void Tim3_Delay_Ms(u16 ms)
{
    TIM3_ARRPreloadConfig(ENABLE);  //使能定时器2自动重载功能
    TIM3_Cmd(ENABLE);               //启动定时器2开始计数
    while(ms--)
    {
        while( TIM3_GetFlagStatus(TIM3_FLAG_Update) == RESET); //等待计数是否达到1毫秒
        TIM3_ClearFlag(TIM3_FLAG_Update);  //计数完成1毫秒，清除相应的标志
    }
    TIM3_Cmd(DISABLE);                   //延时全部结束，关闭定时器2
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
                        Set_Task(COMM, COMM_STATE_PROC);     /*启动GPRS状态处理任务*/
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
                        Set_Task(COMM, COMM_STATE_PROC);     /*启动GPRS状态处理任务*/
                    }
                }
            }
        #endif
        }
        
        if(TASK_State.task_close_sta_proc_flag != DISABLE) //重新打开GPRS状态处理计数
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