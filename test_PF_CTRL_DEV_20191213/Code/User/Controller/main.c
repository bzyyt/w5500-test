
#include"main.h"

typedef struct stINSMSG
{
    double angleInc[2]; // rad 
    double attitude[2]; // rad
    double angleIncDeg[2]; // deg
    double attitudeDeg[2]; // deg
    unsigned int cnt_clock; // 1000Hz cnt
    unsigned char work_mode; // nav mode
}TInsMsg;

u8 g_ucRecvBuff[100] = {0x00};
u32 g_uiRecvCnt = 0;
u8 g_ucRecvBuffUart1[100] = {0x00};
u32 g_ucRecvCntUart1 = 0;
u8 g_ucFlagRecvFrame = 0;

u8 g_ucSendBuff[10] = {0x00};

TInsMsg g_stInsMsg;
double g_dStableLoopCtrl[2]; // 
double g_dCorrectLoopCtrl[2]; // 

u8 g_ucStatePower[2];

int main(void)
{
    //Disable the global interrupt
    __set_PRIMASK(1);  
    //Set interrupt group
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    //Initialize the delay func
    Delay_Init();  
    // Initialize main timer(1000Hz)
    Timer_Init(ETimerType_Timer2, 10000/MAIN_FREQUENCY, ISR_Main_Int);
    // Initialize peripheral
    Peripheral_Init();
    // Initialize control algorithm 
    CA_Init();
    //Enable the global interrupt
    __set_PRIMASK(0); 
    
    while(1)
    {
        // printf("Hello World!\n");
    }
}


void ISR_Main_Int()
{   
    g_counter_main ++;

    // do_tcp_server();
    do_tcp_client();
    
   // ISR_uart(); // recv INS anginc & attitude 
    
 //   CA_Proc(g_dStableLoopCtrl, g_stInsMsg.angleInc, g_stInsMsg.attitude, g_counter_main);
    
    if(g_counter_main % (MAIN_FREQUENCY/2) == 1)
    {
        Led_Blink(LED_1);
    }    
}


void Peripheral_Init(void)
{
    //Initialize LED
    Led_Init();
    // Initialize can
    Can_Init();
    // Initialize DAC
    Dac_Init();
    // Initialize ADC
    Adc_Init();
    // Initialize UDP
    // Udp_Init();
    // Initialize GPIO
    Gpio_Init();
    //Initialize FMC(FPGA)
    FMC_Init();
    // SPI3初始化 
    Spi_Init(SPI_3);                 
    // Uart(FGPA)
    Uart_Init(FPGAUart1, 460800, DataBits_8, StopBits_1, ParityBits_None, 0, 0x01, 0x01);
    Uart_Init(FPGAUart2, 460800, DataBits_8, StopBits_1, ParityBits_None, 0, 0x01, 0x01);
    Uart_Init(FPGAUart3, 460800, DataBits_8, StopBits_1, ParityBits_None, 0, 0x01, 0x01);
    //w5500初始化
    w5500Init();
}

void RecvINSMsg(void)
{
    int iTemp;
    memcpy(&iTemp, g_ucRecvBuffUart1+2, 4);
    g_stInsMsg.angleIncDeg[0] = ((double)iTemp)*600.0/2147483648.0*0.001; // x axis ang inc, unit:deg / 1ms
    g_stInsMsg.angleInc[0] = g_stInsMsg.angleIncDeg[0]*PI/180.0; // x axis ang inc, unit:rad / 1ms
    memcpy(&iTemp, g_ucRecvBuffUart1+6, 4);
    g_stInsMsg.angleIncDeg[1] = ((double)iTemp)*600.0/2147483648.0*0.001; // x axis ang inc, unit:deg / 1ms
    g_stInsMsg.angleInc[1] = g_stInsMsg.angleIncDeg[1]*PI/180.0; // x axis ang inc, unit:rad / 1ms
    memcpy(&iTemp, g_ucRecvBuffUart1+10, 4);
    g_stInsMsg.attitudeDeg[1] = ((double)iTemp)*180.0/2147483648.0; // x axis attitude, unit:deg
    g_stInsMsg.attitude[1] = g_stInsMsg.attitudeDeg[1]*PI/180.0; // x axis attitude, unit:rad
    memcpy(&iTemp, g_ucRecvBuffUart1+14, 4);
    g_stInsMsg.attitudeDeg[0] = ((double)iTemp)*180.0/2147483648.0; // x axis attitude, unit:deg
    g_stInsMsg.attitude[0] = g_stInsMsg.attitudeDeg[0]*PI/180.0; // x axis attitude, unit:rad
    memcpy(&g_stInsMsg.cnt_clock, g_ucRecvBuffUart1+18, 4);
    g_stInsMsg.work_mode = g_ucRecvBuffUart1[22];
}


void ISR_uart(void)
{
    u32 i,len;
    u32 j;
    u8 ucCheckSum;
    // send
    g_ucSendBuff[0] = 0x66;
    g_ucSendBuff[1] = 0x99;
    g_ucSendBuff[2] = g_ucStatePower[0];
    g_ucSendBuff[3] = g_ucStatePower[1];
    SendUartMsg(FPGAUart1, 0, 4, g_ucSendBuff);
    
    // recv
    len = RecvUartMsg(FPGAUart1, g_ucRecvBuff);
    if(len != 0)
    {
        for (i=0; i<len; i++)
        {
            g_ucRecvBuffUart1[g_ucRecvCntUart1] = g_ucRecvBuff[i];
            switch(g_ucRecvCntUart1)
            {
                case 0: // frame head1
                    if (0x7E == g_ucRecvBuffUart1[g_ucRecvCntUart1])
                        g_ucRecvCntUart1++;
                    else
                        g_ucRecvCntUart1 = 0;
                    break;
                case 1: // frame head2
                    if (0x7E == g_ucRecvBuffUart1[g_ucRecvCntUart1])
                        g_ucRecvCntUart1++;
                    else
                        g_ucRecvCntUart1 = 0;
                    break;
                default:
                    g_ucRecvCntUart1++;
                    if (g_ucRecvCntUart1 == 24)
                    {
                        ucCheckSum = 0;
                        for (j=2; j<23; j++)
                            ucCheckSum ^= g_ucRecvBuffUart1[j];
                        if (g_ucRecvBuffUart1[j] == ucCheckSum)
                        {
                            RecvINSMsg();
                            g_ucFlagRecvFrame = 1;
                            
                            //SendUartMsg(FPGAUart2, 0, g_ucRecvCntUart1, g_ucRecvBuffUart1);
                        }
                        g_ucRecvCntUart1 = 0;
                    }
                    break;
            }
            
        
        }
        //SendUartMsg(FPGAUart2, 0, len, g_ucRecvBuff);
    }
    
}


