#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "global.h"

void ADC_USB_Zero_Calibration(u8 is_init);
void ADC_USB_Insert_Check(void);
void ADC_USB_Pullout_Check(void);
u32 ADC_Vref_Calculate(void);
u16 USB_Voltage_Value(void);
#endif