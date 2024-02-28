
#include "I2CAdapter.h"
#include "DelayAdapter.h"

// 通过复位来手动复位BUSY位,表示IIC总线不忙
void IIC1_BUSY_RESET(void)
{
    I2C1->CR1 |= 0x1<<15;  // SWRST 位置1
}

// 取消复位,BUSY位自动置1,然后初始化并且使能IIC外设
void IIC1_BUSY_SET(void)
{
    I2C_InitTypeDef  I2C_InitStruct;

    I2C1->CR1 &= ~(0x1<<15);   // SWRST 位清0;

    // 初始化IIC1
    /* I2C Struct Initialize */
    I2C_InitStruct.I2C_ClockSpeed = 100000;             // 100 kHz
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x00;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    /* I2C Initialize */
    I2C_Init(I2C1, &I2C_InitStruct);
    /* 使能 IIC1 模块 */
    I2C_Cmd(I2C1,ENABLE);   
}

void I2C1_Config()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    
    // Enable I2C clock 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    // Enable GPIO clock
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

    //PB8: I2C1_SCL  PB9: I2C1_SDA
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
       
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
    Delay_us(10);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

    /* I2C Struct Initialize */       
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitStructure); 
    
    /* I2C Enable */
    I2C_Cmd(I2C1, ENABLE);

}




void I2c_Init(I2cType i2cType)
{
    switch(i2cType)
    {
        case I2C_1:
            I2C1_Config();
            break;
    }
}


u8 I2c_CS(I2cType i2cType, u8 val)
{
    u8 retVal = 0x00;
    switch(i2cType)
    {
        case I2C_1:
            break;
    }  
    return retVal;    
}

u8 I2c_WriteByte(I2cType i2cType, u8 sAddress, u8 regAddr, u8 data)
{
    return I2c_WriteBytes(i2cType, sAddress, regAddr, &data, 1);
}


u8 I2c_WriteBytes(I2cType i2cType, u8 sAddress, u8 addr, u8 *pData, u8 len)
{
    const u32 TIMEOUT_MAX = 0x1000;
    u8 retVal = 0x00;
    int i = 0;
    u32 count_timeout = 0;
    
    if(i2cType == I2C_1)
    {   
        /* Enable the ACK */
        I2C_AcknowledgeConfig(I2C1, ENABLE);
        /* Check the bus state */
        count_timeout = TIMEOUT_MAX;
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)&&(count_timeout--))
        {
            if((count_timeout--)==0) return 0x01;
        }
        /* Generate the start stage */
        I2C_GenerateSTART(I2C1, ENABLE);
        /* check EV5, use for start operation*/
        count_timeout = TIMEOUT_MAX;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if((count_timeout--)==0) return 0x02;
        }
        /* Send slave address for write */
        I2C_Send7bitAddress(I2C1, sAddress<<1, I2C_Direction_Transmitter);
        /* check EV6, use for send slave address */
        count_timeout = TIMEOUT_MAX;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if((count_timeout--)==0) return 0x03;
        }
        
        if(len == 1)
        {
             /* Send the register address */
            I2C_SendData(I2C1,addr);
            /* check EV8, use for send data */
            count_timeout = TIMEOUT_MAX;
            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            {
                if((count_timeout--)==0) return 0x04;
            }       
            /* Send the register data */
            I2C_SendData(I2C1,pData[0]);
            /* check EV8, use for send data */
            count_timeout = TIMEOUT_MAX;
            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
            {
                if((count_timeout--)==0) return 0x05;
            }             
        }
        else if((len%2) == 0)
        {
            for(i=0; i<len/2; i++)
            {
                  /* Send the register address */
                I2C_SendData(I2C1,pData[i*2+1]);
                /* check EV8, use for send data */
                count_timeout = TIMEOUT_MAX;
                while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
                {
                    if((count_timeout--)==0) return 0x04;
                }       
                /* Send the register data */
                I2C_SendData(I2C1,pData[i*2]);
                /* check EV8, use for send data */
                count_timeout = TIMEOUT_MAX;
                while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
                {
                    if((count_timeout--)==0) return 0x05;
                }                
            }           
        }
        else
        {
            return 0x06;
        }
        

  
        /* Generate end state */
        I2C_GenerateSTOP(I2C1,ENABLE);        
        
    }
    return retVal;
}

u8 I2c_ReadBytes(I2cType i2cType, u8 sAddress, u8 startRegAddr, u8 *pData, u8 len)
{
    u8 retVal = 0x00;
    
    const u32 TIMEOUT_MAX = 0x1000;
    int i = 0;
    u32 count_timeout = 0;
    
    if(i2cType == I2C_1)
    {   
        /* Enable the ACK */
        // I2C_AcknowledgeConfig(I2C1, ENABLE);
        /* Check the bus state */
        count_timeout = TIMEOUT_MAX;
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
        {
            if((count_timeout--)==0) return 0x01;
        }
        /* Generate the start stage */
        I2C_GenerateSTART(I2C1, ENABLE);
        /* check EV5, use for start operation*/
        count_timeout = TIMEOUT_MAX;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if((count_timeout--)==0) return 0x02;
        }
        /* Send slave address for write */
        I2C_Send7bitAddress(I2C1, sAddress<<1, I2C_Direction_Transmitter);
        /* check EV6, use for send slave address */
        count_timeout = TIMEOUT_MAX;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        {
            if((count_timeout--)==0) return 0x03;
        }
        /* Send the register address */
        I2C_SendData(I2C1,startRegAddr);
        /* check EV8, use for send data */
        count_timeout = TIMEOUT_MAX;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if((count_timeout--)==0) return 0x04;
        }      


        /* Generate the start stage */
        I2C_GenerateSTART(I2C1, ENABLE);
        /* check EV5, use for start operation*/
        count_timeout = TIMEOUT_MAX;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
        {
            if((count_timeout--)==0) return 0x05;
        }
        /* Send slave address for read */
        I2C_Send7bitAddress(I2C1, sAddress<<1, I2C_Direction_Receiver);
        /* check EV6, use for send slave address */
        count_timeout = TIMEOUT_MAX;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
            if((count_timeout--)==0) return 0x06;
        }
        
        for(i=0; i<len; i++)
        {     
            if(i==len-1)
            {
                /* Disable the ack */
                I2C_AcknowledgeConfig(I2C1,DISABLE);
                /* Generate end state */
                I2C_GenerateSTOP(I2C1,ENABLE);      
            }
            /* Read the data */
            count_timeout = TIMEOUT_MAX;
            /* check EV7 */
            while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
            {
                if((count_timeout--)==0) return 0x07;
            }                   
            pData[i] = I2C_ReceiveData(I2C1);   
        }

        I2C_AcknowledgeConfig(I2C1, ENABLE);
        
    }
    
    return retVal;   
}

u8 I2C_ReadByte(I2cType i2cType,u8 sAddress, u8 regAddr, u8 *pData)
{
    return I2c_ReadBytes(i2cType, sAddress, regAddr, pData, 1);
}

