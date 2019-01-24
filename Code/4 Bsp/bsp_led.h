#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "global.h"


#define  BATTERY_LED_INIT()       	 do\
                                     {\
                                        GPIO_WriteBit(LED_PORT ,\
                                                      LED_PIN ,\
                                                      SET);\
                                     }while(0);


void LED_Init(void);
void LED_ALL_ON(void);
void LED_ALL_OFF(void);
void LED_BATTERY_INDICATION(u8 battery_grade);

#endif
