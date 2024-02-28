
#ifndef __SPI_ADAPTER_H
#define __SPI_ADAPTER_H

#include "stm32f4xx.h"


typedef enum ESpiType
{
    SPI_1 = 0, //��DA1
    SPI_2 = 1, //��DA2  
    SPI_3 = 2, //��W5500
}SpiType;


#define SPI_DA1 SPI_1
#define SPI_DA2 SPI_2
#define SPI_W5500 SPI_3

extern void Spi_Init(SpiType spiType);
extern u16 Spi_ReadWriteData(SpiType spiType, u16 data);
extern void Spi_WriteData(SpiType spiType, u16 data);
extern void Spi_CS(SpiType spiType,u8 val);

#endif

