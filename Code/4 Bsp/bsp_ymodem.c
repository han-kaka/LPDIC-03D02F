#include "bsp_ymodem.h"
#include "crc16.h"
#include <stdbool.h>

Ymodem_Info_TypeDef ymodem_info =
{
    .file_length    = 0,
    .total_package  = 0,
    .package_num    = 0,
    .fname_length   = 0,
};

bool bsp_ymodem_start(u8* p_src)
{
    if(p_src[0] == 0xAB && p_src[1] == 0x01)
    {
        // 发送 "C"
        Uart1_SendData(0x43);
        return true;
    }
    else
        return false;
}

static void ACK_Response(void)
{
    Uart1_SendData(ACK);
}

static void NAK_Response(void)
{
    Uart1_SendData(NAK);
}

static void CAN_Response(void)
{
    Uart1_SendData(CAN);
}

static void EOT_Response(void)
{
    Uart1_SendData(EOT);
}

static bool package_head_check(u8* p_src)
{
    if((p_src[0] + p_src[1] == 0xFF)&&\
        (p_src[0] == ymodem_info.package_num))    //包序号校验
    {
        ymodem_info.package_num++;
        return true;
    }
    else
    {
        return false;
    }
}

static bool data_crc16_check(u8* p_src,u16 data_length,u16 crc_value)
{
    u16 crc_temp = FitCRC_Calc16(p_src,data_length);
    if(FitCRC_Calc16(p_src,data_length) == crc_value)
    {
        return true;
    }
    else
    {
        return false;
    }
}
u32 File_Address;
static bool get_file_name(u8* p_src,u8 len)
{
    u8* fname;
    fname = (u8*)malloc(len);
    memcpy(fname,p_src,len);
    if(strstr((char*)fname, "certificate"))      //certificate file
    {       
        Write_Data((u8*)&ymodem_info.file_length,ADDR_CERTIFICATE_LEN,4);//文件长度记录在eeprom中,AT指令中会有用到
        File_Address = CERTIFICATE_FILE_ADDR;    //certificate file 开始地址
    }
    else if(strstr((char*)fname, "private"))    //private file
    {
        Write_Data((u8*)&ymodem_info.file_length,ADDR_PRIVATE_LEN,4);//文件长度记录在eeprom中,AT指令中会有用到
        File_Address = PRIVATE_FILE_ADDR;        //private file 开始地址
    }
    else if(strstr((char*)fname, "CA"))         //CA file
    {
        Write_Data((u8*)&ymodem_info.file_length,ADDR_CA_LEN,4);//文件长度记录在eeprom中,AT指令中会有用到
        File_Address = CA_FILE_ADDR;             //CA file 开始地址
    }
    else
    {
        #if DEBUG_ERROR
            BSP_LOG("\n file name error [function:get_file_name()]");
        #endif
        free(fname);
        return false;
    }
    free(fname);
    return true;
}

static bool SSL_Certificate_Storage(u8* p_src,u16 crc)
{
    u8* f_data;
    f_data = (u8*)malloc(256);

    FLASH_Erase_Block(File_Address);
    FLASH_Erase_Block(File_Address+128);
    
    WriteBufferFlash(File_Address,p_src,256);

    Read_BufferFlash(File_Address,f_data,256);

    if(FitCRC_Calc16(f_data,256) == crc)
    {
        File_Address = File_Address+256;
        free(f_data);
        return true;
    }
    else
    {
        free(f_data);
        return false;
    }
}

static bool file_info_decode(u8* p_src)
{
    u16  crc_value = p_src[131] << 8 | p_src[132];
    ymodem_info.fname_length = p_src[5] << 8 |\
                               p_src[6];
    ymodem_info.file_length  = p_src[9 + ymodem_info.fname_length] << 8 |\
                               p_src[10 + ymodem_info.fname_length];
    
    if(true != get_file_name(&p_src[7],ymodem_info.fname_length))
    {
        CAN_Response();
    }
    
    if(ymodem_info.file_length%256 == 0)
    {
        ymodem_info.total_package = ymodem_info.file_length/256;
    }
    else
    {
        ymodem_info.total_package = ymodem_info.file_length/256 + 1;
    }
    return data_crc16_check(&p_src[3],128,crc_value);
}

u8 bsp_ymodem_continue(u8* p_src)
{
    //第一帧 SOH帧
    if(p_src[0] == SOH && ymodem_info.package_num == 0)
    {
        //包头校验
        if(true != package_head_check(&p_src[1]))
        {
            //NAK应答
            NAK_Response();
            return YMODEM_TRANS;
        }
        //文件信息解析
        if(true != file_info_decode(p_src))
        {
            //NAK应答
            NAK_Response();
            return YMODEM_TRANS;
        }
        //ACK 应答
        ACK_Response();
        Uart1_SendData(0x43);
        return YMODEM_TRANS;
    }
    else
    {
        //
    }

    //STX帧
    if(p_src[0] == STX && ymodem_info.package_num <= ymodem_info.total_package)
    {
        //包头校验
        if(true != package_head_check(&p_src[1]))
        {
            //NAK应答
            NAK_Response();
            return YMODEM_TRANS;
        }
        //CRC校验
        u16 crc_value = p_src[259] << 8 | p_src[260];
        if(true != data_crc16_check(&p_src[3],256,crc_value))
        {
            //NAK应答
            NAK_Response();
            return YMODEM_TRANS;
        }
        //文件数据写入
        if(true != SSL_Certificate_Storage(&p_src[3],crc_value))
        {
            NAK_Response();
            return YMODEM_TRANS;
        }
        
        ACK_Response();
        if(ymodem_info.package_num == ymodem_info.total_package+1)
        {
            EOT_Response();
            return YMODEM_END;
        }
        return YMODEM_TRANS;
    }
    else
    {
        NAK_Response();
    }
    return YMODEM_TRANS;
}

void bsp_ymodem_finish(u8* p_src)
{   
    //最后一帧 SOH帧
    if(p_src[0] == SOH )//&& ymodem_info.package_num == ymodem_info.total_package + 1)
    {
        //空帧校验
        //应答
        ACK_Response();
        ymodem_info.package_num = 0;
        ymodem_info.total_package  = 0;
    }
}

static u8 ymodem_status = YMODEM_START;
void bsp_ymodem_trans(u8* p_src)
{
    switch(ymodem_status)
    {
        case YMODEM_START:
        {    
            #if DEBUG_YMODEM
                BSP_LOG("\n ymodem start \n");
            #endif
            if(true == bsp_ymodem_start(p_src))
            {
                ymodem_status = YMODEM_TRANS;       
            }
        }
            break;
        case YMODEM_TRANS:
        {
            #if DEBUG_YMODEM
                BSP_LOG("\n ymodem trans \n");
            #endif
            if(bsp_ymodem_continue(p_src) == YMODEM_END)
            {
                ymodem_status = YMODEM_END;
            }
        }
            break;
        case YMODEM_END:
        {
            #if DEBUG_YMODEM
                BSP_LOG("\n ymodem end \n");
            #endif
            bsp_ymodem_finish(p_src);
            ymodem_status = YMODEM_START;
            Read_file_len();
            if(System_Flag.flag_ymode_file == 1)
            {
                g_stGPRS_Handler.State = GPRS_STATE_POWER_ON; 
                g_stGPRS_Handler.StateOld = GPRS_STATE_POWER_ON;
                g_stGPRS_Handler.StepPt = 0;
                GPRS_Comm_Event_Set(COMM_Event_CONN);/*上电需发送一个登录包*/
                Set_Task(COMM, COMM_STATE_PROC);     /*启动GPRS状态处理任务*/
                if(System_Flag.flag_need_charge == 1)
                {
                    mode_status = MODE_CHARGEING;
                }
                else
                {
                    mode_status = MODE_NO_CHARGE;
                }
            }
        }
            break;
        default:
            ymodem_status = YMODEM_START;
            break;
    }
}
