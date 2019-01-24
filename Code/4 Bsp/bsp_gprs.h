#ifndef __BSP_GPRS_H
#define __BSP_GPRS_H

#include "global.h" 

/*
#define  AT             "AT\r\n"
#define  SIMSTATUS      "AT+CPIN?\r\n"
#define  ECHO			"ATE0\r\n"
#define  SIGNAL         "AT+CSQ?\r\n"
#define  AT_REGISTER	"AT+CGREG?\r\n"
#define  IPALLOC        "AT+MIPCALL=1,\"CMNET\"\r\n"
#define  QTLSVER        "AT+GTSSLVER?\r\n"
#define  STLSVER        "AT+GTSSLVER=4\r\n"
#define  CAFILE         "AT+GTSSLFILE=\"CAFILE\",1220\r\n"
#define  KEYFILE        "AT+GTSSLFILE=\"KEYFILE\",1679\r\n"
#define  TRUSTFILE      "AT+GTSSLFILE=\"TRUSTFILE\",1758\r\n"



#define  AT_REGISTER	"AT+CGREG?\r\n"
#define  AT_ALLOC_IP	"AT+MIPCALL=1,\"CMNET\"\r\n"
#define  MQTT_USERNAME 	"MQTT"
#define  MQTT_PASSWORD	"MQTTPW"
#define	 MQTT_USER_INFO	"AT+MQTTUSER=1,\"MQTT\",\"MQTTPW\"\r\n"
#define  MQTT_CONNECTED	"AT+MQTTOPEN=1,\"a7iko9biw0dza.iot.cn-north-1.amazonaws.com.cn\",8883,1,60\r\n"
#define  MQTT_MQTTSUB   "AT+MQTTSUB=1,\"$aws/things/LPD_XC000000/shadow/update/accepted\",0\r\n"
*/



void G510_Power_EN(void);
void G510_Power_DISEN(void);
void Bsp_G510_Init(void);
void G510_Power_ON(void);
void G510_Power_OFF(void);
void Command_Send_G510(u8 *p_data,u16 len);

#endif