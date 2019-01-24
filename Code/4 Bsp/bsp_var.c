#include "bsp_var.h"

NO_INIT System_Flag_TypeDef System_Flag;
NO_INIT TASK_State_TypeDef TASK_State;
u16 hour_time_count;
u16 min_time_count;
u16 remaining_time_count;
u16 outage_time_count;

void BSP_Task_Flag_Init(void)
{
	TASK_State.task_measure_usb_insert_flag  = 0;
	TASK_State.task_measure_usb_pullout_flag = 0;
	TASK_State.task_measure_usb_zero_flag    = 0;
    TASK_State.task_control_charge_open_flag = 0;
    TASK_State.task_close_sta_proc_flag = DISABLE;
    TASK_State.task_at_reday_flag = DISABLE;
}

static void G510_Var_Init(void)
{
    Var_Fill(g_stGPRS_Handler,0);
    gprs_status = GPRS_CLOSE;
    mode_status = MODE_NULL;
    //Var_Fill(PacketHead,0);
    //Net_Par_Config();
}

static void SYS_Mess_Ver_Init(void)
{
    u8 i=0;
    u8 software_var[4] = TERMINAL_SOFTWARE_VER;
    u8 hardware_var[4] = TERMINAL_HARDWARE_VER;
    for(i=0;i<4;i++)
    {
         SYS_HardVerMess[i] = hardware_var[i];
    }
    for(i=0;i<4;i++)
    {
         SYS_SoftVerMess[i] = software_var[i];
    }
    SYS_HardVerMessLen = 4;
    SYS_SoftVerMessLen = 4;
}

void Bsp_SYS_Var_Init(void)
{
	if(System_Flag.flag_need_charge ==1)
	{
		TASK_State.task_control_charge_open_flag = 1;
	}
	else
	{
		TASK_State.task_control_charge_open_flag = 0;
	}
    
    G510_Var_Init();
    SYS_Mess_Ver_Init();
	hour_time_count = 0;
	min_time_count  = 0;
    //remaining_time_count = 0;
	System_Flag.flag_usb_insert  = 0;
	System_Flag.flag_usb_pullout = 0;
    System_Flag.flag_pc_first_word = 1;
    
#if DEBUG_CHARGE
    BSP_LOG("/** Var task_measure_usb_insert_flag: ");
    BSP_LOG_Value(TASK_State.task_measure_usb_insert_flag);
    BSP_LOG(" **/\n\n");
    BSP_LOG("/** Var task_measure_usb_pullout_flag: ");
    BSP_LOG_Value(TASK_State.task_measure_usb_pullout_flag);
    BSP_LOG(" **/\n\n");
#endif  
}
