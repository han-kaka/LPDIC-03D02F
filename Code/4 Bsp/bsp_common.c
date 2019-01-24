#include "bsp_common.h"
#include "bsp_led.h"

static void Bsp_Peripheral_Init(void)
{
    Bsp_GPIO_Init();
    BSP_Task_Flag_Init();
	Tim2_Init();
	Tim3_Delay_Init();
    LED_Init();
    UART2_Init(115200);
    UART1_Init(115200);

#if DEBUG
	BSP_LOG("/** System Start！**/\n\n");
#endif 
}


u8 Com_Bsp_Init(void)
{
    disableInterrupts();   //close system interrupts

    Bsp_Rcc_Init();
	Bsp_Peripheral_Init();

    enableInterrupts();    //open system interrupts

	Bsp_MEM_Init();        // EEPROM 数据初始化
	Bsp_SYS_Var_Init();    // 变量初始化
    //Bsp_G510_Init();
    APP_COMM_Init();       //COMM state and event init
    IWDG_Init();
    
    return TRUE;
}
