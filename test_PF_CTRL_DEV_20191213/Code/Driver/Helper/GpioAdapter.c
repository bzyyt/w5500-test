

#include "GpioAdapter.h"

void Gpio_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure; 
    
    /* GPIOC Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    /* Configure PC2 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* GPIOH Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);

    /* Configure PC2 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOH, &GPIO_InitStructure);
    
}


void Power_Ctrl(EPowerCtrlType type, EPOwerCtrlState onoff)
{
    if(POWER_INNER_EN == type)
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_0, (BitAction)onoff);
    }
    else if(POWER_OUTER_EN == type)
    {
        GPIO_WriteBit(GPIOC, GPIO_Pin_1, (BitAction)onoff);
    }
}


EWarningState Get_Warning_State(EWarningType type)
{
    if(WARNING_INNER == type)
    {
        return (EWarningState)GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_3);
    }
    else if(WARNING_OUTER == type)
    {
        return (EWarningState)GPIO_ReadInputDataBit(GPIOH, GPIO_Pin_4);
    }
    else
    {
        return WARNING_NONE;
    }
}

