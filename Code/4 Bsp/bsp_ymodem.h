#ifndef __BSP_YMODEM_H
#define __BSP_YMODEM_H

#include "global.h"

#define SOH  0x01
#define STX  0x02

#define NAK  0x15
#define ACK  0x06
#define CAN  0x18
#define EOT  0x04

#define  YMODEM_START   0
#define  YMODEM_TRANS   1
#define  YMODEM_END     2



typedef struct
{
    u32 file_length;
    u32 fname_length;
    u8 total_package;
    u8 package_num;
}Ymodem_Info_TypeDef;





void bsp_ymodem_trans(u8* p_src);

#endif
 