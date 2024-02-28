
#include "DACAdapter.h"
#include "SPIAdapter.h"
#include "DelayAdapter.h"

void Dac_Init()
{
    Spi_Init(SPI_DA1);
    Spi_Init(SPI_DA2);
}

void Dac_Output(EDacType type, u16 data)
{
    if(DAC_1 == type)
    {
        Spi_CS(SPI_DA1, 0);
        Spi_WriteData(SPI_DA1, data);
        Delay_us(3);
        Spi_CS(SPI_DA1, 1);     
    }
    else if(DAC_2 == type)
    {
        Spi_CS(SPI_DA2, 0);
        Spi_WriteData(SPI_DA2, data);
        Delay_us(3);
        Spi_CS(SPI_DA2, 1);          
    }        
}

