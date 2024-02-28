
#include "ADCAdapter.h"

void ADC_Common_Config()
{
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    
	ADC_CommonStructInit(&ADC_CommonInitStructure);
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
	
	ADC_CommonInit(&ADC_CommonInitStructure);    
}

void ADC1_Config()
{
    ADC_InitTypeDef ADC_InitStructure;
	
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	
    // PA0 config to ADC1 channel 0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    
	ADC_StructInit(&ADC_InitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //Specified the resolution ratio
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //Disable the scan mode, it will be need in mutichannels mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //Enable the continual conversion
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //Disable the external edge trigger
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; //We use the software trigger, so the external trigger needn't to configure
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1; //Speficied 1 conversion channel
	
	ADC_Init(ADC1, &ADC_InitStructure);
    
	//Configure the channel conversion order is 1, and the first to conversion, and the sample time is 56 clock cycles
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_56Cycles);
	//ADC_ITConfig(ADC1, ADC_IT_EOC ,ENABLE); //The interruption occur when we finished a conversion
	ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1); //Begin the ADC conversion, and intriggered by software

}

void ADC2_Config()
{
    ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	
    // PA0 config to ADC2 channel 1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_StructInit(&ADC_InitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //Specified the resolution ratio
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //Disable the scan mode, it will be need in mutichannels mode
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //Enable the continual conversion
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //Disable the external edge trigger
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; //We use the software trigger, so the external trigger needn't to configure
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1; //Speficied 1 conversion channel
	
	ADC_Init(ADC2, &ADC_InitStructure);
    
	//Configure the channel conversion order is 1, and the first to conversion, and the sample time is 56 clock cycles
	ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_56Cycles);
	//ADC_ITConfig(ADC1, ADC_IT_EOC ,ENABLE); //The interruption occur when we finished a conversion
	ADC_Cmd(ADC2, ENABLE);
	ADC_SoftwareStartConv(ADC2); //Begin the ADC conversion, and intriggered by software

}

void Adc_Init()
{
    ADC_Common_Config();
    ADC1_Config();
    ADC2_Config();    
}

float Adc_Input(EAdcType type)
{
    if(type == ADC_1)
    {
        ADC_SoftwareStartConv(ADC1);
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
        return (float)(ADC_GetConversionValue(ADC1) * (float) 3.3 / 4096);
    }
    else
    {
        ADC_SoftwareStartConv(ADC2);
        while(ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) != SET);
        return (float)(ADC_GetConversionValue(ADC2) * (float) 3.3 / 4096);     
    }
}

