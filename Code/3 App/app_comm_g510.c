#include "app_comm_g510.h"

GPRS_Handler        g_stGPRS_Handler;
u8                  gprs_status;

typedef void(*GPRS_State_Fun)(U8 *RxBuf); /* 定义状态处理函数*/

static void APP_GPRS_State_Init_Proc   (U8 *RxBuf);
static void APP_GPRS_State_Connect_Proc(U8 *RxBuf);
static void APP_GPRS_State_Mess_Proc(U8 *RxBuf);
static void APP_GPRS_State_Comm_Proc   (U8 *RxBuf);

static const GPRS_State_Fun APP_GPRS_StateProcTbl[] = 
{
    NULL,     //POWER_ON
    NULL,     //RESET
    APP_GPRS_State_Init_Proc,
	APP_GPRS_State_Connect_Proc,
	APP_GPRS_State_Mess_Proc,
	APP_GPRS_State_Comm_Proc,
	NULL,
    NULL,
    NULL,
    NULL,
};

/******************GPRS指令集**********************************/
const GPRS_Cmd_Data_Type AT_CmdTbl[] = 
{
    /*------cmdstr-----------------------------------revtimeout(s)--cmdtruebackstr----------retry_times */
    /************************* GPRS  network ***************************/
      {"AT\r\n",                                         4,       "OK",                     2},
      {"ATE0\r\n",                                       3,       "OK",                     3},
      //{"AT+GTSET=\"GDB\",1\r\n",                         3,       "OK",                     3},
      {"AT+CGREG?\r\n",   			                     3,	      "+CGREG: 0,1",         	20},
      
    /************************* READ MESS *******************************/
      {"AT+CSQ\r\n",   			                         3,	      "+CSQ",         	        3},
      {"AT+CCID\r\n",                                    4,       "898",                    2},
      
    /************************* AWS connect *****************************/
      {"AT+MIPCALL=1,\"CMNET\"\r\n",   			         35,	  "+MIPCALL: ",           	3},
      {"AT+MIPCALL?\r\n",                                3,       "+MIPCALL: 1",            2},
      {"AT+GTSSLVER=4\r\n",   		         	         4,	      "OK",            	        2},
      {"AT+GTSSLFILE=\"CAFILE\",0000\r\n",   		     4,	      ">",             	        2},
      {"CAFILE_DATA\r\n",   	                	     4,	      "OK",            	        1},
      {"AT+GTSSLFILE=\"KEYFILE\",0000\r\n",   		     4,	      ">",             	        2},
      {"KEYFILE_DATA\r\n",   		                     4,	      "OK",            	        1},
      {"AT+GTSSLFILE=\"TRUSTFILE\",0000\r\n",   		 4,	      ">",             	        2},
      {"TRUSTFILE_DATA\r\n",   		                     4,	      "OK",            	        1},
      {"AT+MQTTWILL=1,\"Aaws/things/LPD_XC000000/subscriptions\",0,0,149\r\n",
                                                         4,	      ">",             	        2},
      {"MQTTWILL_DATA\r\n",   		                     4,	      "OK",            	        3},
      {"AT+MQTTOPEN=1,\"a7iko9biw0dza.iot.cn-north-1.amazonaws.com.cn\",8883,1,60,1\r\n",   		 
                                                         40,	  "MQTTOPEN: 1,1", 	        5},
      {"AT+MQTTSUB=1,\"$aws/things/LPD_XC000000/shadow/update/accepted\",0\r\n",   		                     
                                                         35,	  "MQTTSUB: 1,1",  	        5},
      {"AT+MQTTSUB=1,\"Aaws/things/LPD_XC000000/subscriptions\",0\r\n",   		                     
                                                         35,	  "MQTTSUB: 1,1",  	        5},
      /************************* MQTT PUB ********************************/
      {"AT+MQTTPUB=1,\"$aws/things/LPD_XC000000/shadow/update\",0,0,95\r\n",35,	 ">",       3},
                                                         
      {"{\"state\"}",                                    35,	  "+MQTTPUB: 1,1", 	        1},  
      {NULL,                                          NULL,       "+MQTTMSG: ",     NULL},
      {NULL,                                          NULL,       "+MQTTLOSE: 1, 1",     NULL},
};

/*************************初始化流程***************************/
static const U8 s_ATCmdStep_Init[] = 
{
    NULL,
    GPRS_AT,       
    GPRS_AT_ATE0,    
    //GPRS_AT_GTSET,
    GPRS_AT_CGREG1,                                
};

/************************* GPRS/MQTT网络建立连接***************************/
static const U8 s_ATCmdStep_Connnect[] = 
{
    GPRS_AT_MIPCALL_1, 
    GPRS_AT_MIPCALL1,
    
    GPRS_AT_GTSSLVER,
    GPRS_AT_CAFILE,
    GPRS_AT_CAFILE_DATA,
    GPRS_AT_KEYFILE,
    GPRS_AT_KEYFILE_DATA,
    GPRS_AT_TRUSFILE,
    GPRS_AT_TRUSFILE_DATA,
    
    GPRS_AT_MQTTWILL,
    GPRS_AT_MQTTWILL_DATA,
    
    GPRS_AT_MQTTOPEN,
    
    GPRS_AT_MQTTSUB,
    GPRS_AT_MQTTSUB1,
};

/*************************数据发送流程***************************/
static const U8 s_ATCmdStep_Comm[] = 
{
    GPRS_AT_MQTTPUB,  
    GPRS_AT_MQTTPUB_DATA,                             
};

/*************************模块数据读取流程**********************/
static const U8 s_ATCmdStep_Mess[] = 
{
    GPRS_AT_CSQ,GPRS_AT_CCID,                    
};

/*校验AT指令应答结果是否正确*/
static U8 APP_GPRS_ATCmdAckCheck(U8 *RxBuf)
{ 
#if DEBUG
    if(Check_Strstr((S8*)RxBuf, "ready", GPRS_ATCmdCB.ucRxLen) == TRUE)
    {
        BSP_LOG((char*)RxBuf);
    }
#endif
    if(Check_Strstr((S8*)RxBuf, "READY", GPRS_ATCmdCB.ucRxLen) == TRUE)
    {
        TASK_State.task_at_reday_flag = ENABLE;
        GPRS_ATCmdCB.RspType = RSP_TYPE_CMD; 
	    return BACK_TRUE;
    }
    
     /*判断接收的数据是否含有期望应答*/
	if(Check_Strstr((S8*)RxBuf, GPRS_ATCmdCB.ExpResultStr, GPRS_ATCmdCB.ucRxLen) == TRUE)
	{
		GPRS_ATCmdCB.RspType = RSP_TYPE_CMD;/*应答AT指令*/
	    return BACK_TRUE;                 /*接收到正确应答*/
	}
    else
	{
	    if(GPRS_ATCmdCB.ATCmdPt != GPRS_AT_MIPCALL1)
	    {
            if(Check_Strstr((S8*)RxBuf, "MIPCALL: 0", GPRS_ATCmdCB.ucRxLen) == TRUE)
            {       
                g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;    //重新连接网络
                g_stGPRS_Handler.StepPt = 0;
                GPRS_Comm_Event_Set(COMM_Event_CONN);/*上电需发送一个登录包*/
            }
        }
      
        /*判断是否是服务器下行数据*/
        if(Check_Strstr((S8*)RxBuf, (S8*)(S8*)AT_CmdTbl[GPRS_AT_MQTTLOSE].ExpResultStr, GPRS_ATCmdCB.ucRxLen) == TRUE)  
        {
            GPRS_ATCmdCB.RspType = RSP_TYPE_NO_NET;  /*通信数据*/
            return BACK_TRUE;   /*接收到正确应答*/
        }
  
        if(Check_Strstr((S8*)RxBuf, (S8*)(S8*)AT_CmdTbl[GPRS_AT_MQTTMSG].ExpResultStr, GPRS_ATCmdCB.ucRxLen) == TRUE)  
        {
            GPRS_ATCmdCB.RspType = RSP_TYPE_DATA;  /*通信数据*/
            return BACK_TRUE;   /*接收到正确应答*/
        }
      
	    /*由于获取网络侧地址和建立soctet连接都是分两条返回的，第一条都为"OK"*/
	    if((GPRS_ATCmdCB.ATCmdPt == GPRS_AT_MIPCALL_1) || 
           (GPRS_ATCmdCB.ATCmdPt == GPRS_AT_MQTTPUB_DATA) ||
           (GPRS_ATCmdCB.ATCmdPt == GPRS_AT_MQTTSUB) ||
           (GPRS_ATCmdCB.ATCmdPt == GPRS_AT_MQTTSUB1) ||   
           (GPRS_ATCmdCB.ATCmdPt == GPRS_AT_MQTTOPEN))
	    {
  	        if(Check_Strstr((S8*)RxBuf, "OK", 4) == TRUE)   //去掉这一句再试 
  	        {
  	            return NO_BACK;   /*第一条"OK"返回正确，所以继续等待*/
  	        }
	    }
        
	     /*查询是否驻网成功，如不成功，那么等待3S，而不是返回错误，该条指令特殊*/
        if(GPRS_ATCmdCB.ATCmdPt == GPRS_AT_CGREG1) 
        {
            return NO_BACK;
        }
		return BACK_ERROR;
	}

	
}

/*根据不同的数据ID,进行组帧，封装协议层的帧内容*/
static U16 APP_GPRS_WriteDataIDPacket(U8 DataID, U8* Packet)
{
    U16 usPacketLen = 0;
    U8 dataLen = 0;
    U8 *fileLen = NULL;
    u16 remaining_time;
    u16 outage_time;
    u8 state;
    u8 time_h;
    u8 time_l;
    U8 data[6]={0};
    U8 *p_data=NULL;
    char *zero = "000";
    U8 fillLen = 0;
    U8 i = 0;
    
    memset((U8*)Packet, 0, GPRS_RX_BUFF_LEN);
    
	switch(DataID)
    {
        case COMM_ID_CONN:
        {
            strcpy((S8*)Packet, MQTT_CONN_DATA1);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            memcpy((S8*)fileLen, SYS_Manuf.MAC, 12);
            fileLen += 12;
            
            strcpy((S8*)fileLen, MQTT_CONN_DATA2);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            if(System_Flag.flag_need_charge == 1)
            {
                state = 0x31;
            }
            else
            {
                state = 0x30;
            }
            memcpy((S8*)fileLen, &state, 1);
            fileLen += 1;
            
            strcpy((S8*)fileLen, MQTT_CONN_DATA3);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            if(hour_time_count >= remaining_time_count)
            {
                remaining_time = 0;
            }
            else
            {
                remaining_time = remaining_time_count - hour_time_count;
            }
    
            if(remaining_time%60 == 0)
            {
                remaining_time = remaining_time/60;
            }
            else
            {
                remaining_time = remaining_time/60 + 1;
            }
            remaining_time = Data8_2_BCD(remaining_time);
            time_h = (remaining_time>>4)+0x30;
            time_l = (remaining_time&0x0f)+0x30;
            memcpy((S8*)fileLen, &time_h, 1);
            fileLen += 1;
            memcpy((S8*)fileLen, &time_l, 1);
            fileLen += 1;
            
            strcpy((S8*)fileLen, MQTT_CONN_DATA4);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            memcpy((S8*)fileLen, SYS_Manuf.ICCID, 20);
            fileLen += 20;
            
            strcpy((S8*)fileLen, MQTT_CONN_DATA5);
            
            GPRS_Comm_Event_Clr(COMM_Event_DEV);
        }
            break;  
            
        case COMM_ID_COM:
        {
            u8 resulet;
          
            strcpy((S8*)Packet, MQTT_COM_DATA1);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            memcpy((S8*)fileLen, SYS_Manuf.MAC, 12);
            fileLen += 12;
            
            strcpy((S8*)fileLen, MQTT_COM_DATA2);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            if(hour_time_count >= remaining_time_count)
            {
                remaining_time = 0;
            }
            else
            {
                remaining_time = remaining_time_count - hour_time_count;
            }
    
            if(remaining_time%60 == 0)
            {
                remaining_time = remaining_time/60;
            }
            else
            {
                remaining_time = remaining_time/60 + 1;
            }
            memset(data,0,5);
            p_data = DecimalToASCII16(remaining_time,data);
            if(p_data != NULL)
            {
                fillLen = strlen((char*)p_data);
                for(i=0;i<(3-fillLen);i++)
                {
                    fileLen[i] = '0';     
                }
                fileLen += (3-fillLen);
                memcpy(fileLen,(char*)p_data,fillLen);
                fileLen += strlen((char*)p_data);
            }
            else
            {
                memcpy((S8*)fileLen, zero, 3);
                fileLen += 3;
            }
            strcpy((S8*)fileLen, MQTT_COM_DATA3);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            outage_time = outage_time_count;
            if(outage_time%60 == 0)
            {
                outage_time = outage_time/60;
            }
            else
            {
                outage_time = outage_time/60+1;
            }
            memset(data,0,5);
            p_data = DecimalToASCII16(outage_time,data);
            if(p_data != NULL)
            {
                fillLen = strlen((char*)p_data);
                for(i=0;i<(3-fillLen);i++)
                {
                    fileLen[i] = '0';     
                }
                fileLen += (3-fillLen);
                memcpy(fileLen,(char*)p_data,strlen((char*)p_data));
                fileLen += strlen((char*)p_data);
            }
            else
            {
                memcpy((S8*)fileLen, zero, 3);
                fileLen += 3;            
            }
            
            strcpy((S8*)fileLen, MQTT_COM_DATA4);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;           
            if(System_Flag.flag_need_charge == 1)
            {
                state = 0x31;
            }
            else
            {
                state = 0x30;
            }
            memcpy((S8*)fileLen, &state, 1);
            fileLen += 1;
            strcpy((S8*)fileLen, MQTT_COM_DATA5);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen; 
            if(System_Flag.flag_return_com_result == 1)
            {
                resulet = 0x31;
            }
            else
            {
                resulet = 0x30;
            }
            memcpy((S8*)fileLen, &resulet, 1);
            fileLen += 1;
            
            strcpy((S8*)fileLen, MQTT_COM_DATA6);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            memcpy((S8*)fileLen, SYS_Manuf.ICCID, 20);
            fileLen += 20;
            
            strcpy((S8*)fileLen, MQTT_COM_DATA7);
        }
            break;  
            
        case COMM_ID_DISCONN:
        {  
            strcpy((S8*)Packet, MQTT_DISCONN_DATA1);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            memcpy((S8*)fileLen, SYS_Manuf.MAC, 12);
            fileLen += 12;
            
            strcpy((S8*)fileLen, MQTT_DISCONN_DATA2);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            if(System_Flag.flag_need_charge == 1)
            {
                state = 0x31;
            }
            else
            {
                state = 0x30;
            }
            memcpy((S8*)fileLen, &state, 1);
            fileLen += 1;
            
            strcpy((S8*)fileLen, MQTT_DISCONN_DATA3);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            if(hour_time_count >= remaining_time_count)
            {
                remaining_time = 0;
            }
            else
            {
                remaining_time = remaining_time_count - hour_time_count;
            }
    
            if(remaining_time%60 == 0)
            {
                remaining_time = remaining_time/60;
            }
            else
            {
                remaining_time = remaining_time/60 + 1;
            }
            remaining_time = Data8_2_BCD(remaining_time);
            time_h = (remaining_time>>4)+0x30;
            time_l = (remaining_time&0x0f)+0x30;
            memcpy((S8*)fileLen, &time_h, 1);
            fileLen += 1;
            memcpy((S8*)fileLen, &time_l, 1);
            fileLen += 1;
            
            strcpy((S8*)fileLen, MQTT_DISCONN_DATA4);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            memcpy((S8*)fileLen, SYS_Manuf.ICCID, 20);
            fileLen += 20;
            
            strcpy((S8*)fileLen, MQTT_DISCONN_DATA5);
        }
            break;  
        case COMM_ID_REPORTED:
        {
            strcpy((S8*)Packet, MQTT_REPORTED_DATA1);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            memcpy((S8*)fileLen, SYS_Manuf.MAC, 12);
            fileLen += 12;
            
            strcpy((S8*)fileLen, MQTT_REPORTED_DATA2);
        }
            break;     
        case COMM_ID_DEV:
        {
            strcpy((S8*)Packet, MQTT_DEV_DATA1);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            memcpy((S8*)fileLen, SYS_Manuf.MAC, 12);
            fileLen += 12;
            
            strcpy((S8*)fileLen, MQTT_DEV_DATA2);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            if(System_Flag.flag_need_charge == 1)
            {
                state = 0x31;
            }
            else
            {
                state = 0x30;
            }
            memcpy((S8*)fileLen, &state, 1);
            fileLen += 1;
            
            strcpy((S8*)fileLen, MQTT_DEV_DATA3);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            if(hour_time_count >= remaining_time_count)
            {
                remaining_time = 0;
            }
            else
            {
                remaining_time = remaining_time_count - hour_time_count;
            }
    
            if(remaining_time%60 == 0)
            {
                remaining_time = remaining_time/60;
            }
            else
            {
                remaining_time = remaining_time/60 + 1;
            }
            memset(data,0,5);
            p_data = DecimalToASCII16(remaining_time,data);
            if(p_data != NULL)
            {
                fillLen = strlen((char*)p_data);
                for(i=0;i<(3-fillLen);i++)
                {
                    fileLen[i] = '0';     
                }
                fileLen += (3-fillLen);
                memcpy(fileLen,(char*)p_data,fillLen);
                fileLen += strlen((char*)p_data);
            }
            else
            {
                memcpy((S8*)fileLen, zero, 3);
                fileLen += 3;
            }
            #if 1
            strcpy((S8*)fileLen, MQTT_DEV_DATA4);
            dataLen = strlen((char*)Packet);
            fileLen = Packet + dataLen;
            
            memcpy((S8*)fileLen, SYS_Manuf.ICCID, 20);
            fileLen += 20;
            #endif
            strcpy((S8*)fileLen, MQTT_DEV_DATA5);
        }
            break; 
            
        default:
            break; 
    }
    
    usPacketLen = strlen((char*)Packet);
    
	return usPacketLen;
}

static void APP_GPRS_SetMAC(U8 *pFrameDes)
{
    U8 i=0;
    
    for(i=0; i<12; i++)
    {
        pFrameDes[i] = SYS_Manuf.MAC[i]; 
    }
}

static U16 APP_GPRS_WritePUBPacket(U8 DataID, U8* Packet)
{
    U16 usPacketLen = 0;
    U8 dataNum;
    U8 i=0;
    U8 data_buff[11] = {0};
    U8 dataLen = 0;
    U8 *fileLen = NULL;
    char *pEndData = "subscriptions\",0,0,95\r\n";
    memset((U8*)Packet, 0, GPRS_RX_BUFF_LEN);
    
    strcpy((S8*)Packet, AT_CmdTbl[GPRS_AT_MQTTPUB].CmdStr); 
    dataNum = Get_Symbol(2, (u8*)Packet, '/');
    APP_GPRS_SetMAC(&Packet[dataNum]);
        
    switch(DataID)
    {
        case COMM_ID_CONN:
        {
            dataNum = Get_Symbol(1, (u8*)Packet, ','); 
            Packet[dataNum+1] = 'A';
            dataNum = Get_Symbol(3, (u8*)Packet, '/');
            memcpy(&Packet[dataNum],pEndData,strlen(pEndData));
            dataNum = Get_Symbol(4, (u8*)Packet, ',');
            fileLen = DecimalToASCII(146, data_buff);
        }
            break;  
            
        case COMM_ID_COM:
        {
            dataNum = Get_Symbol(4, (u8*)Packet, ',');
            fileLen = DecimalToASCII(155, data_buff);
        }
            break;  
            
        case COMM_ID_DISCONN:
        {  
            dataNum = Get_Symbol(4, (u8*)Packet, ',');
            fileLen = DecimalToASCII(149, data_buff);
        }
            break;  
        case COMM_ID_REPORTED:
        {   
            dataNum = Get_Symbol(1, (u8*)Packet, ','); 
            Packet[dataNum+1] = 'A';
            dataNum = Get_Symbol(3, (u8*)Packet, '/');
            memcpy(&Packet[dataNum],pEndData,strlen(pEndData));
            dataNum = Get_Symbol(4, (u8*)Packet, ',');
            fileLen = DecimalToASCII(72, data_buff);
        }
            break;
            
        case COMM_ID_DEV:
        {
            dataNum = Get_Symbol(4, (u8*)Packet, ',');
            fileLen = DecimalToASCII(143, data_buff);
        }
            break; 
            
        default:
            break; 
    }
    
    if(fileLen != NULL)
    {
        dataLen = strlen((char*)fileLen);
            
        for(i=0; i<dataLen; i++)
        {
            Packet[dataNum+i] = fileLen[i]; 
        }
        Packet[dataNum+dataLen]='\r';
        Packet[dataNum+dataLen+1]='\n'; 
        Packet[dataNum+dataLen+2]=0; 
    }
    
    usPacketLen = strlen((char*)Packet);
    
	return usPacketLen;
}

static void Send_WILL_Data(void)
{
//    u16 remaining_time;
//    u8 time_h;
//    u8 time_l;
    u8 state;
    
    Command_Send_G510(MQTT_WILL_DATA1,(sizeof(MQTT_WILL_DATA1)-1));
    Command_Send_G510((u8 *)&SYS_Manuf.MAC,12);
    Command_Send_G510(MQTT_WILL_DATA2,(sizeof(MQTT_WILL_DATA2)-1));
    
    state = 0x30;
    Command_Send_G510(&state,sizeof(state));
    Command_Send_G510(MQTT_WILL_DATA3,(sizeof(MQTT_WILL_DATA3)-1));
#if 0     
    remaining_time = 0;
    remaining_time = Data8_2_BCD(remaining_time);
    time_h = (remaining_time>>4)+0x30;
    time_l = (remaining_time&0x0f)+0x30;
    Command_Send_G510(&time_h,sizeof(time_h));
    Command_Send_G510(&time_l,sizeof(time_l));    
#endif
    Command_Send_G510("000",3); 
    Command_Send_G510(MQTT_WILL_DATA4,(sizeof(MQTT_WILL_DATA4)-1));
    Command_Send_G510((u8 *)&SYS_Manuf.ICCID,20);
    Command_Send_G510(MQTT_WILL_DATA5,(sizeof(MQTT_WILL_DATA5)-1));
}

static void File_Send_G510(u8 file_name)
{
    u8* f_data;
    f_data = (u8*)malloc(256);
    u32 File_Address = 0;
    
    switch(file_name)
    {
        case CERTIFICATE_FILE:
        {
            u16 total_data = 0;
            File_Address = CERTIFICATE_FILE_ADDR;
            
            do
            {
                Read_BufferFlash(File_Address,f_data,256);
                
                for(u16 i=0;i<256&total_data<ssl_file_len.certificate_file_len;i++,total_data++)
                {
                    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);   //判断发送数据寄存器是否为空
                        USART_SendData8(USART2,f_data[i]);
                }
                
                File_Address+=256;
            }while(total_data<ssl_file_len.certificate_file_len);
        }
            break;
            
        case PRIVATE_FILE:
        {
            u16 total_data = 0;
            File_Address = PRIVATE_FILE_ADDR;
            
            do
            {
                Read_BufferFlash(File_Address,f_data,256);
                
                for(u16 i=0;i<256&total_data<ssl_file_len.private_file_len;i++,total_data++)
                {
                    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);   //判断发送数据寄存器是否为空
                        USART_SendData8(USART2,f_data[i]);
                }
                
                File_Address+=256;
            }while(total_data<ssl_file_len.private_file_len);
        }
            break;
            
        case CA_FILE:
        {
            u16 total_data = 0;
            File_Address = CA_FILE_ADDR;
            
            do
            {
                Read_BufferFlash(File_Address,f_data,256);
                
                for(u16 i=0;i<256&total_data<ssl_file_len.CA_file_len;i++,total_data++)
                {
                    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);   //判断发送数据寄存器是否为空
                        USART_SendData8(USART2,f_data[i]);
                }
                
                File_Address+=256;
            }while(total_data<ssl_file_len.CA_file_len);
        }
            break;
            
        default:
            break;
    }
    free(f_data);
    f_data = NULL;
}

static void APP_GPRS_SetFileLen(u8 file_name, U8 *pFrameDes)
{
    U8 i=0;
    U8 data_buff[11] = {0};
    U8 dataLen = 0;
    U8 *fileLen = NULL;
    
    switch(file_name)
    {
        case CERTIFICATE_FILE:
        {
            fileLen = DecimalToASCII(ssl_file_len.certificate_file_len, data_buff);
        }
            break;  
            
        case PRIVATE_FILE:
        {
            fileLen = DecimalToASCII(ssl_file_len.private_file_len, data_buff);
        }
            break;  
            
        case CA_FILE:
        {  
            fileLen = DecimalToASCII(ssl_file_len.CA_file_len, data_buff);
        }
            break;  
            
        default:
            break; 
    }
    
    if(fileLen != NULL)
    {
        dataLen = strlen((char*)fileLen);
            
        for(i=0; i<dataLen; i++)
        {
            pFrameDes[i] = fileLen[i]; 
        }
        pFrameDes[dataLen]='\r';
        pFrameDes[dataLen+1]='\n'; 
    }
}

static U8 APP_GPRS_WriteATPacket(U8 ATCmdIndex, U8 *pFrameDes)
{  
    U8 ucSendLen;
    U8 dataNum;
	switch(ATCmdIndex)
	{
		case GPRS_AT_CAFILE:/*填充数据内容*/
		{
            strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);  
            dataNum = Get_Symbol(1, (u8*)pFrameDes, ',');
            APP_GPRS_SetFileLen(CERTIFICATE_FILE, &pFrameDes[dataNum]);
            ucSendLen = strlen((char*)pFrameDes);
        }
            break;
            
		case GPRS_AT_CAFILE_DATA:/*填充数据内容*/
		{
			File_Send_G510(CERTIFICATE_FILE);
            ucSendLen = 0;
		}
            break;
        
        case GPRS_AT_KEYFILE:/*填充数据内容*/
		{
            strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);  
            dataNum = Get_Symbol(1, (u8*)pFrameDes, ',');
            APP_GPRS_SetFileLen(PRIVATE_FILE, &pFrameDes[dataNum]);
            ucSendLen = strlen((char*)pFrameDes);
        }
            break;
            
		case GPRS_AT_KEYFILE_DATA:/*填充数据内容*/
		{
			File_Send_G510(PRIVATE_FILE);
            ucSendLen = 0;
		}
            break; 
            
        case GPRS_AT_TRUSFILE:/*填充数据内容*/
		{
            strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);  
            dataNum = Get_Symbol(1, (u8*)pFrameDes, ',');
            APP_GPRS_SetFileLen(CA_FILE, &pFrameDes[dataNum]);
            ucSendLen = strlen((char*)pFrameDes);
        }
            break;
            
		case GPRS_AT_TRUSFILE_DATA:/*填充数据内容*/
		{
			File_Send_G510(CA_FILE);
            ucSendLen = 0;
		}
            break;
        
		case GPRS_AT_MQTTWILL:/*填充数据内容*/
		{
            strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);  
            dataNum = Get_Symbol(2, (u8*)pFrameDes, '/');
            APP_GPRS_SetMAC(&pFrameDes[dataNum]);
            ucSendLen = strlen((char*)pFrameDes);
		}
            break;
            
		case GPRS_AT_MQTTWILL_DATA:/*填充数据内容*/
		{
			Send_WILL_Data();
            ucSendLen = 0;
		}
            break;
            
		case GPRS_AT_MQTTSUB:/*填充数据内容*/
		case GPRS_AT_MQTTSUB1:
		{
            strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);  
            dataNum = Get_Symbol(2, (u8*)pFrameDes, '/');
            APP_GPRS_SetMAC(&pFrameDes[dataNum]);
            ucSendLen = strlen((char*)pFrameDes);
		}
            break; 
            
		case GPRS_AT_MQTTPUB:/*填充数据内容*/
        case GPRS_AT_MQTTPUB_DATA:/*填充数据内容*/
		{
		     memcpy(pFrameDes, g_stGPRS_Handler.TxBuf, g_stGPRS_Handler.ucTxLen);/*copy 应用层数据*/
             ucSendLen = strlen((char*)pFrameDes);
		}break;
            
		//case GPRS_AT_MQTTOPEN:/*填充数据内容*/
		//{
        //    strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);
		//	ucSendLen = strlen(AT_CmdTbl[ATCmdIndex].CmdStr);/*默认的指令符长度*/
		//}break;
        
		//case GPRS_AT_MQTTPUB_DATA:/*填充数据内容*/
		//{
        //    memcpy(pFrameDes, g_stGPRS_Handler.TxBuf, g_stGPRS_Handler.ucTxLen);/*copy 应用层数据*/
        //   ucSendLen = strlen((char*)pFrameDes);
		//}break;
        
		default:
		{
			strcpy((S8*)pFrameDes, AT_CmdTbl[ATCmdIndex].CmdStr);
			ucSendLen = strlen(AT_CmdTbl[ATCmdIndex].CmdStr);/*默认的指令符长度*/
		}
            break;
	}
	return ucSendLen;    
}

static void APP_GPRS_SendATCmd(AT_CMD_CB* pNBATCmdCB)
{
	U8 ucResendCnt;
	U8 ATCmdIndex;
	AT_CMD_CB *pAtCmd = pNBATCmdCB;
	ATCmdIndex = pAtCmd->ATCmdPt;
	if(1 == pAtCmd->Busy)
	{
		return ;/*当前忙碌状态*/	
	}
	/*根据不同的AT应答结果,设置重发次数*/
	if((BACK_TRUE == pAtCmd->ATCmdResult) || (BACK_PERTIMES_OUT == pAtCmd->ATCmdResult))
	{
		ucResendCnt = 0;/*重发清零*/	
	}
	else
	{
	    ucResendCnt = GPRS_ATCmdCB.ucResendCnt;/*备份重发次数*/;	
	}

	memset((U8*)pAtCmd, 0, sizeof(GPRS_ATCmdCB));
	
	pAtCmd->Busy         = 1;          /*设置忙绿状态*/
	pAtCmd->ATCmdPt      = ATCmdIndex; /*更新AT索引号*/
	pAtCmd->ATCmdResult  = NO_BACK;    /*初始化返回结果*/
	pAtCmd->ucResendCnt  = ucResendCnt;/*设置重发计数*/ 
	pAtCmd->ucResendTimes = AT_CmdTbl[ATCmdIndex].ucResendTimes;/*设置重发次数*/
	pAtCmd->ulTimeOut    = TmrSecondsToMilliseconds(AT_CmdTbl[ATCmdIndex].usTimeOut);/*刷新超时时间*/

	/*****t填充指令内容长度******/
	pAtCmd->ucCmdStrLen = APP_GPRS_WriteATPacket(ATCmdIndex, pAtCmd->CmdStr);
	/*填充指令符期望结果*/	
	strcpy(pAtCmd->ExpResultStr, AT_CmdTbl[ATCmdIndex].ExpResultStr);
	/*发送AT指令*/	
	GPRS_SendATCmdStr(pAtCmd->CmdStr, pAtCmd->ucCmdStrLen);
	/*发送次数+1*/	
    pAtCmd->ucResendCnt++;
}

static void APP_GPRS_State_Init_Proc(U8 *RxBuf)
{
	U8 ATCmdIndex;
	if(g_stGPRS_Handler.StepPt >= INIT_STEP_NUM)
	{
		return;	
	}
	ATCmdIndex = s_ATCmdStep_Init[g_stGPRS_Handler.StepPt];
	/************************AT指令应答正确·**************************/	
	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*AT指令应答内容正确*/
	{
		/*next Step*/
		g_stGPRS_Handler.StepPt++;
		if(g_stGPRS_Handler.StepPt >= INIT_STEP_NUM)
		{
			/******GPRS模块状态切换***************/
			g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;/*切换到下一个流程*/
			g_stGPRS_Handler.StepPt = 0;     /**clr step*/
			GPRS_NetPar.NetType = NET_GPRS;/*准备GPRS网络连接状态*/
		}
		else
		{
		    g_stGPRS_Handler.State = GPRS_STATE_INIT;/*继续当前流程*/		
		}	
	}
/************************AT 指令达到重发次数**************************/	
	else if(BACK_PERTIMES_OUT <= GPRS_ATCmdCB.ATCmdResult)/*AT指令重发次数到了*/
	{
		switch(ATCmdIndex)
		{
		    case GPRS_AT:   //直接开始下一条指令
			{
			    g_stGPRS_Handler.StepPt++;
			    g_stGPRS_Handler.State = GPRS_STATE_INIT; /*继续当前流程*/
			}
                break;
                
			default:
			{
			    #if DEBUG
	                BSP_LOG("/** AT RESET**/\n\n");
                #endif 
			    g_stGPRS_Handler.State = GPRS_STATE_RESET;       //进行复位
		        g_stGPRS_Handler.StepPt = 0;/**clr step*/
		        GPRS_NetPar.NetType = NET_NO_NET;
			}
				break;	
		}
	}
/************************AT指令应答错误或超时*************************/	
	else
    {
		/*没有到AT指令重发次数，所以再次发送AT指令*/
		g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*直接再次发送AT指令，然后等待应答*/
		GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Init[g_stGPRS_Handler.StepPt];
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/	 
    }	
}

static void APP_GPRS_State_Mess_Proc(U8 *RxBuf)
{
    U8 ATCmdIndex;
    U8 asi_len=0;
    char *str = "898";
    char *p   = NULL;
    if(g_stGPRS_Handler.StepPt >= MESS_STEP_NUM)
    {
        return; 
    }   
    ATCmdIndex = s_ATCmdStep_Mess[g_stGPRS_Handler.StepPt];
    /************************AT指令应答正确·**************************/    
    if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*AT指令应答内容正确*/
    {
        switch(ATCmdIndex)
        {
            case GPRS_AT_CCID:
            {
                //获取ICCID
				p = strstr((char*)RxBuf,str);
				asi_len = strlen(p);
				if(asi_len > 20) 
                  asi_len = 20; 
                
                for(int i=0; i<20; i++)
                {
                    SYS_Manuf.ICCID[i] = p[i];
                }
            }
                break;	
            case GPRS_AT_CSQ:
            {
                GPRS_CommPacket.MessRead.CSQ = Get_Uint_Data((S8*)&RxBuf[Get_Symbol(1, RxBuf, ',')] - 3);
			}
                break;    
                
			default:
				break;
        }
            /*next Step*/
        g_stGPRS_Handler.StepPt++;
        if(g_stGPRS_Handler.StepPt >= MESS_STEP_NUM)
        {
            /******GPRS模块状态切换***************/
            g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;/*切换到下一个流程*/
            g_stGPRS_Handler.StepPt = 0;     /**clr step*/
        }
        else
        {
            g_stGPRS_Handler.State = GPRS_STATE_READ_MESS;/*继续当前流程*/       
        }   
    }
    /************************AT 指令达到重发次数**************************/ 
    else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)/*AT指令重发次数到了*/
    {
        #if DEBUG
	        BSP_LOG("/** AT_MESS RESET**/\n\n");
        #endif 
        g_stGPRS_Handler.State = GPRS_STATE_RESET;  //进行复位
        g_stGPRS_Handler.StepPt = 0;/**clr step*/
        GPRS_NetPar.NetType = NET_NO_NET;
    }
    /************************AT指令应答错误或超时*************************/ 
    else
    {
        /*没有到AT指令重发次数，所以再次发送AT指令*/
        g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*直接再次发送AT指令，然后等待应答*/
        GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stGPRS_Handler.StepPt];
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/  
    }
}

static void APP_GPRS_State_Connect_Proc(U8 *RxBuf)
{
    U8 ATCmdIndex;
	static U8 errCnt=0;
    
	if(g_stGPRS_Handler.StepPt >= CONNECT_STEP_NUM)
	{
		return;	
	}
	
	ATCmdIndex = s_ATCmdStep_Connnect[g_stGPRS_Handler.StepPt];
    /************************AT指令应答正确·**************************/	
	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*AT指令应答内容正确*/
	{	 
        if((GPRS_AT_CAFILE_DATA == ATCmdIndex) ||
           (GPRS_AT_KEYFILE_DATA == ATCmdIndex) ||
           (GPRS_AT_TRUSFILE_DATA == ATCmdIndex) ||
           (GPRS_AT_MQTTWILL_DATA == ATCmdIndex))
        {
            errCnt = 0;
        }
         /*next Step*/
		g_stGPRS_Handler.StepPt++;
		if(g_stGPRS_Handler.StepPt >= CONNECT_STEP_NUM)
		{
			/******GPRS模块状态切换***************/
			g_stGPRS_Handler.State = GPRS_STATE_IDLE;/*切换到下一个流程*/
			g_stGPRS_Handler.StepPt = 0;/**clr step*/
            gprs_status = MQTT_FINISH;
            if(System_Flag.flag_need_charge == 1)
            {
                mode_status = MODE_CHARGEING;
            }
            else
            {
                mode_status = MODE_NO_CHARGE;
            }
            Return_Network();
		}
		else
		{
		    g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;/*继续当前流程*/
		}
	}
    /************************AT 指令达到重发次数**************************/	
	else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)
	{
        if((GPRS_AT_CAFILE_DATA == ATCmdIndex) ||
           (GPRS_AT_KEYFILE_DATA == ATCmdIndex) ||
           (GPRS_AT_TRUSFILE_DATA == ATCmdIndex) ||
           (GPRS_AT_MQTTWILL_DATA == ATCmdIndex))
        {
            if(errCnt >= 3)
            {
                #if DEBUG
	                BSP_LOG("/** AT_CONNECT_ONE RESET**/\n\n");
                #endif 
                g_stGPRS_Handler.State = GPRS_STATE_RESET;       //进行复位
                g_stGPRS_Handler.StepPt = 0;/**clr step*/
                GPRS_NetPar.NetType = NET_NO_NET;	
            }
            else
            {
                errCnt++;
                g_stGPRS_Handler.StepPt--;
                g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET;/*直接再次发送AT指令，然后等待应答*/ 
            }
        }
        else if(GPRS_AT_MIPCALL_1 == ATCmdIndex) //直接开始下一条指令
        {
            g_stGPRS_Handler.StepPt++;
			g_stGPRS_Handler.State = GPRS_STATE_CONNECT_NET; /*继续当前流程*/	
        }
        else
        {
            #if DEBUG
	            BSP_LOG("/** AT_CONNECT_TWO RESET**/\n\n");
            #endif 
            g_stGPRS_Handler.State = GPRS_STATE_RESET;       //进行复位
            g_stGPRS_Handler.StepPt = 0;/**clr step*/
            GPRS_NetPar.NetType = NET_NO_NET;
        }
	}
    /************************AT指令应答错误或超时*************************/	
	else
	{
        g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;   /*直接再次发送AT指令，然后等待应答*/
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/	
        GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stGPRS_Handler.StepPt];  
	}	
}

static void APP_GPRS_State_Comm_Proc(U8 *RxBuf)
{
    U8 ATCmdIndex;  
	static U8 errCnt=0;
    
	if(g_stGPRS_Handler.StepPt >= COMM_STEP_NUM)
	{
		return;	
	} 
	ATCmdIndex = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
    /************************AT指令应答正确·**************************/	
	if(BACK_TRUE == GPRS_ATCmdCB.ATCmdResult)/*AT指令应答内容正确*/
	{
		switch(ATCmdIndex)
		{
			case GPRS_AT_MQTTPUB_DATA:
            {
				/*本次通讯完成,清除对应的事件*/
				GPRS_Comm_Event_Clr((0x01 << g_stGPRS_Handler.ucDataID));
                #if 0
				if(g_stGPRS_Handler.ucCommEvent & COMM_Event_DEV == COMM_Event_DEV)
				{
				    GPRS_Comm_Event_Clr((0x01 << COMM_ID_DEV));
				}
				Time_Count.count_heartbeat = 0;
				#endif 
                errCnt = 0;
                
                
			}break;	
			
			default:
				break;	
		}
		/*next Step*/
		g_stGPRS_Handler.StepPt++;
		if(g_stGPRS_Handler.StepPt >= COMM_STEP_NUM)
		{
			/******GPRS模块状态切换***************/
			/*数据标签来判断是否还有其它数据需要发送，如果有则再次进入comm，没有则退出通讯状态*/
			if(GPRS_Comm_Event_IsEmpty())
			{
                g_stGPRS_Handler.State = GPRS_STATE_IDLE;/*切换到下一个流程*/
				g_stGPRS_Handler.ucCommBusy= 0;/*Comm Complete*/
                gprs_status = MQTT_FINISH;
                //mode_status = MODE_NO_CHARGE;
			}
			else
			{
				g_stGPRS_Handler.State = GPRS_STATE_COMM;/*继续当前流程*/		
			}
			
			g_stGPRS_Handler.StepPt = 0;/**clr step*/
		}
		else
		{
		    g_stGPRS_Handler.State = GPRS_STATE_COMM;/*继续当前流程*/		
		}		
	}
/************************AT 指令达到重发次数**************************/	
	else if(BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult)
	{
        if(GPRS_AT_MQTTPUB_DATA == ATCmdIndex)
        {
            if(errCnt >= 3)
            {
                #if DEBUG
	                BSP_LOG("/** AT_DATA_ONE RESET**/\n\n");
                #endif  
                g_stGPRS_Handler.State = GPRS_STATE_RESET;
                g_stGPRS_Handler.StepPt = 0;/**clr step*/
                g_stGPRS_Handler.ucCommBusy= 0;
                GPRS_NetPar.NetType = NET_NO_NET;
            }
            else
            {
                errCnt++;
                g_stGPRS_Handler.StepPt--;
                g_stGPRS_Handler.State = GPRS_STATE_COMM;/*直接再次发送AT指令，然后等待应答*/ 
            }
        }
        else
        {
            #if DEBUG
	            BSP_LOG("/** AT_DATA_TWO RESET**/\n\n");
            #endif 
            g_stGPRS_Handler.State = GPRS_STATE_RESET;
            g_stGPRS_Handler.StepPt = 0;/**clr step*/
            g_stGPRS_Handler.ucCommBusy= 0;
            GPRS_NetPar.NetType = NET_NO_NET;
        }
	}
/************************AT指令应答错误或超时*************************/	
	else
	{
        /*没有到AT指令重发次数，所以再次发送AT指令*/
        g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;/*直接再次发送AT指令，然后等待应答*/
        GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
        GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
	} 
}

static u8 Transform_Remaining_Time(const char *start_ptr, u8 *remaining_time)
{
    if((*start_ptr>=0x30) && (*start_ptr<=0x39))
    {
        if((*(start_ptr+1)>=0x30) && (*(start_ptr+1)<=0x39))
        {
            if((*(start_ptr+2)>=0x30) && (*(start_ptr+2)<=0x39))
            {
                *remaining_time = ((*start_ptr)-0x30)*100+((*(start_ptr+1))-0x30)*10+(*(start_ptr+2))-0x30;
            }
            else
            {
                *remaining_time = ((*start_ptr)-0x30)*10+(*(start_ptr+1))-0x30;
            }
            return TRUE;
        }
    }
    return FALSE;

}

static bool Set_Remaining_Time(const char *start_ptr)
{
    u8 time;
    
    if(Transform_Remaining_Time(start_ptr,&time))
    {
        remaining_time_count = time*60;
        Write_Data((u8*)(&remaining_time_count),ADDR_REMAINING_TIME,2);
        return TRUE;
    }
    return FALSE;
}

static bool Set_Outage_Time(const char *start_ptr)
{
    u8 time;
    
    if(Transform_Remaining_Time(start_ptr,&time))
    {
        if(time ==0) /*如果下发拔出时间为0,那么设为默认值 10min*/
        {
            time = 10;
        }
        outage_time_count = time*60;
        Write_Data((u8*)(&outage_time_count),ADDR_OUTAGE_TIME,2); 
        return TRUE;
    }
    return FALSE;
}

void APP_COMM_Init(void)
{  
    if(System_Flag.flag_ymode_file == 1)
    {
        g_stGPRS_Handler.State = GPRS_STATE_POWER_ON; 
        g_stGPRS_Handler.StateOld = GPRS_STATE_POWER_ON;
        g_stGPRS_Handler.StepPt = 0;
        GPRS_Comm_Event_Set(COMM_Event_CONN);/*上电需发送一个登录包*/
        //GPRS_Comm_Event_Set(COMM_Event_REPORTED);
        Set_Task(COMM, COMM_STATE_PROC);     /*启动GPRS状态处理任务*/
    }
}

void APP_GPRS_TimeProc(U16 usPeriod)
{

//    if(GPRS_ATCmdCB.Busy == 1)   //在数据接收时，AT指令也忙碌，这边可能会有冲突，把接收时，忙碌标志去除
//	{
		/*********AT指令应答超时**************/
		if(GPRS_ATCmdCB.ulTimeOut >= usPeriod)
        {
            GPRS_ATCmdCB.ulTimeOut -= usPeriod;
            if(GPRS_ATCmdCB.ulTimeOut < usPeriod)
            {
				GPRS_ATCmdCB.ATCmdResult = BACK_TIMEOUT;/*超时*/ 			
            }
        }
//	}
  
	if(GPRS_ATCmdCB.ucByteTimeOut >= usPeriod)
	{
	    GPRS_ATCmdCB.ucByteTimeOut -= usPeriod;
	    if(GPRS_ATCmdCB.ucByteTimeOut < usPeriod)
	    {
            GPRS_ATCmdCB.RxFrameOk = 1;
            Set_Task(COMM, COMM_CMD_PROC);
	    }
	}
    
	/*****************GPRS模块延时计数*************************/
	if(g_stGPRS_Handler.ulDelayCnt >= usPeriod)    
	{
	    g_stGPRS_Handler.ulDelayCnt -= usPeriod;
	}
    else
    {
        g_stGPRS_Handler.ulDelayCnt = 0;
    }
	
    /*****************CPRS 模块状态处理函数*************************/	
    g_stGPRS_Handler.ulProcCtrTaskPer += usPeriod;
    if(g_stGPRS_Handler.ulProcCtrTaskPer >= SUB_TASK_PROCCTR_PER)
    {
        g_stGPRS_Handler.ulProcCtrTaskPer -= SUB_TASK_PROCCTR_PER;
        Set_Task(COMM, COMM_STATE_PROC);
    }
}

/*模块AT指令解析任务*/
void APP_SubTask_CmdProc(void)
{
    disableInterrupts();
    GPRS_ATCmdCB.Busy = 0;   
    GPRS_ATCmdCB.ucRxLen = GPRS_ATCmdCB.ucRxCnt;
    GPRS_ATCmdCB.ucRxCnt = 0;
    GPRS_ATCmdCB.RxFrameOk = 0;
    memcpy(g_stGPRS_Handler.RxBuf,GPRS_ATCmdCB.RxBuf,GPRS_ATCmdCB.ucRxLen);
    enableInterrupts();

    GPRS_ATCmdCB.ATCmdResult = APP_GPRS_ATCmdAckCheck(g_stGPRS_Handler.RxBuf); 
    /**********应答错误直接返回***********************/ 
    if(BACK_ERROR == GPRS_ATCmdCB.ATCmdResult) 
    {
        return;
    }
    /**********应答AT指令直接返回,通过APP_SubTask_StateProc 处理***/
    if(RSP_TYPE_CMD == GPRS_ATCmdCB.RspType)
    {
        return;
    }
    
    /***********对服务器的下行数据进行协议解析和相应处理****************/
    if(RSP_TYPE_NO_NET == GPRS_ATCmdCB.RspType)
    {
        g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State;
        #if DEBUG
	        BSP_LOG("/** AT_NET_COLSE RESET**/\n\n");
        #endif 
        g_stGPRS_Handler.State = GPRS_STATE_RESET; 
        g_stGPRS_Handler.StepPt = 0;
        GPRS_Comm_Event_Set(COMM_Event_CONN);/*上电需发送一个登录包*/
        Set_Task(COMM, COMM_STATE_PROC);     /*启动GPRS状态处理任务*/
        
        #if DEBUG_CONTROLLER
            BSP_LOG("START STATE RESET\r\n");
        #endif
    }  
    /***********对服务器的下行数据进行协议解析和相应处理****************/
    if(RSP_TYPE_DATA == GPRS_ATCmdCB.RspType)
    {
        Set_Task(COMM,COMM_DECODE);
        #if DEBUG_CONTROLLER
            BSP_LOG("START DECODE\r\n");
        #endif
    }  
    GPRS_ATCmdCB.ATCmdResult = NO_BACK;
}

/*模块状态处理任务*/
void APP_SubTask_StateProc(void)
{
    static U8 ResetTimesCount = 0;
    
    switch(g_stGPRS_Handler.State)
    {
         case GPRS_STATE_WAIT_ACK:
         {
              //如果没有结果就直接返回
 			  if(NO_BACK == GPRS_ATCmdCB.ATCmdResult)
			  {
				    return;	
			  }
			  //AT应答超时，或者AT指令应答解析错误，则判断是否需要重发
			  if((GPRS_ATCmdCB.ATCmdResult == BACK_ERROR) || (GPRS_ATCmdCB.ATCmdResult == BACK_TIMEOUT))  
			  {
			        #if DEBUG
	                    BSP_LOG("/** AT_CMD failure**/\n\n");
	                    BSP_LOG("/*** ");
	                    BSP_LOG_Value(GPRS_ATCmdCB.ATCmdPt);
	                    BSP_LOG("**");
	                    BSP_LOG_Value(GPRS_ATCmdCB.ucResendCnt);
	                    BSP_LOG("**");
	                    BSP_LOG_Value(GPRS_ATCmdCB.ucResendTimes);
	                    BSP_LOG(" ***/");
                    #endif
                    if(GPRS_ATCmdCB.ucResendCnt >= GPRS_ATCmdCB.ucResendTimes)
	        	    {	
	        	        GPRS_ATCmdCB.ATCmdResult = BACK_PERTIMES_OUT;/*重发次数到了*/
	        	    }
			  }
			  
			  //根据不同的结果，做相应处理
			  if(APP_GPRS_StateProcTbl[g_stGPRS_Handler.StateOld] != NULL)
		      {
		            APP_GPRS_StateProcTbl[g_stGPRS_Handler.StateOld](g_stGPRS_Handler.RxBuf);		
		      }
              if((BACK_TRUE == GPRS_ATCmdCB.ATCmdResult) || (BACK_PERTIMES_OUT == GPRS_ATCmdCB.ATCmdResult))/*AT指令应答内容正确*/
              {
                     /*更新StateOld*/
                     g_stGPRS_Handler.StateOld = GPRS_STATE_WAIT_ACK;
              }
              GPRS_ATCmdCB.Busy = 0;
              GPRS_ATCmdCB.ulTimeOut = 0;
         }
            break;
         
        case GPRS_STATE_POWER_ON:
         {
              if(g_stGPRS_Handler.ulDelayCnt > 0)
              {
                    return;
              }
              if(0 == g_stGPRS_Handler.StepPt)
			  {
                    /*************************
                        G510 Power on流程：
                         ___        ___
                           |_______|
                              800ms
                        电路转换 高->低；低->高
                    **************************/
                    BSP_GPRS_BUCKEN_SET
				    BSP_GPRS_POWERON_SET;   
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 900;   //拉低850ms
                    #if DEBUG_NETWORK
                        BSP_LOG("/** power on set！**/\n\n");
                    #endif
			  }
			  else if(1 == g_stGPRS_Handler.StepPt)
			  {
			        BSP_GPRS_POWERON_RESET;
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 1000; //等待1S后再操作GPRS模块
                    #if DEBUG_NETWORK
                        BSP_LOG("/** power on rest！**/\n\n");
                    #endif
              }
			  else/*next state*/
			  {
				    /*一些变量清0*/
                    gprs_status = GPRS_INIT;
                    #if DEBUG_NETWORK
                        BSP_LOG("/** power on！**/\n\n");
                    #endif
                    mode_status = MODE_NETWORKING;
                    GPRS_NetPar.NetType = NET_NO_NET;/*无网络状态连接*/
				    g_stGPRS_Handler.StateOld = GPRS_STATE_POWER_ON;	
				    g_stGPRS_Handler.State = GPRS_STATE_INIT;/*开始下一个流程*/
				    g_stGPRS_Handler.StepPt = 0;	
			  }
         }
            break;
         
          /************该状态为GPRS模块POWER_OFF，所以执行完该流程后需POWER_ON ************/
         /**如该流程直接完成模块的复位，那么执行完该流程后,可开始AT指令流程**/
         case GPRS_STATE_RESET:  
         {
              if(g_stGPRS_Handler.ulDelayCnt > 0)
              {
                    return;
              }
              if(0 == g_stGPRS_Handler.StepPt)
			  {
				    //引脚操作
				    G510_Power_DISEN();
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 3500; //拉低3.5S
			  }
			  else if(1 == g_stGPRS_Handler.StepPt)
			  {
			        //引脚操作
			        G510_Power_EN();
				    g_stGPRS_Handler.StepPt++;
				    g_stGPRS_Handler.ulDelayCnt = 3000;
			  }
			  else/*next state*/
			  {
				    /*一些变量清0*/
                    GPRS_NetPar.NetType = NET_NO_NET;/*无网络状态连接*/
                    TASK_State.task_at_reday_flag = DISABLE;
				    g_stGPRS_Handler.StateOld = GPRS_STATE_RESET;	
				    g_stGPRS_Handler.State = GPRS_STATE_POWER_ON;/*开始下一个流程*/
                    GPRS_Comm_Event_Set(COMM_Event_CONN);       //模块复位后，需重新发送登录包
				    g_stGPRS_Handler.StepPt = 0;
                    gprs_status = GPRS_CLOSE;
                    mode_status = MODE_NO_CHARGE;
				    ResetTimesCount++;
				    if(ResetTimesCount >=3)
				    {
				        ResetTimesCount = 0;
                        gprs_status = GPRS_CLOSE;
                        G510_Power_DISEN();
				        TASK_State.task_close_sta_proc_flag = ENABLE;
                        Return_Network();
				    }
                    /**这边添加复位次数计数，如超过设定复位次数，那么暂时不进行对GPRS的操作**/
                    /****具体实现可关闭定时器里面的周期状态处理函数*****/
			  }
         }
            break;   
            
         case GPRS_STATE_INIT:
         {
            if((g_stGPRS_Handler.ulDelayCnt > 0) && (TASK_State.task_at_reday_flag == DISABLE))
            {
                return;
            }
            if(0 == g_stGPRS_Handler.StepPt)
			{
                g_stGPRS_Handler.StepPt++;
                g_stGPRS_Handler.ulDelayCnt = 20000; //20S GPRS AT准备就绪超时时间
			}
			else
			{
                g_stGPRS_Handler.ulDelayCnt = 0;
                g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State;
                g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
                GPRS_ATCmdCB.SendATCmdEn = 1; /*enable send ATCmd*/
                GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Init[g_stGPRS_Handler.StepPt]; 
			}
         }
            break; 
          
         case GPRS_STATE_CONNECT_NET:
		 {
			g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
			g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
			GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Connnect[g_stGPRS_Handler.StepPt];
         }
            break;
         
         case GPRS_STATE_READ_MESS:
         {
            g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
			g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
			GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Mess[g_stGPRS_Handler.StepPt];
         }
            break;
         
         case GPRS_STATE_COMM:
         {
            /*判断是否有数据需重发*/
            if(GPRS_Retry_Event_IsEmpty())
            {
                 /*判断是否有数据需要发送*/
                if(GPRS_Comm_Event_IsEmpty())
			    {
                      break;
			    }
            }
            else
            {
                if(GPRS_RetryCtrol.rtyEn == DISABLE)
                {
                      break;
                }
            }
           
			if(0 == g_stGPRS_Handler.StepPt)
			{
				g_stGPRS_Handler.ucCommBusy = 1;/*标记正在和服务器通讯*/
			}
            
            if(GPRS_AT_MQTTPUB == s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt])
            {
			    if(GPRS_Retry_Event_IsEmpty())
			    {
                    /*获取当前需要发送的数据类型*/
				    #if DEBUG_CONTROLLER
                        BSP_LOG("start send mess\r\n");
                    #endif
				    g_stGPRS_Handler.ucDataID = Get_CommEvent(g_stGPRS_Handler.ucCommEvent);
				}
				else
				{
                    #if DEBUG_CONTROLLER
                        BSP_LOG("retry send mess\r\n");
                    #endif
				    //g_stGPRS_Handler.ucDataID = Get_CommEvent(GPRS_RetryCtrol.rtyEvent);
				}
				/*封装协议层的帧内容*/
				g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WritePUBPacket(g_stGPRS_Handler.ucDataID, g_stGPRS_Handler.TxBuf);
            }
			else if(GPRS_AT_MQTTPUB_DATA == s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt])
			{
			    if(GPRS_Retry_Event_IsEmpty())
			    {
                    /*获取当前需要发送的数据类型*/
				    #if DEBUG_CONTROLLER
                        BSP_LOG("start send mess data\r\n");
                    #endif
				    g_stGPRS_Handler.ucDataID = Get_CommEvent(g_stGPRS_Handler.ucCommEvent);
				}
				else
				{
                    #if DEBUG_CONTROLLER
                        BSP_LOG("retry send mess data\r\n");
                    #endif
				    //g_stGPRS_Handler.ucDataID = Get_CommEvent(GPRS_RetryCtrol.rtyEvent);
				}
				/*封装协议层的帧内容*/
				g_stGPRS_Handler.ucTxLen = (U8)APP_GPRS_WriteDataIDPacket(g_stGPRS_Handler.ucDataID, g_stGPRS_Handler.TxBuf);
			}
            
            //此时重发计数为3，并且为第一条指令。因为第二条指令会把重传计数加1
			//if((GPRS_RetryCtrol.rtycnt >= RTY_TIMES_MAX) && (g_stGPRS_Handler.StepPt ==0))   
			//{
			//    g_stGPRS_Handler.StateOld = GPRS_STATE_RESET;   //复位GPRS模块
			//    g_stGPRS_Handler.State = GPRS_STATE_RESET;
			//    g_stGPRS_Handler.StepPt = 0;                
			//    APP_GPRS_RetryMessInit();                   //重传两次失败后，则清除重传事件，并复位GPRS模块
			//    return;
			//}
            
			g_stGPRS_Handler.StateOld = g_stGPRS_Handler.State; 
			g_stGPRS_Handler.State = GPRS_STATE_WAIT_ACK;
			GPRS_ATCmdCB.SendATCmdEn = 1;/*enable send ATCmd*/
			GPRS_ATCmdCB.ATCmdPt = s_ATCmdStep_Comm[g_stGPRS_Handler.StepPt];
         }
            break;  
            
         case GPRS_STATE_IDLE:
		 {
			U8 NextStateOffset;
             /*判断是否有数据需要通讯*/
			if(GPRS_Comm_Event_IsEmpty() == FALSE || GPRS_Retry_Event_IsEmpty() == FALSE)
			{
			    /*有数据要通讯则设置切换到COM状态的事件*/
                GPRS_State_Event_Set(STATE_Event_COMM);
			}
            
			if(GPRS_IsSwitch_State())  //现不做处理，后续可用做低功耗
			{
                break;
			}
			
			/*获取下一个优先级高的状态*/
            NextStateOffset = Get_NextState(g_stGPRS_Handler.ucStateEvent);
			g_stGPRS_Handler.State = GPRS_STATE_COMM + NextStateOffset;
			GPRS_State_Event_Clr(0x01 << NextStateOffset);
	        g_stGPRS_Handler.StateOld = GPRS_STATE_IDLE; 
			g_stGPRS_Handler.StepPt = 0;
		 }
            break;
             
         default:
		 {
		    g_stGPRS_Handler.State = GPRS_STATE_INIT;
			g_stGPRS_Handler.StepPt = 0;
		 }
            break;
    }
    
    if(1 == GPRS_ATCmdCB.SendATCmdEn)
    {
        GPRS_ATCmdCB.SendATCmdEn = 0;
        APP_GPRS_SendATCmd(&GPRS_ATCmdCB);
    }
}

void APP_SuTask_DecodeProc(void)
{
    U8 *rxBuff = &g_stGPRS_Handler.RxBuf[Get_Symbol(2, g_stGPRS_Handler.RxBuf, ',')];
    u8 data;
    char *start_ptr = NULL;
    char *start_ptr2 = NULL;
                
    start_ptr = strstr((char*)rxBuff, "\"state\":");

    if(start_ptr != NULL)
    {
        start_ptr = strstr((start_ptr+8), "\"desired\":");
        if(start_ptr != NULL)
        {
            start_ptr = strstr((start_ptr+11), "\"cmd\":");
            if(start_ptr != NULL)
            {
                start_ptr2 = strstr((start_ptr+6), "\"command\",");
                if(start_ptr2 != NULL)
                {
                    start_ptr = strstr((start_ptr2+10), "\"state\":");
                    start_ptr += 8;
                    if((*start_ptr) == 0x31)
                    {
                        start_ptr = strstr(start_ptr2, "\"remaining_time\":");
                        if(start_ptr != NULL)
                        {
                            start_ptr += 18;
                            if(Set_Remaining_Time(start_ptr))
                            {
                                start_ptr = strstr(start_ptr2, "\"outage_time\":");
                                if(start_ptr != NULL)

                                {
                                    start_ptr += 15;
                                    if(Set_Outage_Time(start_ptr))
                                    {
                                        System_Flag.flag_need_charge = 1;
                                        TASK_State.task_control_charge_open_flag = 1;
                                        hour_time_count = 0;
                                        min_time_count = 0;
                                        data = 0x01;
                                        Write_Data(&data,ADDR_CHARGE,1);
                                        System_Flag.flag_return_com_result = 0;
                                        mode_status = MODE_CHARGEING;
                                    }
                                }

                            }
                            else
                            {
                                System_Flag.flag_return_com_result = 1;
                            }
                            
                            GPRS_Comm_Event_Set(COMM_Event_COM); /*发送指令返回包*/
                            Set_Task(COMM, COMM_STATE_PROC);     /*启动GPRS状态处理任务*/
                        }
                    }
                    else
                    {
                        start_ptr = strstr(start_ptr2, "\"remaining_time\":");
                        if(start_ptr != NULL)
                        {
                            u8 time;
                            start_ptr += 18;
                            
                            if (Transform_Remaining_Time(start_ptr,&time))
                            {
                                if(!time)
                                {
                                    Set_Task(CONTROL,CONTROL_CHARGE_CLOSE);
                                    TASK_State.task_measure_usb_insert_flag = 0;
                                    TASK_State.task_measure_usb_pullout_flag = 0;
                                    System_Flag.flag_need_charge = 0;
                                    mode_status = MODE_NO_CHARGE;
                                    remaining_time_count = 0;
                                    Write_Data((u8*)(&remaining_time_count),ADDR_REMAINING_TIME,2);
                                    
                                    remaining_time_count = 0;
                                    System_Flag.flag_return_com_result = 0;
                                    data = 0x00;
                                    Write_Data(&data,ADDR_CHARGE,1); 
                                }
                                else
                                {
                                    System_Flag.flag_return_com_result = 1;
                                }
                                GPRS_Comm_Event_Set(COMM_Event_COM); /*发送指令返回包*/
                                Set_Task(COMM, COMM_STATE_PROC);     /*启动GPRS状态处理任务*/
                            }
                        }
                    }
                }
                else
                {
                    start_ptr2 = strstr((start_ptr+6), "\"ota\",");
                    if(start_ptr2 != NULL)
                    {
                        GPRS_Comm_Event_Set(COMM_Event_OTA); /*发送指令返回包*/
                        Set_Task(COMM, COMM_STATE_PROC);     /*启动GPRS状态处理任务*/
                    }
                }
            }
        }
    }
}
