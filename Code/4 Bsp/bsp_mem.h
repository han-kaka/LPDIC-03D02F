#ifndef __BSP_MEM_H
#define __BSP_MEM_H

#include "global.h"

/**********************************************************************************/
#define ADDR_VERSION                       0x0001        //len  01
#define ADDR_CHARGE                        0x0002		//      01
#define ADDR_MAC    					   0x0003		// 		12
#define ADDR_CERTIFICATE_LEN               0x000f       //      04
#define ADDR_PRIVATE_LEN                   0x0013       //      04
#define ADDR_CA_LEN                        0x0017       //      04
#define ADDR_REMAINING_TIME                0x001b       //      02
#define ADDR_OUTAGE_TIME                   0x001d       //      02

/**********************************************************************************/
#define PAGE_QUANTITY                       256     //Ã¿Ò³×Ö½ÚÊý

#define EEPROM_BASE_ADDRESS                 0x001000

/*********************************************************************************/

#define TERMINAL_SOFTWARE_VER          {0x12,0x09,0x12,0x01}
#define TERMINAL_HARDWARE_VER          {0x12,0x09,0x12,0x01}

/******************************
0x017800 -  0x017FFF    certificate file 
0x017000 -  0x0177FF    private file
0x016800 -  0x016FFF    CA file
*****************************/

typedef struct
{
    u32 certificate_file_len;
    u32 private_file_len;
    u32 CA_file_len;
}SSL_Certificate_TypeDef;

extern SSL_Certificate_TypeDef ssl_file_len;

void Write_Data(u8* pBuffer,u32 WriteAddr,u32 n);
void Read_Data(u8* pBuffer,u32 ReadAddr,u32 n);
u8 Read_BufferFlash(u32 DataAddress, u8 *DataBuffer, u16 DataCount);

__ramfunc void FLASH_Erase_Block(u32 Address);
u8 WriteBufferFlash(u32 DataAddress, u8 *DataBuffer, u16 DataCount);

bool  Erase_Page(const u8 pg_num);
void Bsp_MEM_Init(void);
void Read_file_len(void);
#endif













