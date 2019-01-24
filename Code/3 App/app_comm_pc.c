#include "app_comm_pc.h"

u8 Broadcast_Flag;

SYS_ManufStruct     SYS_Manuf = {.MeterId={0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

u8 PC_Decode(u8 *buff)
{
    if(FALSE == Sum_Check(buff, buff[10]+12))                //和校验
        return FALSE;
    if(EQUIPMENT_TYPE != buff[1])           //表型0x02
        return FALSE;
    
    u8 *p = buff+2;
    u8 temp1 = 0;
    u8 i = 0;
    
    for(i = 0; i < 7; ++i)                                  //广播地址
    {
        if(p[i] != 0xaa)
        {
            temp1 = 1; 
            Broadcast_Flag=0;  //非广播
            break;
        }
        if(i==6)
           Broadcast_Flag=1;  //广播
    }

    if(temp1 == 1)                                          //如果非广播地址 则与自身表号对比
    {
        for(i=0; i < 7; ++i)
        {     
            if((p[i]) != SYS_Manuf.MeterId[i])
            {
                return FALSE;
            }
        }
    } 
    
    return TRUE;
}

void PC_Action(u8 *buff)
{
    u8 *p = buff+14;

    u8 ctr_code = buff[9];
    u8 data_length = buff[10];

    u16 command = ((u16)(buff[11])<<8) + buff[12];  
  
    switch(ctr_code)      //解析控制码
    {
        case 0x21: //////////////////////////////////////////////////////////////////厂商自定义读
        {
            switch (command)    //解析标示符
            {
                case 0xB011:  //读取电压
                    Broadcast_Flag=0;   //本来是广播，这里强制为非广播
                    break;  
                
                case 0xB012:  //读取MAC地址
                    Broadcast_Flag=0;   //本来是广播，这里强制为非广播
                    break; 
                
                case 0xB014:  //读取设备信息
                {
                    Broadcast_Flag=0;   //本来是广播，这里强制为非广播
                    
                    if(g_stGPRS_Handler.ucCommBusy == 1)
                    {
                        //这里再读一次信号强度
                        g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
                        g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;        //重新读取信息
                        g_stGPRS_Handler.StepPt = 0;
                    }
                }
                    break; 
                    
                default:
                    break;
            }
        }
            break;
            
        case 0x24: //////////////////////////////////////////////////////////////////厂商自定义写
        {   
            switch (command)    //解析标示符
            {
                case 0xB021:  //充电控制
                {
                    u8 k = 0;
                    if(data_length!=0x04)//
                    {  
                       break; 
                    }
                    Broadcast_Flag=0;   //本来是广播，这里强制为非广播
                    switch(p[k])
                    {
                        case CHARGE_ENABLE:
                        {
                            CHARGE_OPEN(); 
                        }
                          break;
                          
                        case CHARGE_DISABLE:
                        {
                            CHARGE_CLOSE();     //关闭充电
                        }
                          break;
                          
                        default:
                          break;
                    }
                }
                    break;  
              
                case 0xB022:  //设置MAC地址
                {
                    u8 k = 0;
                    if(data_length!=0x0f)//
                    {  
                       break; 
                    }  
                    Broadcast_Flag=0;

                    MemSet(&SYS_Manuf.MAC[0],12, p[k++]);
                    
                    Write_Data((u8*)SYS_Manuf.MAC,ADDR_MAC,12);
                }    
                break;  
                
                default:
                    break;
            }
        }
            break;
            
        default:
            break;
    }
}

u8 PC_Return(u8 *data_in, u8 *data_out, u16 length)
{
    u8 frame_err_flag = 0;                               //错误的帧格式则该标记置位 此时返回错误帧数据
    u8 no_replay_flag = 0;                               //已出厂启用情况下 仍接受到相关命令则该标志置位 此时无数据回复
    u8 data_length = 0;
    u8 i = 0;  
    u8 j = 0;  
    
    MemSet(data_out,FE_NUM, 0xfe);  
    MemSet(data_out+FE_NUM,length, data_in[i++]);    //数据复制
    i = 0;
    MemSet(&data_out[2+FE_NUM],7, SYS_Manuf.MeterId[i++]); //为了避免广播地址数据返回帧为7字节AA
    u8 ctr_code = data_in[9];                            //控制码   
    u16 command = ((u16)(data_in[11])<<8) + data_in[12];
    u8 *p =   data_out+14+FE_NUM;                               //有效数据开始地址   
    
    u16 voltage_usb;
    u16 voltage_vcc;
    
    switch(ctr_code)
    {
        case 0x21: //////////////////////////////////////////////////////////////////厂商自定义读
        {
            switch (command)    //解析标示符
            {
                case 0xB011:  //读取电压
                {
                    u8 k = 0;
                    data_length = 0x08;
                    if(Broadcast_Flag ==1)//广播
                        no_replay_flag = 1;
                    
                    voltage_usb = Data16_2_BCD(USB_Voltage_Value());
                    voltage_vcc = Data16_2_BCD((u16)ADC_Vref_Calculate()/10);
                    
                    p[k++] = VOLTAGE_ALL;
                    p[k++] = voltage_usb&0xff;
                    p[k++] = (voltage_usb>>8)&0xff;
                    p[k++] = voltage_vcc&0xff;
                    p[k++] = (voltage_vcc>>8)&0xff;
                }
                    break; 
                
                case 0xB012:  //读MAC地址
                {
                    u8 k = 0;
                    data_length = 0x0f;
                    if(Broadcast_Flag ==1)//广播
                       no_replay_flag = 1; 
                     
                    MemSet(p,12,SYS_Manuf.MAC[k++]);
                }
                    break;    
                    
                case 0xB014:  //读取设备信息
                {
                    u8 k = 0;
                    data_length = 0x10;
                    if(Broadcast_Flag ==1)//广播
                       no_replay_flag = 1; 
                    
                    p[k++] = 0x00;
                    p[k++] = 0x00;
                    p[k++] = (GPRS_CommPacket.MessRead.CSQ) & 0xff;
                    p[k++] = (GPRS_CommPacket.MessRead.hardwareVerLen) & 0xff;
                    for(j=0;j<GPRS_CommPacket.MessRead.hardwareVerLen;j++)
                    {
                        p[k++] = (GPRS_CommPacket.MessRead.hardwareVer[j]) & 0xff;
                    }
                    p[k++] = (GPRS_CommPacket.MessRead.softwareVerLen) & 0xff;
                    for(j=0;j<GPRS_CommPacket.MessRead.softwareVerLen;j++)
                    {
                        p[k++] = (GPRS_CommPacket.MessRead.softwareVer[j]) & 0xff;
                    }
                }
                    break;     
                    
                default:
                    no_replay_flag = 1; 
                    break;
            }
        }
            break; 
            
        case 0x24: //////////////////////////////////////////////////////////////////厂商自定义写
        {
            switch (command)    //解析标示符
            {
                case 0xB021:  //充电控制
                {
                    data_length = 0x03;//
                }
                    break; 
                
                case 0xB022:  //设置MAC地址
                {
                    data_length = 0x03;//
                }    
                    break;  
                
                default:
                    no_replay_flag = 1; 
                    break;
            }
        }
            break;  
            
        default:
            no_replay_flag = 1; 
            break;
    }
    
    if(no_replay_flag)
    {
        return FALSE;
    }
    
    if(frame_err_flag)
    {
        
        data_out[9] = data_out[13] | 0xC0;                   //异常应答控制码
        data_out[10] = 0x03;                                //数据长度 
        data_out[11] = data_in[13];                       //SER
        
        p = data_out+12;
   //     *(p++) =    (((INT8U)(SYS_Err.LowVolt_Flag))<<2) + 
    //                (((INT8U)(SYS_Status.LeaveFactory_Flag))<<5); //状态ST
   //     *(p++) =  0;
        
        data_out[14] = Sum(data_out, 14);
        data_out[15] = 0x16;
        
        return 16;                                          //返回数据长度
    }
    else 
    {
        data_out[9+FE_NUM] = data_out[9+FE_NUM] | 0x80;                   //正常应答控制码
        data_out[10+FE_NUM] = data_length;                         //数据长度
    //    data_out[13]++;                                    //SER   
       
        data_out[data_length+11+FE_NUM] = Sum(&data_out[FE_NUM], data_length+11);
                                                            //校验和
        data_out[data_length+12+FE_NUM] = 0x16;                    //
        
        return (data_length+13+FE_NUM);                            //返回数据长度
    }
}

void Return_Network(void)
{
    u8 i = 0;  
  
    MemSet(PC_TX_BUFF,FE_NUM, 0xfe); 
    PC_TX_BUFF[FE_NUM] = 0x68;
    PC_TX_BUFF[1+FE_NUM] = 0x02;
    MemSet(&PC_TX_BUFF[2+FE_NUM],7, SYS_Manuf.MeterId[i++]);          //为了避免广播地址数据返回帧为7字节AA
    PC_TX_BUFF[9+FE_NUM] = 0xa1;                                      //正常应答控制码
    PC_TX_BUFF[10+FE_NUM] = 0x18;                                       //数据长度
    PC_TX_BUFF[11+FE_NUM] = 0xb0;
    PC_TX_BUFF[12+FE_NUM] = 0x13;
    PC_TX_BUFF[13+FE_NUM] = 0x01;
    
    if(gprs_status == MQTT_FINISH)
    {
        PC_TX_BUFF[14+FE_NUM] = 0x01;                                    //联网成功
        i = 0;
        MemSet(&PC_TX_BUFF[15+FE_NUM], 20, SYS_Manuf.ICCID[i++]);
    }
    else
    {
        i = 0;
        MemSet(&PC_TX_BUFF[14+FE_NUM], 21, 0x00);                      //联网失败返回空数据
    }
    
    PC_TX_BUFF[35+FE_NUM] = Sum(&PC_TX_BUFF[FE_NUM],35);               //校验和
    PC_TX_BUFF[36+FE_NUM] = 0x16;
    
    
    for(i = 0 ; i < 40 ; i++)
    {
        Uart1_SendData(PC_TX_BUFF[i]);
    }
}
