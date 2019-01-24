/******************************************************************
** 版权:       杭州利拍档科技有限公司
** 文件名:     main.h
** 版本：
** 工作环境:   IAR 1.42
** 作者:       li
** 生成日期:   2017.8.18
** 功能:       文件包含
** 相关文件:
** 修改日志:
*******************************************************************/

#ifndef  __MAIN_H
#define  __MAIN_H


//#include "global.h"

#include "stm8l15x.h"
#include "config.h"

#include "1 task_safeguard.h"
#include "2 task_control.h"
#include "3 task_mem_read.h"
#include "4 task_mem_write.h"
#include "5 task_measure.h"
#include "6 task_communicate.h"
#include "7 task_net_connect.h"
#include "8 task_interaction.h"
#include "task_common.h"

//#include "app_sys_scan.h"
//#include "app_nb_c1100.h"
//#include "app_gps_c1100.h"
//#include "app_comm_c1100.h"
//#include "app_get_package.h"
//#include "app_measure.h"
//#include "app_other.h"
#include "app_clock.h"
//#include "app_control.h"
#include "app_comm_g510.h"
#include "app_comm_pc.h"
#include "app_common.h"

#include "bsp_adc.h"
#include "bsp_gpio.h"
#include "bsp_gprs.h"
#include "bsp_iwdg.h"
//#include "bsp_spi.h"
#include "bsp_tim.h"
#include "bsp_uart.h"
#include "bsp_rcc.h"
//#include "bsp_rtc.h"
//#include "bsp_lsm3ds6.h"
//#include "bsp_lsi3dh.h"
#include "bsp_mem.h"
#include "bsp_task.h"
#include "bsp_led.h"
#include "bsp_var.h"
//#include "bsp_key.h"
#include "bsp_ymodem.h"
#include "bsp_common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

#endif