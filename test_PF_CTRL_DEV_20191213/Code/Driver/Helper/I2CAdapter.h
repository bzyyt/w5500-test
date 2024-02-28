#ifndef __I2C_ADAPTER_H
#define __I2C_ADAPTER_H

#include "stm32f4xx.h"

typedef enum EI2cType
{
    I2C_1 = 0, //接加速度计
    
}I2cType;

#define I2C_BMP388 I2C_1

extern void I2c_Init(I2cType i2cType);
extern u8 I2c_CS(I2cType i2cType, u8 val);
extern u8 I2C_ReadByte(I2cType i2cType,u8 sAddress, u8 regAddr, u8 *pData);
extern u8 I2c_WriteByte(I2cType i2cType, u8 sAddress, u8 regAddr, u8 data);
extern u8 I2c_WriteBytes(I2cType i2cType, u8 sAddress, u8 addr, u8 *pData, u8 len);
extern u8 I2c_ReadBytes(I2cType i2cType, u8 sAddress, u8 startRegAddr, u8 *pData, u8 len);



#endif

