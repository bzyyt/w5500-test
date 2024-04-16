

#ifndef __MAIN_H
#define __MAIN_H

#include "string.h"
#include "DelayAdapter.h"
#include "TimAdapter.h"
#include "UartAdapter.h"
#include "LedAdapter.h"
#include "FMCAdapter.h"
#include "CanAdapter.h"
#include "DACAdapter.h"
#include "ExtiAdapter.h"
#include "ADCAdapter.h"
// #include "UdpAdapter.h"
#include "GpioAdapter.h"
#include "SPIAdapter.h"
#include "SwoAdapter.h"

#include "ctrlAlgorithm.h"

#include "tcp_server.h"
#include "tcp_client.h"
#include "w5500_conf.h"

#define MAIN_FREQUENCY 1000

//
u32 g_counter_main = 0x00;

void Peripheral_Init(void);
//main interrupt function
void ISR_Main_Int(void);
void ISR_uart(void);
void RecvINSMsg(void);

#endif

