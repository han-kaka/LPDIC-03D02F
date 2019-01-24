#include "6 task_communicate.h"

u8 UART_RxByteCnt; 
u8 Frame_Time_Cnt;

u8 COMM_Task(u8 prio)
{
    u8 m_SYS_SubTask_prio=0;
    while(ga_Subtask[prio])
    {   
        m_SYS_SubTask_prio = ga_TaskMapTable[ga_Subtask[prio]];
        switch(m_SYS_SubTask_prio)
        { 
            
            case COMM_CMD_PROC:            
            {
                APP_SubTask_CmdProc();
            }
                break;
            
            case COMM_STATE_PROC:            
            {
                APP_SubTask_StateProc();
            }
                break;

            case COMM_DECODE: 
            {
                APP_SuTask_DecodeProc();
            }
                break;
                
            case COMM_SSLFLIE:
            {
                #if DEBUG_YMODEM
                    BSP_LOG("\n COMM_SSLFLIE \n");
                #endif
                bsp_ymodem_trans(PC_RX_BUFF);          
               // memset(RX_BUFF,'\0',sizeof(RX_BUFF));
            }
                break;
                
            case COMM_PC_DECODE:                                                      //数据解码
            {
                if(TRUE == PC_Decode(PC_RX_BUFF))
                {
                    Set_Task(COMM,COMM_PC_ACTION);
                }
            }
                break;
    
            case COMM_PC_ACTION:
            {
                PC_Action(PC_RX_BUFF);   
                Set_Task(COMM, COMM_PC_RETURN);
                if(Broadcast_Flag==1)  //广播写时间命令则清除COMM_RETURN任务
                {   
                    Clr_Task(COMM, COMM_PC_RETURN);
                    Broadcast_Flag=0;
                }
            }
                break;
                
            case COMM_PC_RETURN:
            {
                u8 datalen;
                u8 i;
                datalen = PC_Return(PC_RX_BUFF,PC_TX_BUFF,PC_BUFFSIZE);
                if(datalen != FALSE)
                {
                    for(i = 0 ; i < datalen ; i++)
                    {
                        Uart1_SendData(PC_TX_BUFF[i]);
                    }
                }
            }
                break; 
                
            default:
                break;
         }
         if(clear_task(prio,m_SYS_SubTask_prio) == TRUE)
        {
            return TRUE;
        }
    }
    return FALSE;
}


