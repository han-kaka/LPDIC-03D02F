#include "bsp_gpio.h"

/******** GPRS GPIO INIT ********************/
static void gpio_gprs_init(void)
{
    GPIO_Init(GPRS_BUCK_EN_PORT , GPRS_BUCK_EN_PIN , GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(GPRS_POWER_ON_PORT , GPRS_POWER_ON_PIN , GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(GPRS_WAKE_UP_PORT  , GPRS_WAKE_UP_PIN  , GPIO_Mode_Out_PP_Low_Fast);
    /* init status */
    GPIO_ResetBits(GPRS_BUCK_EN_PORT,GPRS_BUCK_EN_PIN);  
    GPIO_SetBits(GPRS_POWER_ON_PORT,GPRS_POWER_ON_PIN);  
    GPIO_SetBits(GPRS_WAKE_UP_PORT ,GPRS_WAKE_UP_PIN);  
}

static void gpio_led_init(void)
{
    GPIO_Init(LED_PORT , LED_PIN , GPIO_Mode_Out_PP_Low_Fast);
    
	/* init status : Set */
    GPIO_SetBits(LED_PORT,LED_PIN);  //µÕµÁ∆Ω¡¡
}

static void charge_control_gpio_init(void)
{
    GPIO_Init(CHARGE_CONTROL_PORT , CHARGE_CONTROL_PIN , GPIO_Mode_Out_PP_Low_Fast);
    
    /* init status : Reset */
    GPIO_ResetBits(CHARGE_CONTROL_PORT,CHARGE_CONTROL_PIN);
}

void Bsp_GPIO_Init(void)
{
    gpio_led_init();
    gpio_gprs_init();
    charge_control_gpio_init();
}

/***********  ADC GPIO INIT  ******************/
void USB_Sample_GPIO_Init(void)
{
    GPIO_Init(USB_DETECT_PORT , USB_DETECT_PIN , GPIO_Mode_In_FL_No_IT);
}
