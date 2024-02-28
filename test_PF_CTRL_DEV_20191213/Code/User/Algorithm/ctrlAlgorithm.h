#ifndef __CTRL_ALGORITHM_H
#define __CTRL_ALGORITHM_H

#include "stm32f4xx.h"
#define PI 3.1415926535897932384626433832795



extern void CA_Init(void);
extern void CA_Proc(double u_ctrl[], double angleInc[], double attitude[], u32 count);


#endif

