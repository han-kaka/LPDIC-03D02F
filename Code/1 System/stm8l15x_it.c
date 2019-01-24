/**
  ******************************************************************************
  * @file    Project/STM8L15x_StdPeriph_Template/stm8l15x_it.c
  * @author  MCD Application Team
  * @version V1.6.1
  * @date    30-September-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x_it.h"
#include "global.h"
#include "bsp_led.h"
/** @addtogroup STM8L15x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/

#ifdef _COSMIC_
/**
  * @brief Dummy interrupt routine
  * @par Parameters:
  * None
  * @retval
  * None
*/

INTERRUPT_HANDLER(NonHandledInterrupt,0)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
#endif

/**
  * @brief TRAP interrupt routine
  * @par Parameters:
  * None
  * @retval
  * None
*/
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief FLASH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(FLASH_IRQHandler,1)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief DMA1 channel0 and channel1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL0_1_IRQHandler,2)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief DMA1 channel2 and channel3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(DMA1_CHANNEL2_3_IRQHandler,3)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief RTC / CSS_LSE Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(RTC_CSSLSE_IRQHandler,4)
{
    if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
    {
      
    }
}
/**
  * @brief External IT PORTE/F and PVD Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIE_F_PVD_IRQHandler,5)
{
     if(EXTI_GetITStatus(EXTI_IT_PortE) !=RESET)
     {
           EXTI_ClearITPendingBit(EXTI_IT_PortE);
     }
}

/**
  * @brief External IT PORTB / PORTG Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTIB_G_IRQHandler,6)
{
    if(EXTI_GetITStatus(EXTI_IT_PortB) !=RESET)
    {
        EXTI_ClearITPendingBit(EXTI_IT_PortB);
    }
}

/**
  * @brief External IT PORTD /PORTH Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTID_H_IRQHandler,7)
{
    if(EXTI_GetITStatus(EXTI_IT_PortD) !=RESET)
    {
        EXTI_ClearITPendingBit(EXTI_IT_PortD);
    }
}

/**
  * @brief External IT PIN0 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI0_IRQHandler,8)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI1_IRQHandler,9)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI2_IRQHandler,10)
{
   if(EXTI_GetITStatus(EXTI_IT_Pin2)!= RESET)
   {
        EXTI_ClearITPendingBit(EXTI_IT_Pin2);  //清除标志位
   }
}

/**
  * @brief External IT PIN3 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI3_IRQHandler,11)
{

   if(EXTI_GetITStatus(EXTI_IT_Pin3)!= RESET)
   {
        EXTI_ClearITPendingBit(EXTI_IT_Pin3);  //清除标志位
   }

}

/**
  * @brief External IT PIN4 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI4_IRQHandler,12)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN5 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI5_IRQHandler,13)
{
    if(EXTI_GetITStatus(EXTI_IT_Pin5)!= RESET)
    {
//        EXTI_ClearITPendingBit(EXTI_IT_Pin5);  //清除标志位
    }
}

/**
  * @brief External IT PIN6 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI6_IRQHandler,14)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief External IT PIN7 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI7_IRQHandler,15)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
	if(EXTI_GetITStatus(EXTI_IT_Pin7)!= RESET)
    {
        EXTI_ClearITPendingBit(EXTI_IT_Pin7);  //清除标志位
        BSP_LOG(" EXTI7 \r\n");
    }
}
/**
  * @brief LCD /AES Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(LCD_AES_IRQHandler,16)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief CLK switch/CSS/TIM1 break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SWITCH_CSS_BREAK_DAC_IRQHandler,17)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief ADC1/Comparator Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(ADC1_COMP_IRQHandler,18)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
      if(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)!= RESET)
      {
          ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
      }
}

/**
  * @brief TIM2 Update/Overflow/Trigger/Break /USART2 TX Interrupt routine.
  * @param  None
  * @retval None
  */
Timeout_uart_t timeout_uart1 =
{
	.ms_count   = 0,
	.status     = timeout_true,
	.buff_point = 0,
};

Timeout_uart_t timeout_uart2 =
{
	.ms_count   = 0,
	.status     = timeout_true,
	.buff_point = 0,
};

INTERRUPT_HANDLER(TIM2_UPD_OVF_TRG_BRK_USART2_TX_IRQHandler,19)
{
    if(TIM2_GetFlagStatus(TIM2_FLAG_Update) != RESET)
    {
    	TIM2_ClearITPendingBit(TIM2_IT_Update);
        s_MsCount++;
        
        if(TASK_State.task_close_sta_proc_flag == DISABLE)
        {
                if(System_Flag.flag_ymode_file == 1)
                {
                    APP_GPRS_TimeProc(10);
                }
        }
        
        if(s_MsCount >= MS_TO_S)
        {
            s_MsCount =0;
            Bsp_Time_Update();
        }
        
        if(!timeout_uart1.status)
        {
            timeout_uart1.ms_count++;
            if(timeout_uart1.ms_count > UART_RECIEVE_TIMEOUT)
            {
                timeout_uart1.status = timeout_true;
                timeout_uart1.ms_count   = 0;
                timeout_uart1.buff_point = 0;
                System_Flag.flag_pc_first_word = 1;
                //Command_Send_G510(RX_BUFF,strlen(RX_BUFF));
                //BSP_LOG((char*)RX_BUFF);
                //memset(RX_BUFF,'\0',sizeof(RX_BUFF));
                Set_Task(COMM,COMM_SSLFLIE);
            }
        }
    }
}

/**
  * @brief Timer2 Capture/Compare / USART2 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM2_CC_USART2_RX_IRQHandler,20)
{
    if(USART_GetFlagStatus(USART2, USART_FLAG_OR) == SET)
    {
            USART_ReceiveData8(USART2); 
            USART_ClearFlag(USART2, USART_FLAG_OR); 
    }
  
    if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
     {
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);   //清除中断标志位
         
         GPRS_ATCmdCB.Busy = 1;                           

          GPRS_ATCmdCB.RxBuf[GPRS_ATCmdCB.ucRxCnt++] = USART_ReceiveData8(USART2);
          //Uart1_SendData(GPRS_ATCmdCB.RxBuf[GPRS_ATCmdCB.ucRxCnt-1]);
          if(GPRS_ATCmdCB.ucRxCnt >= GPRS_RX_BUFF_LEN)
          {
               GPRS_ATCmdCB.ucRxCnt = 250;
          }
          GPRS_ATCmdCB.ucByteTimeOut = 500;
	 }
}


/**
  * @brief Timer3 Update/Overflow/Trigger/Break Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_UPD_OVF_TRG_BRK_USART3_TX_IRQHandler,21)
{
    if(TIM3_GetITStatus(TIM3_IT_Update) != RESET)
    {
      TIM3_ClearITPendingBit(TIM3_IT_Update); //清除中断标志


   }
}
/**
  * @brief Timer3 Capture/Compare /USART3 RX Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM3_CC_USART3_RX_IRQHandler,22)
{
  /*
    u8 Res;
    if(USART_GetITStatus(USART3,USART_IT_RXNE) == SET)  //接收中断
     {
            USART_ClearITPendingBit(USART3,USART_IT_RXNE);
            Res = USART_ReceiveData8(USART3); 
//            USART_SendData8(USART3, Res);   
            timeout_uart1.ms_count = 0;
       		 if(!timeout_uart1.status)
       		 {
            		RX_BUFF[timeout_uart1.buff_point++] = Res;
       		 }
      		  else
      		 {
        		    RX_BUFF[timeout_uart1.buff_point++] = Res;
        		    timeout_uart1.status = timeout_false;
       		 }
     }
*/
}
/**
  * @brief TIM1 Update/Overflow/Trigger/Commutation Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_COM_IRQHandler,23)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @brief TIM1 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CC_IRQHandler,24)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief TIM4 Update/Overflow/Trigger Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM4_UPD_OVF_TRG_IRQHandler,25)
{
/*
    if(TIM4_GetFlagStatus(TIM4_FLAG_Update) != RESET)
    {
        
    }
*/
}
/**
  * @brief SPI1 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI1_IRQHandler,26)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}

/**
  * @brief USART1 TX / TIM5 Update/Overflow/Trigger/Break Interrupt  routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(USART1_TX_TIM5_UPD_OVF_TRG_BRK_IRQHandler,27)
{
    if(TIM5_GetITStatus(TIM5_IT_Update) != RESET)     //外部低速晶振启用时，精准延时1S的定时器
    {
      TIM5_ClearITPendingBit(TIM5_IT_Update); //清除中断标志
   }

}

/**
  * @brief USART1 RX / Timer5 Capture/Compare Interrupt routine.
  * @param  None
  * @retval None
  */

INTERRUPT_HANDLER(USART1_RX_TIM5_CC_IRQHandler,28)
{
    u8 Res;
    
    if(USART_GetFlagStatus(USART1, USART_FLAG_OR) == SET)
    {
            USART_ReceiveData8(USART1); 
            USART_ClearFlag(USART1, USART_FLAG_OR); 
    }
    
     if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
     {
            USART_ClearITPendingBit(USART1,USART_IT_RXNE);
            Res = USART_ReceiveData8(USART1); 

            if(System_Flag.flag_pc_first_word == 1)
            {
                if(Res == 0x68)
                {
                        System_Flag.flag_frame_time_en = 1;
                        timeout_uart1.status = timeout_true;
                        PC_RX_BUFF[UART_RxByteCnt] = Res;
                        UART_RxByteCnt++;
                        System_Flag.flag_pc_first_word = 0;
                }
                else if((Res == 0xAB)||(Res == 0x01)||(Res == 0x02))
                {
                        System_Flag.flag_frame_time_en = 0;
                        timeout_uart1.status = timeout_false;
                        PC_RX_BUFF[timeout_uart1.buff_point++] = Res;
                        timeout_uart1.ms_count = 0;
                        System_Flag.flag_pc_first_word = 0;
                }
            }
            else
            {
                  if(System_Flag.flag_frame_time_en == 1)
                  {
                        PC_RX_BUFF[UART_RxByteCnt] = Res;
                        UART_RxByteCnt++;
                        if( UART_RxByteCnt > 10)
                        {
                                if( (UART_RxByteCnt == PC_RX_BUFF[10]+13) && (Res == 0x16) )   //COMM_Rx_Buff[10]憷?诉?峪懋?，13=1+1+7+1+1+1（cs）+1(16H)
                                {
                                    Set_Task(COMM, COMM_PC_DECODE); 
                                    UART_RxByteCnt = 0;
                                    System_Flag.flag_frame_time_en=0;
                                    Frame_Time_Cnt=0;
                                    System_Flag.flag_pc_first_word = 1;
                                }
                        }
                  }
                  else if(!timeout_uart1.status)
                  {
                        PC_RX_BUFF[timeout_uart1.buff_point++] = Res;
                        timeout_uart1.ms_count = 0;
                  }
            }      
          /*
//            USART_SendData8(USART3, Res);   
            timeout_uart1.ms_count = 0;
       		 if(!timeout_uart1.status)
       		 {
            		PC_RX_BUFF[timeout_uart1.buff_point++] = Res;
       		 }
      		  else
      		 {
        		    PC_RX_BUFF[timeout_uart1.buff_point++] = Res;
        		    timeout_uart1.status = timeout_false;
       		 }
          */
     }
}

/**
  * @brief I2C1 / SPI2 Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C1_SPI2_IRQHandler,29)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
}
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/