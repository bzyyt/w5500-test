
#include "ctrlAlgorithm.h"
#include "string.h"
#include "ADCAdapter.h"
#include "DACAdapter.h"
#include "GpioAdapter.h"

#define LEN_ATTI_SERIES 4u
#define TIME_INNER_POWERON 13*1000  // inner axis normal 13s after power on
#define TIME_OUTER_POWERON 15*1000  // outer axis normal 15s after power on
#define DEGREE_INNER_COEF_CHANGE 2.0*PI/180.0 // inner axis ctrl coef change after pitch change
#define DEGREE_OUTER_COEF_CHANGE 2.0*PI/180.0 // outer axis ctrl coef change after roll change

typedef enum TCATYPE
{
    CA_INNER, // inner axis
    CA_OUTER, // outer axis
}CAType;

typedef struct TPOWERCTRLPARA
{
    float threshold_current_amplitude[2];
    u32 threshold_current_over_count[2];
    u32 threshold_warning_count[2];
    
}TPowerCtrlPara;

typedef struct TPOWERCTRLCALC
{
    float current[2];
    u8 flag_current_over[2];    
    u32 count_current_over[2];
    
    EWarningState state_warning[2];
    u8 flag_warning[2];
    u32 count_warning[2];
    
    u8 state_power[2];
}TPowerCtrlCalc;


typedef struct TFRQCTRLPARA
{
    double t[2][4];
    double kc[2];
    double invT; // 1/T
    double invTpower; // 1/T^2
    double a[2][3]; // num 
    double b[2][3]; // den
    
}TFrqCtrlPara;

typedef struct TFRQCTRLCALC
{
    double angleInc[2][LEN_ATTI_SERIES]; // rad 
	
	double attiSeries[2][LEN_ATTI_SERIES]; // [0]-current, [1]-pre, [2]-pre_pre, [3]-pre_pre_pre, ...
	double attiInc[2][LEN_ATTI_SERIES];  // attitude error
	
	double u_ctrl[2][LEN_ATTI_SERIES]; // ctrl value
}TFrqCtrlCalc;

TPowerCtrlCalc g_CA_POWER_Calc;
TPowerCtrlPara g_CA_POWER_Para;

TFrqCtrlPara g_CA_FC_Para;
TFrqCtrlCalc g_CA_FC_Calc;

extern u8 g_ucStatePower[2];

/////////////////////////////////////////////////////////
//define
void CA_Power_Ctrl(u32 count);
void CA_Frequency_Ctrl(double u_ctrl[], double angleInc[], double attitude[]);

void Inner_Weak_Coef_Init(void);
void Outer_Weak_Coef_Init(void);
void Inner_Work_Coef_Init(void);
void Outer_Work_Coef_Init(void);
void CA_Init(void);

void CA_Proc(double u_ctrl[], double angleInc[], double attitude[], u32 count);
/////////////////////////////////////////////////////////

void CA_Power_Ctrl(u32 count)
{
    int i = 0;
    for(i = 0; i < 2; i++)
    {

    }    
    /*
    Power_Ctrl(POWER_INNER_EN, POWER_ON);
    Power_Ctrl(POWER_OUTER_EN, POWER_ON);
    */
}

// u_ctrl - ctrl value output 
// ang_inc, atti - ang inc & atti from INS
void CA_Frequency_Ctrl(double u_ctrl[], double angleInc[], double attitude[])
{
    int i = 0;
	int j;
    u16 temp =  0x00;
    double u[2] = {0.0};


    
}

void Inner_Weak_Coef_Init(void)
{
    // inner
;  
}

void Inner_Work_Coef_Init(void)
{
    // inner
 ;  
}    

void Outer_Weak_Coef_Init(void)
{
    // outer
; 
}
   
void Outer_Work_Coef_Init(void)
{
    // outer
;
}
    
void CA_Init(void)
{
    // power control initialize
    memset(&g_CA_POWER_Calc, 0, sizeof(g_CA_POWER_Calc));
    memset(&g_CA_POWER_Para, 0, sizeof(g_CA_POWER_Para));
      
    g_CA_POWER_Para.threshold_current_amplitude[0] = 2.0;
    g_CA_POWER_Para.threshold_current_amplitude[1] = 2.0;
    
    g_CA_POWER_Para.threshold_current_over_count[0] = 50;
    g_CA_POWER_Para.threshold_current_over_count[1] = 50;
    
    g_CA_POWER_Para.threshold_warning_count[0] = 50;
    g_CA_POWER_Para.threshold_warning_count[1] = 50;
    
    // motor control initialize
    memset(&g_CA_FC_Calc, 0, sizeof(g_CA_FC_Calc));
    memset(&g_CA_FC_Para, 0, sizeof(g_CA_FC_Para));
    
    g_CA_FC_Para.invT = 1000.0; // 1/T=1000hz
    g_CA_FC_Para.invTpower = 1000000.0; // 1/T^2

    // init coef of ctrl when start up
    Inner_Weak_Coef_Init();
    Outer_Weak_Coef_Init();
}

void CA_Proc(double u_ctrl[], double angleInc[], double attitude[], u32 count)
{
    // power control
    CA_Power_Ctrl(count);

    // motor control
    CA_Frequency_Ctrl(u_ctrl, angleInc, attitude);
}




