#include "app_common.h"

void Set_Task(u8 main_task,u8 sub_task)
{
  u16 mm,ss;
  mm = 1<<main_task;
  ss = 1<<sub_task;
  g_Maintask |= mm;
  ga_Subtask[main_task] |= ss;
}

void Clr_Task(u8 main_task,u8 sub_task)
{
    u16 ss;
    if(sub_task == TASK_ALL)
    {
        ga_Subtask[main_task] = 0;
    }
    else
    {        
        ss = 1<<sub_task;
        ga_Subtask[main_task] &= ~ss;
    }
}

u8 clear_task(u8 prio , u8 m_SYS_SubTask_prio)
{
   ga_Subtask[prio] &=~ (1<<m_SYS_SubTask_prio);
   if(ga_Subtask[prio] == 0)
   {
        g_Maintask &=~(1<<prio);
        return TRUE;
   }
   return FALSE;
}

/***********************************************************************
��������:         ���ݰ��ĺ�У�� ��֤
�������:         INT8U *data   ��У�����ݰ����׵�ַ 
                  INT8U length  ��У�����ݰ��ĳ���              		
�������:         ��
��������ֵ˵��:   TURE  ���ݰ���֤ͨ��
                  FALSE ���ݰ���֤ʧ��
ʹ�õ���Դ:    	  ��
***********************************************************************/

u8 Sum_Check(u8 *data,u8 length)
{
  u8 temp = 0;
  for(u8 i= 0; i < length-1; ++i)
  {
    temp = temp + *(data + i);
  }
  
  if( *(data+length-1) == temp )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/***********************************************************************
��������:         ������ݵ�У��ֵ 
�������:         INT8U *data   ��У�����ݵ��׵�ַ 
                  INT8U length  ��У�����ݵĳ���              		
�������:         ��
��������ֵ˵��:   У����
ʹ�õ���Դ:    	  ��
***********************************************************************/
u8 Sum(u8 *data,u8 length)
{
  u8 result=0;
  for(u8 i= 0; i < length; ++i)
  {
    result = result + *(data + i);
  }
  return result;
}
  
/***********************************************************************
��������:         BCD��ת8λ���� 
�������:         ��ת������  ������0x20,���=20=0x14��
                  		
�������:         ��
��������ֵ˵��:   ת�����
ʹ�õ���Դ:    	  ��
***********************************************************************/
u8 BCD_2_Data8(u8 data)
{
  u8 result;
  u8 high,low;

  high = data>>4;
  low = (data&0x0f);
  result = high*10 + low;
  return result;
}

/***********************************************************************
��������:         8λ����תBCD�� 
�������:         ��ת������ ��Χ(0~99) ����99��Ϊ��data%100
                  ����62�������0x62		
�������:         ��
��������ֵ˵��:   ת�����
ʹ�õ���Դ:    	  ��
***********************************************************************/

u8 Data8_2_BCD(u8 data)
{

  u8 return_data = 0;
  while(data >= 100)                                //�������Ϊ99 ȥ����λ��
  {
      data = data - 100;
  }
  while(data >= 10)
  {
      data = data - 10;
      return_data = return_data + 16;
  }  
  return_data = return_data + data;

  return return_data;
}

/***********************************************************************
��������:         16λ����תBCD�� 
�������:         ��ת������ ��Χ(0~9999) ����9999��Ϊ��data%100
                  ����162�������0x162		
�������:         ��
��������ֵ˵��:   ת�����
ʹ�õ���Դ:    	  ��
***********************************************************************/
u16 Data16_2_BCD(u16 data)
{

  u16 return_data = 0;
  while(data >= 10000)                                //�������Ϊ99 ȥ����λ��
  {
      data = data - 10000;
  }
  while(data >= 1000)
  {
      data = data - 1000;
      return_data = return_data + 16*16*16;
  }  
  while(data >= 100)
  {
      data = data - 100;
      return_data = return_data + 16*16;
  }  
  while(data >= 10)
  {
      data = data - 10;
      return_data = return_data + 16;
  }  
  return_data = return_data + data;

  return return_data;
}

BOOL Check_Strstr(S8 *data1,S8 *data2,uint16_t len)
{
    uint16_t i=0;
    S8 *p;
    p=data2;
	
    do
    {
        if(*data1==*p)
        { 
            p++;
            if(*p=='\0')
            {
                return TRUE;
            }
            
        }
        else
        { 
            p=data2;
        }
		
        data1++;
        if(*data1=='\0')
        {
            return FALSE;
        }
        i++;
    }while(i<len);
    
    return FALSE;
}

//����ǰ��0
U8 CntLeadZeros(U8 ucData)
{
	U8 ucIndex;
	ucIndex = 0;
	
	while(ucIndex < 8)
	{
		if(ucData & (0x01 << ucIndex))
		{
		    return ucIndex;
		}
		ucIndex++;
	}
	return (U8)0xFF;
}

u8 Get_Symbol(u8 num_symbol,u8 *buf, char symbol)
{
	 u8 i=0;
	 u8 j=0;
	 u8 len=0;
	 char str = symbol;
	 u8 location=0;
	 u8 *p_buf=buf;
	 len=strlen((char *)p_buf);
	 for(i=0;i<num_symbol;i++)
	 {
		 
		  while(p_buf[j]!=str)
			{
				 j++;
				 if(j>=len)
				 {
					  return FALSE;
				 }
			}
			j+=1;
			location=j;
	 }
	 return location;
}

u8* DecimalToASCII(u32 port,u8*data)
{
    u8 num,i;
    
    num = port/1000000000+0x30;
    data[0] = num;
    num = port%1000000000/100000000+0x30;
    data[1] = num;
    num = port%100000000/10000000+0x30; 
    data[2] = num;
    num = port%10000000/10000000+0x30;
    data[3] = num;
    num = port%1000000/1000000+0x30;
    data[4] = num;
    num = port%100000/10000+0x30;
    data[5] = num;
    num = port%10000/1000+0x30;
    data[6] = num;
    num = port%1000/100+0x30;
    data[7] = num;
    num = port%100/10+0x30;
    data[8] = num;
    num = port%10+0x30;
    data[9] = num;
    data[10] = 0;
    
    for(i=0;i<10;i++)
    {
        if(data[i] != 0x30) return (&data[i]);
        
    }
    return NULL;
}

U8 Get_Uint_Data(const S8 *buf)
{
	U8 i=0;
	U8 len=0;
	U8 int_data=0;
	U8 byte=0;
	S8 str=',';

	S8 data[20];
	len=strlen((S8 *)buf);
	while(buf[i]!=str)
	{
		data[i]=buf[i];
		i++;
		if(i>=len)
		{
			data[i]='\0';
			int_data=atoi(data);
			byte=(U8)int_data;
			return byte;
		}
	}
	data[i]='\0';
	int_data=atoi(data);
	byte=(U8)int_data;
	return byte;
}

u8* DecimalToASCII16(u16 port,u8*data)
{
    u8 num,i;
    num = port/10000+0x30;
    data[0] = num;
    num = port%10000/1000+0x30;
    data[1] = num;
    num= port%1000/100+0x30; 
    data[2] = num;
    num = port%100/10+0x30;
    data[3] = num;
    num = port%10+0x30;
    data[4] = num;
    data[5] = 0;
    for(i=0;i<5;i++)
    {
        if(data[i] != 0x30) return (&data[i]);
        
    }
    return NULL;
}
