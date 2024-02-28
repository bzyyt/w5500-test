
#ifndef __FMC_ADAPTER_H
#define __FMC_ADAPTER_H

#include "stm32f4xx.h"

extern void FMC_Init(void);
extern u16 FMC_ReadWord(u32 addr);
extern void FMC_WriteWord(u32 addr, u16 data);
extern void FMC_ReadBuffer(u32 addr, u16 *pBuffer, u32 size);
extern void FMC_WriteBuffer(u32 addr, u16 *pBuffer, u32 size);

#endif

