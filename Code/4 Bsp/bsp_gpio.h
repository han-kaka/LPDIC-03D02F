#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "global.h"

/******  LED GPIO  DEFINE  OUTPUT ******/
//#define  LED_PORT                          GPIOG
//#define  LED_PIN                           GPIO_Pin_4

#define  LED_PORT                          GPIOA
#define  LED_PIN                           GPIO_Pin_5

/******  ADC GPIO DEFINE INPUT ******/
#define  USB_DETECT_PORT						GPIOE
#define  USB_DETECT_PIN							GPIO_Pin_5

/******  CHARGE CONTROL GPIO DEFINE OUTPUT ******/
#define  CHARGE_CONTROL_PORT                    GPIOB
#define  CHARGE_CONTROL_PIN                     GPIO_Pin_1

#define  CHARGE_OPEN()                          do\
                                                {\
                                                  GPIO_SetBits(CHARGE_CONTROL_PORT,\
                                                                CHARGE_CONTROL_PIN);\
                                                }while(0);

#define  CHARGE_CLOSE()                         do\
                                                {\
                                                  GPIO_ResetBits(CHARGE_CONTROL_PORT,\
                                                                  CHARGE_CONTROL_PIN );\
                                                }while(0);
                                                
/*****  GPRS GPIO ******/                                          
#define  GPRS_BUCK_EN_PORT                     GPIOB
#define  GPRS_BUCK_EN_PIN                      GPIO_Pin_4
#define  BSP_GPRS_BUCKEN_SET                   GPIO_SetBits(GPRS_BUCK_EN_PORT ,GPRS_BUCK_EN_PIN);                                           
#define  BSP_GPRS_BUCKEN_RESET                 GPIO_ResetBits(GPRS_BUCK_EN_PORT ,GPRS_BUCK_EN_PIN);

#define  GPRS_POWER_ON_PORT                    GPIOD
#define  GPRS_POWER_ON_PIN                     GPIO_Pin_2
#define  BSP_GPRS_POWERON_SET                  GPIO_SetBits(GPRS_POWER_ON_PORT ,GPRS_POWER_ON_PIN);                                           
#define  BSP_GPRS_POWERON_RESET                GPIO_ResetBits(GPRS_POWER_ON_PORT ,GPRS_POWER_ON_PIN);
                                                
#define  GPRS_WAKE_UP_PORT                      GPIOB
#define  GPRS_WAKE_UP_PIN                       GPIO_Pin_6
                                                

                                                
void Bsp_GPIO_Init(void);
void USB_Sample_GPIO_Init(void);
#endif










