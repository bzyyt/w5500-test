
#ifndef __UART_ADAPTER_H
#define __UART_ADAPTER_H

#include "stdbool.h"
#include "stm32f4xx.h"

typedef enum EDataBits
{
    DataBits_8 = 0,
    DataBits_9,
    
}DataBits;

typedef enum EStopBits
{
    StopBits_1 = 0,
    StopBits_0_half,
    StopBits_2,
    StopBits_1_half,
    
}StopBits;

typedef enum EParityBits
{
    ParityBits_None,
    ParityBits_Odd,
    ParityBits_Even,
    
}ParityBits;

typedef enum EUartType
{
    ARMUart1 = 0,
    ARMUart2,
    ARMUart3,
    FPGAUart1,
    FPGAUart2,
    FPGAUart3,
    Uart_Null,
    
}UartType;

#define MAX_RECV_LEN_BUFFER 320
#define MAX_SEND_LEN_BUFFER 320

typedef struct SUartData
{
   UartType uartType;
   u8 Send_Buffer[MAX_SEND_LEN_BUFFER];
   u8 Recv_Buffer[MAX_RECV_LEN_BUFFER];
   u8 Cache_Buffer[MAX_RECV_LEN_BUFFER];
   u8 Recv_Length;
   u8 Recv_Status;
   
   
}UartData;

#define UART_MAX_CHANNEL_NUMBER 10


extern UartData g_UartData[UART_MAX_CHANNEL_NUMBER];

typedef void (*Uart_Hander)(u8 *buf, u8 len);

extern u32 RecvUartMsg(u16 uartType, u8 *buf);
extern bool SendUartMsg(u16 uartType, u8 start, u8 len, u8 *buf);
extern bool Uart_Init(UartType uartType, u32 baudrate, DataBits dataBits, StopBits stopBits, ParityBits parityBits, Uart_Hander hander, u8 isTxEnable, u8 isRxEnable);


#endif

