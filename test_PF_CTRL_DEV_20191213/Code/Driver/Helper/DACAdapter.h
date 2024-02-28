
#ifndef __DAC_ADAPTER_H
#define __DAC_ADAPTER_H

#include "stm32f4xx.h"

typedef enum EDACTYPE
{
    DAC_1,
    DAC_2
    
}EDacType;

extern void Dac_Init(void);
extern void Dac_Output(EDacType type, u16 data);


#endif

