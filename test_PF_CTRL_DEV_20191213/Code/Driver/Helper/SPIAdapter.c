
#include "SPIAdapter.h"
#include "DelayAdapter.h"

void SPI1_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    
    //PA15--CS
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);  //开启时钟
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //引脚初始化
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_15);
    
    //PA5--SCLK PB5--MOSI
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);  //开启时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  //开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);  //时钟
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  //引脚初始化
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1);  
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI1);
    
    
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,ENABLE);
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1,DISABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx; 
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;  
    SPI_InitStructure.SPI_CPOL  = SPI_CPOL_Low; 
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // APB2 = 90MHz, 90/16 = 6.25M 
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1,&SPI_InitStructure);
    
    SPI_Cmd(SPI1,ENABLE); 
	
}

void SPI2_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    //PI0--CS
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOI,&GPIO_InitStructure);
    GPIO_SetBits(GPIOI, GPIO_Pin_0);  
    
    //PD3--SCLK PI3--MOSI 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI,ENABLE); 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);  
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOD,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOI,&GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource3,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOI,GPIO_PinSource3,GPIO_AF_SPI2);
    
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);
    
    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;   
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;   
    SPI_InitStructure.SPI_CPOL  = SPI_CPOL_High; 
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // APB1 = 50MHz, 25/8 = 6.25M 
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2,&SPI_InitStructure);
    SPI_Cmd(SPI2,ENABLE); 
}

void SPI3_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;
    
    //PA4--CS
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    
    //PB3--SCLK PB4--MISO PC12--MOSI 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);  
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOC,&GPIO_InitStructure);
    
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);  
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SPI3);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);    
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, DISABLE);
    
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;   
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  
    SPI_InitStructure.SPI_CPOL  = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; // APB1 = 45MHz, 45/2 = 22.5M 
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI3,&SPI_InitStructure);
    SPI_Cmd(SPI3,ENABLE); 
}


void Spi_Init(SpiType spiType)
{
    switch(spiType)
    {
        case SPI_1:
            SPI1_Config();            
            break;
        case SPI_2: 
            SPI2_Config();              
            break;            
        case SPI_3:
            SPI3_Config();            
            break;                
        default:
            break;
    }
}

void Spi_CS(SpiType spiType,u8 val)
{
    switch(spiType)
    {
        case SPI_1:
            GPIO_WriteBit(GPIOA, GPIO_Pin_15, (BitAction)val);
            break;
        case SPI_2:
            GPIO_WriteBit(GPIOI, GPIO_Pin_0, (BitAction)val);
            break;
        case SPI_3:
            GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)val);
            break;
        default:
            break;
    }
}

u16 Spi_ReadWrite(SPI_TypeDef * spi, u16 TxData)
{		    
    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(spi, TxData);

    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(spi);
}

void Spi_Write(SPI_TypeDef * spi, u16 TxData)
{		    
    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(spi, TxData);
}

void Spi_WriteData(SpiType spiType, u16 data)
{
    switch(spiType)
    {
        case SPI_1:
            Spi_Write(SPI1, data);
            break;
        case SPI_2:
            Spi_Write(SPI2, data);
            break;
        case SPI_3:
            Spi_Write(SPI3, data);
            break;
        default:
            break;
    } 
}

u16 Spi_ReadWriteData(SpiType spiType, u16 data)
{
    u8 retData = 0;
    switch(spiType)
    {
        case SPI_1:
            retData = Spi_ReadWrite(SPI1, data);
            break;
        case SPI_2:
            retData = Spi_ReadWrite(SPI2, data);
            break;
        case SPI_3:
            retData = Spi_ReadWrite(SPI3, data);
            break;
        default:
            break;
    } 
    
    return retData;
}

