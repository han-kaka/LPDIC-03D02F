#include "bsp_mem.h"

SSL_Certificate_TypeDef ssl_file_len;

void Unlock_DATA(void)
{
    do
    {
        FLASH->DUKR = 0xAE;                          
        FLASH->DUKR = 0x56;
    }while((FLASH->IAPSR & 0x08)==0);
}

void Lock_DATA(void)
{
    FLASH->IAPSR &= (~0x08);
}

void Write_Data(u8* pBuffer,u32 WriteAddr,u32 n)
{
	Unlock_DATA();
	for(;n!=0;n--)
	{
		*(volatile u8*)(EEPROM_BASE_ADDRESS+WriteAddr)=*pBuffer;
		WriteAddr++;
		pBuffer++;
	}
	Lock_DATA();
}

void Read_Data(u8* pBuffer,u32 ReadAddr,u32 n)
{
	Unlock_DATA();
	for(;n!=0;n--)
	{
		*pBuffer++=*(volatile u8*)(EEPROM_BASE_ADDRESS+ReadAddr);
		ReadAddr++;
	}
	Lock_DATA();
}

bool  Erase_Page(const u8 pg_num)
{
    u8 temp;
    u16 i=0;
    u32 WriteAddr = (PAGE_QUANTITY * pg_num);
    for (i=0;i<PAGE_QUANTITY;i++)
    {
        *(volatile u8*)(EEPROM_BASE_ADDRESS+WriteAddr) = 0xFF;
        WriteAddr++;
    }
    Tim3_Delay_Ms(5);
    for(i=0;i<PAGE_QUANTITY;i++)
    {
        temp=*(volatile u8*)(EEPROM_BASE_ADDRESS+WriteAddr);
        WriteAddr++;
        if(temp != 0xFF)
            return FALSE;
    }
    return TRUE;
}

void unlock_PROG(void)
{
    do{
        FLASH->PUKR = 0x56;
        FLASH->PUKR = 0xAE;
    }while((FLASH->IAPSR & 0x02)==0);
}

void lock_PROG(void)
{
    FLASH->IAPSR &= (~0x02);
}

__ramfunc void FLASH_Erase_Block(u32 Address)
{
    u8 __far *pwFlash = (__far u8 *)Address;
    FLASH->CR2 |= 0x20;   
    for(u16 i=0;i<128;i+=4)
    {        
             /* Enable erase block mode */       
             //*((__far u32*)Address) = (u32)0;
             *(pwFlash+i) = 0;
             *(pwFlash+1+i) = 0;
             *(pwFlash+2+i) = 0;
             *(pwFlash+3+i) = 0;
    }           
}

u8 WriteBufferFlash(u32 DataAddress, u8 *DataBuffer, u16 DataCount)
{
    u32 Address = (u32)DataAddress;
    u8 *DataPointer = DataBuffer;
    unlock_PROG();
//    unlock_DATA();
    while(DataCount >= 4)
    {
      FLASH->CR2 |= (uint8_t)0x40;
      *((__far u8*) Address) = *DataPointer  ; 	                    /* Write one byte - from lowest address*/
      *((__far u8*)(Address + 1)) = *(DataPointer + 1);                 /* Write one byte*/
      *((__far u8*)(Address + 2)) = *(DataPointer + 2);                 /* Write one byte*/
      *((__far u8*)(Address + 3)) = *(DataPointer + 3);                 /* Write one byte - from higher address*/
      
      while((FLASH->IAPSR & (0x04 | 0x01)) == 0);
      Address    += 4;
      DataPointer+= 4;
      DataCount  -= 4;
    }
    lock_PROG();
//    lock_DATA();
    return 1;
}

u8 Read_BufferFlash(u32 DataAddress, u8 *DataBuffer, u16 DataCount)
{
    for(u16 i=0;i<DataCount;i++)
    {
        DataBuffer[i] = FLASH_ReadByte(DataAddress+i);
    }
    return TRUE;
}

void Bsp_MEM_Init(void)
{
	u8 data = 0xFF;
	Read_Data(&data,ADDR_VERSION,1);
    
    //u8 data1[4] = {0x00};
    
    //Write_Data((u8*)&data1,ADDR_CERTIFICATE_LEN,4);  
    //Write_Data((u8*)&data1,ADDR_PRIVATE_LEN,4);
    //Write_Data((u8*)&data1,ADDR_CA_LEN,4);
    
	if(data != SOFT_VERSION)
	{
		data = SOFT_VERSION;
		Write_Data(&data, ADDR_VERSION, 1);

		data = 0;
		Write_Data(&data, ADDR_CHARGE, 1);
	}
	else
	{
        Read_Data((u8*)SYS_Manuf.MAC, ADDR_MAC, 12);
        Read_Data(&data, ADDR_CHARGE, 1);
        Read_Data((u8*)(&remaining_time_count), ADDR_REMAINING_TIME, 2);
        Read_Data((u8*)(&outage_time_count), ADDR_OUTAGE_TIME, 2);
        if(data == 0x01)
        {
            System_Flag.flag_need_charge = 1;
        }
        else
        {
            System_Flag.flag_need_charge = 0;
        }
	}
    
    Read_file_len();
}

void Read_file_len(void)
{
    /***  读取各证书文件大小  ***/
    Read_Data((u8*)&ssl_file_len.certificate_file_len,ADDR_CERTIFICATE_LEN,4);  
    Read_Data((u8*)&ssl_file_len.private_file_len,ADDR_PRIVATE_LEN,4);
    Read_Data((u8*)&ssl_file_len.CA_file_len,ADDR_CA_LEN,4);
        
    if((ssl_file_len.certificate_file_len != 0) &&
       (ssl_file_len.private_file_len != 0)&&
       (ssl_file_len.CA_file_len != 0))
    {
        System_Flag.flag_ymode_file = 1;
        //BSP_LOG("/** have file！**/\n\n");
        mode_status = MODE_NO_CHARGE;
    }
    else
    {
        System_Flag.flag_ymode_file = 0;
        //BSP_LOG("/** no file！**/\n\n");
        mode_status = MODE_NULL;
    } 
    
    
}
