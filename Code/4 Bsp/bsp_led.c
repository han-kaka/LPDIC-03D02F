#include "bsp_led.h"
#include "bsp_gpio.h"

void LED_Init(void)
{
    LED_ALL_ON();
    Tim3_Delay_Ms(1000);
    LED_ALL_OFF();
}

void LED_ALL_ON(void)
{
    GPIO_ResetBits(LED_PORT,LED_PIN);
}

void LED_ALL_OFF(void)
{
    GPIO_SetBits(LED_PORT,LED_PIN);
}

void LED_BATTERY_INDICATION(u8 mode_status)
{
	static u8 t_battery_grade = MODE_NULL;
    static u8 time_networking_cnt = 0;
    
	if(t_battery_grade != mode_status)
	{
		BATTERY_LED_INIT();
		t_battery_grade = mode_status;
	}
    
    switch(mode_status)
    {
        case MODE_NULL:
            GPIO_ResetBits(LED_PORT,LED_PIN);
            break;
        case MODE_CHARGEING:
            GPIO_ResetBits(LED_PORT,LED_PIN);
            break;
        case MODE_NETWORKING:
            time_networking_cnt++;
            if(time_networking_cnt>10)
            {
                time_networking_cnt = 0;
                GPIO_ToggleBits(LED_PORT,LED_PIN);
            }
            break;
        case MODE_NO_CHARGE:
            GPIO_SetBits(LED_PORT,LED_PIN);
            break;
        case MODE_OTA:
          
            break;   
        default:
            break;
    }
}