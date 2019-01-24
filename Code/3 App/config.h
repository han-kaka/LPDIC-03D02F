#ifndef  __CONFIG_H
#define  __CONFIG_H
/******************* version control *******************/

#define SOFTWARE_VERSION            "LPDIC-03D02H_Rev01_180627" 

#define NO_INIT                     __no_init

////////////////  客户定义  //////////////////
#define LSD                         1
#define SL                          0  

///////////////////////////////////////////////////////////////////////////////
//////////////////////////////  通讯  ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#if( SL || LSD)
  #define FE_NUM                    3  
#else
  #define FE_NUM                    0  
#endif

#define		USE_VERSION_GPRS	    0

#if USE_VERSION_GPRS == 1
	#define   	VERSION_GPRS        1
#else
	#define		VERSION_KEY         1
#endif

/****************** debug control  *******************/
#define      DEBUG				    0

#if DEBUG
    #define      DEBUG_LED          1
    #define      DEBUG_KEY          1
    #define      DEBUG_CHARGE       1
    #define      DEBUG_ERROR        1
    #define      DEBUG_LOWPOWER     1
    #define      DEBUG_YMODEM       0 
    #define      DEBUG_NETWORK      1 
#endif

/****************** Version control  *******************/
#define  SOFT_VERSION    		    0x02

/****************** const define  *******************/
//#define  TOTAL_CHARGE_TIME		    3600		// s
//#define  PULLOUT_TIME_MAX		    180			// s

#define  ADC_VALUE_ZERO_MAX         12

#define  UART_RECIEVE_TIMEOUT       5           //100ms

/****************** test define  *******************/
#define  CHARGE_ENABLE              0x01
#define  CHARGE_DISABLE             0x02

#define  VOLTAGE_NULL               0x00
#define  VOLTAGE_DC5V               0x01
#define  VOLTAGE_VBAT4_2V           0x02
#define  VOLTAGE_USB                0x04
#define  VOLTAGE_VCC                0x08
#define  VOLTAGE_ALL                0x1F

/******************* file address *********************/
#define  CERTIFICATE_FILE_ADDR     0x017800
#define  PRIVATE_FILE_ADDR         0x017000
#define  CA_FILE_ADDR              0x016800

/******************* file class *********************/
#define  CERTIFICATE_FILE          0x01
#define  PRIVATE_FILE              0x02
#define  CA_FILE                   0x03

#define MemSet(adr,len,data)    for(u16 ii = 0; ii < len; ++ii)\
                                *(adr+ii) = data;                           //向数据adr中填充长度为len个数据data

#endif
