#include "bsp_adc.h"

void Bsp_ADC_USB_Init(void)
{
	ADC_DeInit(ADC1);
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1 , ENABLE);//使能ADC1时钟
    USB_Sample_GPIO_Init();
    ADC_Init(ADC1,
             ADC_ConversionMode_Continuous,   //单次转换模式
             ADC_Resolution_12Bit,        //12位精度转换械
             ADC_Prescaler_2              //时钟设置为2分频
             );
    ADC_SamplingTimeConfig(ADC1 ,
                           ADC_Group_SlowChannels ,
                           ADC_SamplingTime_384Cycles
                           );
    ADC_SchmittTriggerConfig(ADC1 ,
                            ADC_Channel_23 ,
                            ENABLE
                            );
	ADC_ChannelCmd(ADC1,
                   ADC_Channel_23,         //设置为通道18进行采样
                   ENABLE);

    ADC_Cmd(ADC1 , ENABLE);               //使能ADC
}

static void ADC_VrefIN_Init(void)
{
	ADC_DeInit(ADC1);
    CLK_PeripheralClockConfig(CLK_Peripheral_ADC1 , ENABLE);
    ADC_VrefintCmd(ENABLE);
    ADC_Init(ADC1,
             ADC_ConversionMode_Continuous,   //单次转换模式
             ADC_Resolution_12Bit,        //12位精度转换械
             ADC_Prescaler_1              //时钟设置为2分频
             );
	ADC_SamplingTimeConfig(ADC1 ,
                           ADC_Group_FastChannels ,
                           ADC_SamplingTime_384Cycles
                           );

	//ADC_SchmittTriggerConfig(ADC1 ,
    //                        ADC_Channel_Vrefint ,
    //                        ENABLE
    //                       );
    ADC_ChannelCmd(ADC1,
                   ADC_Channel_Vrefint,   //设置为内部参考进行采样
                   ENABLE);

    ADC_Cmd(ADC1 , ENABLE);               //使能ADC
}

void bubble_sort(u32 *p_src)
{
  	u32 temp;
    for (int i = 0; i < 7; i++) // 8个数，8 - 1轮冒泡，每一轮都将当前最大的数推到最后
    {
        for (int j = 0; j < 7 - i; j++) // 7 - i，意思是每当经过一轮冒泡后，就减少一次比较
        {
            if (p_src[j] > p_src[j+1])
            {
                temp = p_src[j];
                p_src[j] = p_src[j+1];
                p_src[j+1] = temp;
             }
        }
    }
}

u32 Get_ADC_Value(void)
{
  	u32 adc_value_buff[8],adc_value = 0;
    for(int i = 0;i<8;i++)
    {
        ADC_SoftwareStartConv(ADC1);
        while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));//等待转换结束
        ADC_ClearFlag(ADC1,ADC_FLAG_EOC);//清除对应标志
        adc_value_buff[i]=ADC_GetConversionValue(ADC1); //获取转换值
    }
    bubble_sort(adc_value_buff);
    for(int i = 1;i<7;i++)
    {
        adc_value += adc_value_buff[i];
    }
    adc_value = adc_value/6;
	return adc_value;
}

u32  adc_usb_zero_value = 0;
//is_init = 1 while system init
void ADC_USB_Zero_Calibration(u8 is_init)
{
  	u32 adc_value;
  	Bsp_ADC_USB_Init();
	adc_value = Get_ADC_Value();
#if DEBUG_CHARGE
    BSP_LOG("/** USB ADC_Value_Zero: ");
    BSP_LOG_Value(adc_value);
    BSP_LOG(" **/\n\n");
#endif    
	if(adc_value <= ADC_VALUE_ZERO_MAX)
	{
	 	adc_usb_zero_value = adc_value;
	}
//	if(!is_init)
//	{
//		TASK_State.task_measure_usb_pullout_flag = 0;
//		hour_time_count = 0;
//	}
}

void ADC_USB_Insert_Check(void)
{
  	u32 adc_value;
  	Bsp_ADC_USB_Init();
	adc_value = Get_ADC_Value();
	if(((adc_value-adc_usb_zero_value) > ADC_VALUE_ZERO_MAX) &&
       (adc_value>adc_usb_zero_value))//检测到USB插入
	{       
	 	System_Flag.flag_usb_insert = 1;
		System_Flag.flag_usb_pullout = 0;
		TASK_State.task_measure_usb_insert_flag = 0;
		TASK_State.task_measure_usb_pullout_flag = 1;
		min_time_count = 0;
        #if DEBUG_CHARGE
            BSP_LOG("/** USB Insert Value: ");
            BSP_LOG_Value(adc_value);
            BSP_LOG(" **/\n\n");
        #endif
	}
	else
	{
		System_Flag.flag_usb_pullout = 1;
		System_Flag.flag_usb_insert = 0;
        #if DEBUG_CHARGE
            BSP_LOG("/** USB Pullout Value: ");
            BSP_LOG_Value(adc_value);
            BSP_LOG(" **/\n");
            BSP_LOG("/** min count: ");
            BSP_LOG_Value(min_time_count);
            BSP_LOG(" **/\n\n");
        #endif
	}
}

void ADC_USB_Pullout_Check(void)
{
  	u32 adc_value;
  	Bsp_ADC_USB_Init();
	adc_value = Get_ADC_Value();
	if(((adc_value-adc_usb_zero_value) <= ADC_VALUE_ZERO_MAX) ||
		(adc_value<adc_usb_zero_value))
	{
	 	System_Flag.flag_usb_pullout = 1;
		System_Flag.flag_usb_insert = 0;
		TASK_State.task_measure_usb_insert_flag = 1;
		TASK_State.task_measure_usb_pullout_flag = 0;
        #if DEBUG_CHARGE
            BSP_LOG("/** USB Pullout Value: ");
            BSP_LOG_Value(adc_value);
            BSP_LOG(" **/\n\n");
        #endif
	}
	else
	{
		System_Flag.flag_usb_insert = 1;
		System_Flag.flag_usb_pullout = 0;
        #if DEBUG_CHARGE
            BSP_LOG("/** USB Insert Value: ");
            BSP_LOG_Value(adc_value);
            BSP_LOG(" **/\n");
            BSP_LOG("/** hour count: ");
            BSP_LOG_Value(hour_time_count);
            BSP_LOG(" **/\n\n");
        #endif
	}
}

u32 ADC_Vref_Calculate(void)
{
    ADC_VrefIN_Init();
    u32 value = 0,adc_value = 0;
    adc_value = Get_ADC_Value();
    value=(u32)1225*(u32)4096/adc_value;
    return value;
}

u16 USB_Voltage_Value(void)
{
	u32 vref = ADC_Vref_Calculate();
	u16 voltage = 0;u32 adc_value;
	Bsp_ADC_USB_Init();
	adc_value = Get_ADC_Value();
	voltage = (adc_value * vref)/4096;
	return voltage;
}


