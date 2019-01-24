#include "bsp_gprs.h"

void G510_Power_EN(void)
{
    GPIO_SetBits(GPRS_BUCK_EN_PORT,GPRS_BUCK_EN_PIN);
}

void G510_Power_DISEN(void)
{
    GPIO_ResetBits(GPRS_BUCK_EN_PORT,GPRS_BUCK_EN_PIN);
}

/*************************
    G510 Power on流程：
    ___         ___
       |_______|
         800ms
  电路转换 高->低；低->高
**************************/
void G510_Power_ON(void)
{
    GPIO_SetBits(GPRS_POWER_ON_PORT,GPRS_POWER_ON_PIN);
    Tim3_Delay_Ms(850);
    GPIO_ResetBits(GPRS_POWER_ON_PORT,GPRS_POWER_ON_PIN);
}

void Bsp_G510_Init(void)
{
    G510_Power_EN();
    G510_Power_ON();
}

/*************************
    G510 Power off流程：
    关闭所有应用接口和注销网络
    ___         ___
       |_______|
          3s
  电路转换 高->低；低->高
**************************/
void G510_Power_OFF(void)
{
    GPIO_SetBits(GPRS_POWER_ON_PORT,GPRS_POWER_ON_PIN);
    Tim3_Delay_Ms(3500);
    GPIO_ResetBits(GPRS_POWER_ON_PORT,GPRS_POWER_ON_PIN);
}

void Command_Send_G510(u8 *p_data,u16 len)
{
	u16 CmdSize = len;
	for(int i = 0;i<CmdSize;i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);   //判断发送数据寄存器是否为空
    	USART_SendData8(USART2,(u8)p_data[i]);
	}
}



