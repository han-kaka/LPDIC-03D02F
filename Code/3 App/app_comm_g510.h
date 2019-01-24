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

/***********�������������������*************************/
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

/*********GPRSģ��Ĺ���״̬****************/
typedef enum 
{
    GPRS_STATE_POWER_ON = 0,
    GPRS_STATE_RESET,
    GPRS_STATE_INIT,      /*����AT��ʼ������*/
	GPRS_STATE_CONNECT_NET,/*TCP/IP������·��������*/
	GPRS_STATE_READ_MESS,
	GPRS_STATE_COMM,
	GPRS_STATE_IDLE,
	GPRS_STATE_WAIT_ACK,
	GPRS_STATE_MAX,
   
}GPRS_STATE;

/*********GPRSģ��ͨѶ������ID**********/
typedef enum 
{
	COMM_ID_CONN = 0,   /*���͵�¼��*/
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
	COMM_Event_CONN         = (0x01 << COMM_ID_CONN),                  /*�������Ӱ�*/
	COMM_Event_COM          = (0x01 << COMM_ID_COM),                   /*���Ϳ��Ʒ���Ӧ���*/
    COMM_Event_DISCONN      = (0x01 << COMM_ID_DISCONN),               /*���ͶϿ����Ӱ�*/
    COMM_Event_REPORTED     = (0x01 << COMM_ID_REPORTED),
    COMM_Event_DEV          = (0x01 << COMM_ID_DEV),                   /*���Ͷ�ʱ������*/
    COMM_Event_OTA          = (0x01 << COMM_ID_OTA),                   /*�̼�������*/
}GPRS_COMM_Event;

/*State switch Event*/
typedef enum 
{
	STATE_Event_COMM   = BIT0,     /*go to Comm*/
    STATE_Event_MAX,
	
}GPRS_STATE_Event;

/*******GPRSӦ������*************/
typedef enum 
{
    RSP_TYPE_CMD = 0, /*Ӧ��ATָ��*/
    RSP_TYPE_DATA,    /*ͨ������*/
    RSP_TYPE_NO_NET,    /*ͨ������*/
	RSP_TYPE_MAX,

}RSP_TYPE;

typedef struct
{
	const S8* CmdStr;                /*ָ���*/
	U16 usTimeOut;                   /*���ճ�ʱ*/
	const S8* ExpResultStr;          /*�������*/
	U8 ucResendTimes;                /*�ط�����*/
		
}GPRS_Cmd_Data_Type; 

/*********ATִ�н��***********/
typedef enum 
{
    NO_BACK = 0,
    BACK_TIMEOUT,           /*ATָ��Ӧ��ʱ*/
    BACK_ERROR,             /*ATָ��Ӧ�����*/
	BACK_PERTIMES_OUT,      /*ATָ���ط���������*/
	BACK_TRUE,              /*ATָ��Ӧ��������ȷ*/
	
}AT_RESULT_TYPE;

/**********GPRSָ�*****************/
typedef enum 
{

    GPRS_AT=0,/************************* ��ʼ������***************************/
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

/*******��������*************/
typedef enum 
{
    NET_NO_NET=0,
    NET_GPRS,
	NET_TYPE_MAX,
	
}NET_TYPE;

/*�������*/
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
    //DEVICE_STATE_Type DeviceState;   //�豸״̬
    U8 CSQ;                          //GPRS�ź�ǿ��
    U8 hardwareVerLen;               //Ӳ���汾�ų���
    U8 hardwareVer[4];               //Ӳ���汾��
    U8 softwareVerLen;               //����汾�ų���
    U8 softwareVer[4];               //����汾��
    
}MESS_READ_Type;

/*********ͨѶ�����ݰ�����**************/
typedef struct
{
	//AUTH_DATA_Type AuthData;
	MESS_READ_Type MessRead;
		
}COMM_Packet_Type;

typedef struct
{
    U8  rtyEn;                                          //�ش�ʹ��
    U8  rtycnt;                                         //�ش�����
    U8  rtycntEn;                                       //�ش�����ʹ��
    U8  rtyBuff[GPRS_RTY_MESS_LEN];                     //�ش���Ϣ����
    U8  rtyLen;                                         //�ش���Ϣ����
    U16 rtyEvent;                                       //�ش��¼�
    U16 rtyTime;                                        //�ش�ʱ��
}Retry_Ctr_Type;

/*******ATָ��ͨѶ���ƿ�**********/
typedef struct
{
    U8 SendATCmdEn;                 /*enable send ATCmd*/
	U8 ATCmdPt;                     /*��ǰAT��������*/
	U8 ucResendCnt;                 /*�ط�����*/	
	U8 ucResendTimes;               /*�ط�����*/
	U32 ulTimeOut;                  /*���ճ�ʱ*/
	U8 ATCmdResult;                 /*��ǰATָ��ͨѶ���*/
	BOOL Busy;                      /*MCU��ģ��ͨѶæµ��־*/
	U8 CmdStr[GPRS_RX_BUFF_LEN];    /*��ǰ���͵�ָ���*/
	U8 ucCmdStrLen;                 /*��ǰָ���*/
	S8 ExpResultStr[20];               /*��ǰ���͵�ָ������������*/
	U16 ucByteTimeOut;               /*�ֽڳ�ʱ*/
	U8 RxBuf[GPRS_RX_BUFF_LEN];     /*�������ݻ���*/
	U16 ucRxCnt;                     /*�������ݼ���*/
	U8 ucRxLen;                     /*�������ݳ���*/
	U8 RxFrameOk;                   /*ָʾһ֡��������*/
    U8 RspType;                     /*NBģ��Ӧ������*/

}AT_CMD_CB;

/*******GPRSģ����ƿ�**********/
typedef struct
{
	U8 State;                       /*ģ�鵱ǰ״̬*/
	U8 StateOld;                    /*ģ����һ��״̬*/
	U8 StepPt;                      /*step ponit*/
	U16 ulDelayCnt;                 /*��ʱ����*/
	U16 ulProcCtrTaskPer;           /*״̬�����������*/
	U16 ucCommEvent;                /*�����Ҫ���͵���������*/
	U8 ucStateEvent;                /*��ǰ���¼�״̬*/
	U8 ucDataID;                    /*������ڷ��͵���ϢID*/
	U8 ucCommBusy;                  /*ͨ��æµ���*/
	U8 RxBuf[GPRS_RX_BUFF_LEN];     /*Ӧ�ò���յ�������*/
	U8 TxBuf[GPRS_RX_BUFF_LEN];     /*Ӧ�ò���Ҫ���͵�����*/
	U8 ucTxLen;                     /*Ӧ�ò㷢�����ݳ���*/              
	Net_Parameters NetPar;          /*ģ������������Ϣ*/
    COMM_Packet_Type CommPacket;    /*ͨѶ���ݰ�*/
    Retry_Ctr_Type RetryCtrol;      /*�ط�����*/
    AT_CMD_CB ATCmdCB;              /*******ATָ��ͨѶ���ƿ�**********/
	
}GPRS_Handler;

#define GPRS_NetPar                    (g_stGPRS_Handler.NetPar)
#define GPRS_RetryCtrol                (g_stGPRS_Handler.RetryCtrol)
#define GPRS_ATCmdCB                   (g_stGPRS_Handler.ATCmdCB)
#define GPRS_CommPacket                (g_stGPRS_Handler.CommPacket)

/*************����������̵�step*******************/
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
