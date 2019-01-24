#include "app_comm_pc.h"

u8 Broadcast_Flag;

SYS_ManufStruct     SYS_Manuf = {.MeterId={0x00,0x00,0x00,0x00,0x00,0x00,0x00}};

u8 PC_Decode(u8 *buff)
{
    if(FALSE == Sum_Check(buff, buff[10]+12))                //��У��
        return FALSE;
    if(EQUIPMENT_TYPE != buff[1])           //����0x02
        return FALSE;
    
    u8 *p = buff+2;
    u8 temp1 = 0;
    u8 i = 0;
    
    for(i = 0; i < 7; ++i)                                  //�㲥��ַ
    {
        if(p[i] != 0xaa)
        {
            temp1 = 1; 
            Broadcast_Flag=0;  //�ǹ㲥
            break;
        }
        if(i==6)
           Broadcast_Flag=1;  //�㲥
    }

    if(temp1 == 1)                                          //����ǹ㲥��ַ ���������ŶԱ�
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
  
    switch(ctr_code)      //����������
    {
        case 0x21: //////////////////////////////////////////////////////////////////�����Զ����
        {
            switch (command)    //������ʾ��
            {
                case 0xB011:  //��ȡ��ѹ
                    Broadcast_Flag=0;   //�����ǹ㲥������ǿ��Ϊ�ǹ㲥
                    break;  
                
                case 0xB012:  //��ȡMAC��ַ
                    Broadcast_Flag=0;   //�����ǹ㲥������ǿ��Ϊ�ǹ㲥
                    break; 
                
                case 0xB014:  //��ȡ�豸��Ϣ
                {
                    Broadcast_Flag=0;   //�����ǹ㲥������ǿ��Ϊ�ǹ㲥
                    
                    if(g_stGPRS_Handler.ucCommBusy == 1)
                    {
                        //�����ٶ�һ���ź�ǿ��
                        g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
                        g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;        //���¶�ȡ��Ϣ
                        g_stGPRS_Handler.StepPt = 0;
                    }
                }
                    break; 
                    
                default:
                    break;
            }
        }
            break;
            
        case 0x24: //////////////////////////////////////////////////////////////////�����Զ���д
        {   
            switch (command)    //������ʾ��
            {
                case 0xB021:  //������
                {
                    u8 k = 0;
                    if(data_length!=0x04)//
                    {  
                       break; 
                    }
                    Broadcast_Flag=0;   //�����ǹ㲥������ǿ��Ϊ�ǹ㲥
                    switch(p[k])
                    {
                        case CHARGE_ENABLE:
                        {
                            CHARGE_OPEN(); 
                        }
                          break;
                          
                        case CHARGE_DISABLE:
                        {
                            CHARGE_CLOSE();     //�رճ��
                        }
                          break;
                          
                        default:
                          break;
                    }
                }
                    break;  
              
                case 0xB022:  //����MAC��ַ
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
    u8 frame_err_flag = 0;                               //�����֡��ʽ��ñ����λ ��ʱ���ش���֡����
    u8 no_replay_flag = 0;                               //�ѳ������������ �Խ��ܵ����������ñ�־��λ ��ʱ�����ݻظ�
    u8 data_length = 0;
    u8 i = 0;  
    u8 j = 0;  
    
    MemSet(data_out,FE_NUM, 0xfe);  
    MemSet(data_out+FE_NUM,length, data_in[i++]);    //���ݸ���
    i = 0;
    MemSet(&data_out[2+FE_NUM],7, SYS_Manuf.MeterId[i++]); //Ϊ�˱���㲥��ַ���ݷ���֡Ϊ7�ֽ�AA
    u8 ctr_code = data_in[9];                            //������   
    u16 command = ((u16)(data_in[11])<<8) + data_in[12];
    u8 *p =   data_out+14+FE_NUM;                               //��Ч���ݿ�ʼ��ַ   
    
    u16 voltage_usb;
    u16 voltage_vcc;
    
    switch(ctr_code)
    {
        case 0x21: //////////////////////////////////////////////////////////////////�����Զ����
        {
            switch (command)    //������ʾ��
            {
                case 0xB011:  //��ȡ��ѹ
                {
                    u8 k = 0;
                    data_length = 0x08;
                    if(Broadcast_Flag ==1)//�㲥
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
                
                case 0xB012:  //��MAC��ַ
                {
                    u8 k = 0;
                    data_length = 0x0f;
                    if(Broadcast_Flag ==1)//�㲥
                       no_replay_flag = 1; 
                     
                    MemSet(p,12,SYS_Manuf.MAC[k++]);
                }
                    break;    
                    
                case 0xB014:  //��ȡ�豸��Ϣ
                {
                    u8 k = 0;
                    data_length = 0x10;
                    if(Broadcast_Flag ==1)//�㲥
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
            
        case 0x24: //////////////////////////////////////////////////////////////////�����Զ���д
        {
            switch (command)    //������ʾ��
            {
                case 0xB021:  //������
                {
                    data_length = 0x03;//
                }
                    break; 
                
                case 0xB022:  //����MAC��ַ
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
        
        data_out[9] = data_out[13] | 0xC0;                   //�쳣Ӧ�������
        data_out[10] = 0x03;                                //���ݳ��� 
        data_out[11] = data_in[13];                       //SER
        
        p = data_out+12;
   //     *(p++) =    (((INT8U)(SYS_Err.LowVolt_Flag))<<2) + 
    //                (((INT8U)(SYS_Status.LeaveFactory_Flag))<<5); //״̬ST
   //     *(p++) =  0;
        
        data_out[14] = Sum(data_out, 14);
        data_out[15] = 0x16;
        
        return 16;                                          //�������ݳ���
    }
    else 
    {
        data_out[9+FE_NUM] = data_out[9+FE_NUM] | 0x80;                   //����Ӧ�������
        data_out[10+FE_NUM] = data_length;                         //���ݳ���
    //    data_out[13]++;                                    //SER   
       
        data_out[data_length+11+FE_NUM] = Sum(&data_out[FE_NUM], data_length+11);
                                                            //У���
        data_out[data_length+12+FE_NUM] = 0x16;                    //
        
        return (data_length+13+FE_NUM);                            //�������ݳ���
    }
}

void Return_Network(void)
{
    u8 i = 0;  
  
    MemSet(PC_TX_BUFF,FE_NUM, 0xfe); 
    PC_TX_BUFF[FE_NUM] = 0x68;
    PC_TX_BUFF[1+FE_NUM] = 0x02;
    MemSet(&PC_TX_BUFF[2+FE_NUM],7, SYS_Manuf.MeterId[i++]);          //Ϊ�˱���㲥��ַ���ݷ���֡Ϊ7�ֽ�AA
    PC_TX_BUFF[9+FE_NUM] = 0xa1;                                      //����Ӧ�������
    PC_TX_BUFF[10+FE_NUM] = 0x18;                                       //���ݳ���
    PC_TX_BUFF[11+FE_NUM] = 0xb0;
    PC_TX_BUFF[12+FE_NUM] = 0x13;
    PC_TX_BUFF[13+FE_NUM] = 0x01;
    
    if(gprs_status == MQTT_FINISH)
    {
        PC_TX_BUFF[14+FE_NUM] = 0x01;                                    //�����ɹ�
        i = 0;
        MemSet(&PC_TX_BUFF[15+FE_NUM], 20, SYS_Manuf.ICCID[i++]);
    }
    else
    {
        i = 0;
        MemSet(&PC_TX_BUFF[14+FE_NUM], 21, 0x00);                      //����ʧ�ܷ��ؿ�����
    }
    
    PC_TX_BUFF[35+FE_NUM] = Sum(&PC_TX_BUFF[FE_NUM],35);               //У���
    PC_TX_BUFF[36+FE_NUM] = 0x16;
    
    
    for(i = 0 ; i < 40 ; i++)
    {
        Uart1_SendData(PC_TX_BUFF[i]);
    }
}
