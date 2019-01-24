#ifndef __TASK_INTERACTION_H
#define __TASK_INTERACTION_H

#include "global.h"

#define  MODE_NULL                   0
#define  MODE_NO_CHARGE              1
#define  MODE_CHARGEING              2
#define  MODE_NETWORKING             3
#define  MODE_OTA                    4

extern u8 mode_status;

u8 Interaction_Task(u8 prio);
void Battery_Status_Set(u8  data);

#endif