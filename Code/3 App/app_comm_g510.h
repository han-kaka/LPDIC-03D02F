#ifndef __APP_COMM_G510_H
#define __APP_COMM_G510_H

#include "global.h"

#define  GPRS_CLOSE		0
#define  GPRS_INIT		1
#define  MQTT_FINISH   	2

#define  MQTT_PUB_COM   "AT+MQTTPUB=1,\"$aws/things/LPD_XC000000/shadow/update\",0,0,126\r\n"
#define  MQTT_COM_DATA1 "{\"state\":{\"reported\":{\"cmd\":\"command\",\"id\":\""
#define  MQTT_COM_DATA2 "\",\"remaining_time\":\""
#define  MQTT_COM_DATA3 "\",\"outage_time\":\""
#define  MQTT_COM_DATA4 "\",\"state\":"
#define  MQTT_COM_DATA5 ",\"result\":"
#define  MQTT_COM_DATA6 ",\"iccid\":\""
#define  MQTT_COM_DATA7 "\"}}}"

#define  MQTT_PUB_REPORTED   "AT+MQTTPUB=1,\"$aws/things/LPD_XC000000/subscription\",0,0,72\r\n"
#define  MQTT_REPORTED_DATA1 "{\"state\":{\"reported\":{\"cmd\":\"power\",\"id\":\""
#define  MQTT_REPORTED_DATA2 "\",\"state\":\"off\"}}}"
//#define  MQTT_REPORTED_DATA2 "\",\"state\":0"
#define  MQTT_REPORTED_DATA3 ",\"fireware_ver\":\"1.0.0\",\"remaining_time\":\""
#define  MQTT_REPORTED_DATA4 "\",\"iccid\":\""
#define  MQTT_REPORTED_DATA5 "\"}}}"



#define  MQTT_PUB_DEV   "AT+MQTTPUB=1,\"$aws/things/LPD_XC000000/shadow/update\",0,0,142\r\n"
#define  MQTT_DEV_DATA1 "{\"state\":{\"reported\":{\"cmd\":\"dev\",\"id\":\""
#define  MQTT_DEV_DATA2 "\",\"state\":"
#define  MQTT_DEV_DATA3 ",\"fireware_ver\":\"1.0.0\",\"remaining_time\":\""
#define  MQTT_DEV_DATA4 "\",\"iccid\":\""
#define  MQTT_DEV_DATA5 "\"}}}"

#define  MQTT_PUB_WILL   "AT+MQTTPUB=1,\"$aws/things/LPD_XC000000/subscription\",0,0,149\r\n"
#define  MQTT_WILL_DATA1 "{\"state\":{\"reported\":{\"cmd\":\"disconnect\",\"id\":\""
#define  MQTT_WILL_DATA2 "\",\"state\":"
#define  MQTT_WILL_DATA3 ",\"fireware_ver\":\"1.0.0\",\"remaining_time\":\""
#define  MQTT_WILL_DATA4 "\",\"iccid\":\""
#define  MQTT_WILL_DATA5 "\"}}}"

#define  MQTT_PUB_CONN   "AT+MQTTPUB=1,\"$aws/things/LPD_XC000000/subscription\",0,0,146\r\n"
#define  MQTT_CONN_DATA1 "{\"state\":{\"reported\":{\"cmd\":\"connect\",\"id\":\""
#define  MQTT_CONN_DATA2 "\",\"state\":"
#define  MQTT_CONN_DATA3 ",\"fireware_ver\":\"1.0.0\",\"remaining_time\":\""
#define  MQTT_CONN_DATA4 "\",\"iccid\":\""
#define  MQTT_CONN_DATA5 "\"}}}"

#define  MQTT_PUB_DISCONN   "AT+MQTTPUB=1,\"$aws/things/LPD_XC000000/subscription\",0,0,149\r\n"
#define  MQTT_DISCONN_DATA1 "{\"state\":{\"reported\":{\"cmd\":\"disconnect\",\"id\":\""
#define  MQTT_DISCONN_DATA2 "\",\"state\":"
#define  MQTT_DISCONN_DATA3 ",\"fireware_ver\":\"1.0.0\",\"remaining_time\":\""
#define  MQTT_DISCONN_DATA4 "\",\"iccid\":\""
#define  MQTT_DISCONN_DATA5 "\"}}}"

/* Bit defined */
#define BIT0			(0x01)
#define BIT1			(0x02)
#define BIT2			(0x04)
#define BIT3			(0x08)
#define BIT4			(0x10)
#define BIT5			(0x20)
#define BIT6			(0x40)
#define BIT7			(0x80)

/***********各个子任务的运行周期*************************/
#define SUB_TASK_PROCCTR_PER          (10U) /*10ms*/

/*********************BUFF LEN***********************/
#define GPRS_RTY_MESS_LEN         (40)
#define GPRS_RX_BUFF_LEN          (256)

#define GPRS_SendATCmdStr(pTxBuff, usLen)	      BSP_UART_Transmit(pTxBuff, usLen)

/*switch state*/
#define GPRS_IsSwitch_State()                       !READ_REG(g_stGPRS_Handler.ucStateEvent)
#define GPRS_State_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.ucStateEvent, Event)
#define GPRS_State_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.ucStateEvent, Event)
#define GPRS_State_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.ucStateEvent, Event)

/*Comm Event*/
#define GPRS_Comm_Event_IsEmpty()                   !READ_REG(g_stGPRS_Handler.ucCommEvent)
#define GPRS_Comm_Event_Get(Event)                  READ_BIT(g_stGPRS_Handler.ucCommEvent, Event)
#define GPRS_Comm_Event_Set(Event)                  SET_BIT(g_stGPRS_Handler.ucCommEvent, Event)
#define GPRS_Comm_Event_Clr(Event)                  CLEAR_BIT(g_stGPRS_Handler.ucCommEvent, Event)

/*Comm Retry  Event*/
#define GPRS_Retry_Event_IsEmpty()                  !READ_REG(g_stGPRS_Handler.RetryCtrol.rtyEvent)
#define GPRS_Retry_Event_Get(Event)                 READ_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
#define GPRS_Retry_Event_Set(Event)                 SET_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)
#define GPRS_Retry_Event_Clr(Event)                 CLEAR_BIT(g_stGPRS_Handler.RetryCtrol.rtyEvent, Event)

#define Get_CommEvent(Event)                        CntLeadZeros(Event)
#define Get_NextState(Event)                        CntLeadZeros(Event)

/*********GPRS模块的工作状态****************/
typedef enum 
{
    GPRS_STATE_POWER_ON = 0,
    GPRS_STATE_RESET,
    GPRS_STATE_INIT,      /*进入AT初始化流程*/
	GPRS_STATE_CONNECT_NET,/*TCP/IP数据链路建立流程*/
	GPRS_STATE_READ_MESS,
	GPRS_STATE_COMM,
	GPRS_STATE_IDLE,
	GPRS_STATE_WAIT_ACK,
	GPRS_STATE_MAX,
   
}GPRS_STATE;

/*********GPRS模块通讯的数据ID**********/
typedef enum 
{
	COMM_ID_CONN = 0,   /*发送登录包*/
    COMM_ID_COM,
    COMM_ID_DISCONN,
    COMM_ID_REPORTED,
    COMM_ID_DEV,
    COMM_ID_OTA,
	COMM_ID_MAX,
	COMM_ID_INVALID = 0xFF,
}GPRS_COMM_ID;

typedef enum 
{
	COMM_Event_CONN         = (0x01 << COMM_ID_CONN),                  /*发送连接包*/
	COMM_Event_COM          = (0x01 << COMM_ID_COM),                   /*发送控制返回应答包*/
    COMM_Event_DISCONN      = (0x01 << COMM_ID_DISCONN),               /*发送断开连接包*/
    COMM_Event_REPORTED     = (0x01 << COMM_ID_REPORTED),
    COMM_Event_DEV          = (0x01 << COMM_ID_DEV),                   /*发送定时心跳包*/
    COMM_Event_OTA          = (0x01 << COMM_ID_OTA),                   /*固件升级包*/
}GPRS_COMM_Event;

/*State switch Event*/
typedef enum 
{
	STATE_Event_COMM   = BIT0,     /*go to Comm*/
    STATE_Event_MAX,
	
}GPRS_STATE_Event;

/*******GPRS应答类型*************/
typedef enum 
{
    RSP_TYPE_CMD = 0, /*应答AT指令*/
    RSP_TYPE_DATA,    /*通信数据*/
    RSP_TYPE_NO_NET,    /*通信数据*/
	RSP_TYPE_MAX,

}RSP_TYPE;

typedef struct
{
	const S8* CmdStr;                /*指令符*/
	U16 usTimeOut;                   /*接收超时*/
	const S8* ExpResultStr;          /*期望结果*/
	U8 ucResendTimes;                /*重发次数*/
		
}GPRS_Cmd_Data_Type; 

/*********AT执行结果***********/
typedef enum 
{
    NO_BACK = 0,
    BACK_TIMEOUT,           /*AT指令应答超时*/
    BACK_ERROR,             /*AT指令应答错误*/
	BACK_PERTIMES_OUT,      /*AT指令重发次数到了*/
	BACK_TRUE,              /*AT指令应答内容正确*/
	
}AT_RESULT_TYPE;

/**********GPRS指令集*****************/
typedef enum 
{

    GPRS_AT=0,/************************* 初始化流程***************************/
	GPRS_AT_ATE0,
	//GPRS_AT_GTSET,
    GPRS_AT_CGREG1,
    
    GPRS_AT_CSQ,
    GPRS_AT_CCID,
    
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
    
    GPRS_AT_MQTTPUB,  
    GPRS_AT_MQTTPUB_DATA,
    
    GPRS_AT_MQTTMSG,

    GPRS_AT_MQTTLOSE,
}AT_CMD_Type;

/*******网络类型*************/
typedef enum 
{
    NET_NO_NET=0,
    NET_GPRS,
	NET_TYPE_MAX,
	
}NET_TYPE;

/*网络参数*/
typedef struct
{
	NET_TYPE NetType;
	//NET_PROTOCOL NetProtocol;
	//U8 ServerApn[SERVER_APN_LEN];
	//U8 ServerIp[SERVER_IP_LEN];
    //U16 ServerPort;
    //U16 ulReportPeriod; 
}Net_Parameters;

typedef struct
{
    //DEVICE_STATE_Type DeviceState;   //设备状态
    U8 CSQ;                          //GPRS信号强度
    U8 hardwareVerLen;               //硬件版本号长度
    U8 hardwareVer[4];               //硬件版本号
    U8 softwareVerLen;               //软件版本号长度
    U8 softwareVer[4];               //软件版本号
    
}MESS_READ_Type;

/*********通讯的数据包类型**************/
typedef struct
{
	//AUTH_DATA_Type AuthData;
	MESS_READ_Type MessRead;
		
}COMM_Packet_Type;

typedef struct
{
    U8  rtyEn;                                          //重传使能
    U8  rtycnt;                                         //重传计数
    U8  rtycntEn;                                       //重传计数使能
    U8  rtyBuff[GPRS_RTY_MESS_LEN];                     //重传消息备份
    U8  rtyLen;                                         //重传消息长度
    U16 rtyEvent;                                       //重传事件
    U16 rtyTime;                                        //重传时间
}Retry_Ctr_Type;

/*******AT指令通讯控制块**********/
typedef struct
{
    U8 SendATCmdEn;                 /*enable send ATCmd*/
	U8 ATCmdPt;                     /*当前AT命令索引*/
	U8 ucResendCnt;                 /*重发计数*/	
	U8 ucResendTimes;               /*重发次数*/
	U32 ulTimeOut;                  /*接收超时*/
	U8 ATCmdResult;                 /*当前AT指令通讯结果*/
	BOOL Busy;                      /*MCU与模块通讯忙碌标志*/
	U8 CmdStr[GPRS_RX_BUFF_LEN];    /*当前发送的指令符*/
	U8 ucCmdStrLen;                 /*当前指令长度*/
	S8 ExpResultStr[20];               /*当前发送的指令符的期望结果*/
	U16 ucByteTimeOut;               /*字节超时*/
	U8 RxBuf[GPRS_RX_BUFF_LEN];     /*接收数据缓存*/
	U16 ucRxCnt;                     /*接收数据计数*/
	U8 ucRxLen;                     /*接收数据长度*/
	U8 RxFrameOk;                   /*指示一帧接收完整*/
    U8 RspType;                     /*NB模块应答类型*/

}AT_CMD_CB;

/*******GPRS模块控制块**********/
typedef struct
{
	U8 State;                       /*模块当前状态*/
	U8 StateOld;                    /*模块上一个状态*/
	U8 StepPt;                      /*step ponit*/
	U16 ulDelayCnt;                 /*延时计数*/
	U16 ulProcCtrTaskPer;           /*状态函数处理计数*/
	U16 ucCommEvent;                /*标记需要发送的数据类型*/
	U8 ucStateEvent;                /*当前的事件状态*/
	U8 ucDataID;                    /*标记正在发送的消息ID*/
	U8 ucCommBusy;                  /*通信忙碌标记*/
	U8 RxBuf[GPRS_RX_BUFF_LEN];     /*应用层接收到的数据*/
	U8 TxBuf[GPRS_RX_BUFF_LEN];     /*应用层需要发送的数据*/
	U8 ucTxLen;                     /*应用层发送数据长度*/              
	Net_Parameters NetPar;          /*模块网络连接信息*/
    COMM_Packet_Type CommPacket;    /*通讯数据包*/
    Retry_Ctr_Type RetryCtrol;      /*重发控制*/
    AT_CMD_CB ATCmdCB;              /*******AT指令通讯控制块**********/
	
}GPRS_Handler;

#define GPRS_NetPar                    (g_stGPRS_Handler.NetPar)
#define GPRS_RetryCtrol                (g_stGPRS_Handler.RetryCtrol)
#define GPRS_ATCmdCB                   (g_stGPRS_Handler.ATCmdCB)
#define GPRS_CommPacket                (g_stGPRS_Handler.CommPacket)

/*************定义各个流程的step*******************/
#define INIT_STEP_NUM        NumberOfElements(s_ATCmdStep_Init)
#define CONNECT_STEP_NUM     NumberOfElements(s_ATCmdStep_Connnect)
#define MESS_STEP_NUM        NumberOfElements(s_ATCmdStep_Mess)
#define COMM_STEP_NUM        NumberOfElements(s_ATCmdStep_Comm)

#define SYS_SoftVerMessLen   GPRS_CommPacket.MessRead.softwareVerLen
#define SYS_SoftVerMess      GPRS_CommPacket.MessRead.softwareVer
#define SYS_HardVerMessLen   GPRS_CommPacket.MessRead.hardwareVerLen
#define SYS_HardVerMess      GPRS_CommPacket.MessRead.hardwareVer

extern GPRS_Handler        g_stGPRS_Handler;
extern u8                  gprs_status;

void APP_COMM_Init(void);
void APP_GPRS_TimeProc(U16 usPeriod);

void APP_SubTask_CmdProc(void);
void APP_SubTask_StateProc(void);
void APP_SuTask_DecodeProc(void);

#endif
