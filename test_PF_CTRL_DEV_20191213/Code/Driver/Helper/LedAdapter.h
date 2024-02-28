
#ifndef __LED_ADAPTER_H
#define __LED_ADAPTER_H

#include "stm32f4xx.h"

typedef enum ELEDTYPE
{
    LED_1 = 0,
    LED_2,
    
}ELedType;

typedef enum ELEDSTATE
{
    LED_OFF = 0,
	LED_ON,
	
}LedState;

extern void Led_Init(void);
extern void Led_Ctrl(ELedType type, LedState onoff);
extern void Led_Blink(ELedType type);


#endif

