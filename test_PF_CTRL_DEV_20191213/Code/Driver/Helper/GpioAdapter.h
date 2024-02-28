
#ifndef __GPIO_ADAPTER_H
#define __GPIO_ADAPTER_H

#include"stm32f4xx.h"

typedef enum EPOWERCTRLTYPE
{
    POWER_INNER_EN = 0,
    POWER_OUTER_EN,
    
}EPowerCtrlType;

typedef enum EPOWERCTRLSTATE
{
    POWER_OFF = 0,
	POWER_ON,
	
}EPOwerCtrlState;

typedef enum EWARNINGTYPE
{
    WARNING_INNER = 0,
    WARNING_OUTER,
    
}EWarningType;

typedef enum EWARNINGSTATE
{
    WARNING_FAULT = 0,
	WARNING_NORMAL,
    WARNING_NONE,
	
}EWarningState;

extern void Gpio_Init(void);
extern void Power_Ctrl(EPowerCtrlType type, EPOwerCtrlState onoff);
extern EWarningState Get_Warning_State(EWarningType type);

#endif

