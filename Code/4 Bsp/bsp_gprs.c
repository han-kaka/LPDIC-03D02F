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
    G510 Power on���̣�
    ___         ___
       |_______|
         800ms
  ��·ת�� ��->�ͣ���->��
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
    G510 Power off���̣�
    �ر�����Ӧ�ýӿں�ע������
    ___         ___
       |_______|
          3s
  ��·ת�� ��->�ͣ���->��
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
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);   //�жϷ������ݼĴ����Ƿ�Ϊ��
    	USART_SendData8(USART2,(u8)p_data[i]);
	}
}



