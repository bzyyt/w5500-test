
#ifndef __ADC_ADAPTER_H
#define __ADC_ADAPTER_H

#include "stm32f4xx.h"

typedef enum EADCTYPE
{
    ADC_1,
    ADC_2
    
}EAdcType;

extern void Adc_Init(void);
extern float Adc_Input(EAdcType type);


#endif

