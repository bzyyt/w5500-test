
#include "UartAdapter.h"
#include "Description.h"
#include "string.h"
#include "FPGADefine.h"

//uart interrupt priority 
#define UART_SUB_PRIORITY_0 0x00
#define UART_SUB_PRIORITY_1 0x01
#define UART_SUB_PRIORITY_2 0x02
#define UART_SUB_PRIORITY_3 0x03

//DMA uart memory
UartData g_UartData[UART_MAX_CHANNEL_NUMBER];

//
u8 USART1_TX_BUSY=0; //0������ 1:���ڷ���
u8 USART2_TX_BUSY=0; //0������ 1:���ڷ���
u8 USART3_TX_BUSY=0; //0������ 1:���ڷ���

uint16_t GetDataBits(DataBits dataBits)
{
    uint16_t ret = USART_WordLength_8b;
    switch(dataBits)
    {
        case DataBits_8:
            ret = USART_WordLength_8b;
            break;
        case DataBits_9:
            ret = USART_WordLength_9b;
            break;
        default:
            break;            
    }
    
    return ret;
}

uint16_t GetStopBits(StopBits stopBits)
{
    uint16_t ret = USART_StopBits_1;
    switch(stopBits)
    {
        case StopBits_1:
            ret = USART_StopBits_1;
            break;
        case StopBits_0_half:
            ret = USART_StopBits_0_5;
            break;
        case StopBits_2:
            ret = USART_StopBits_2;
            break;
        case StopBits_1_half:
            ret = USART_StopBits_1_5;
            break;
        default:
            break;            
    }
    
    return ret;
}

uint16_t GetParityBits(ParityBits parityBits)
{
    uint16_t ret = USART_StopBits_1;
    switch(parityBits)
    {
        case ParityBits_None:
            ret = USART_Parity_No;
            break;
        case ParityBits_Odd:
            ret = USART_Parity_Odd;
            break;
        case ParityBits_Even:
            ret = USART_Parity_Even;
            break;
        default:
            break;            
    }
    
    return ret;
}

void ConfigGPIO_Uart1()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* Connect the GPIOA9 & GPIOB7 to USART1 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

    /* Connect the GPIOA9 & GPIOB7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);    
}

void ConfigGPIO_Uart2()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    /* Connect the GPIOA2 & GPIOD6 to USART1 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

    /* Connect the GPIOA2 & GPIOD6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_Init(GPIOD, &GPIO_InitStructure);    
}

void ConfigGPIO_Uart3()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    
    /* Connect the GPIOC10 & GPIOC11 to USART6 */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART6);
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART6);

    /* Connect the GPIOC10 & GPIOC11 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void USART1_DMA_Init(u32 baudrate, DataBits dataBits, StopBits stopBits, ParityBits parityBits, Uart_Hander hander, u8 isTxEnable, u8 isRxEnable)
{
    NVIC_InitTypeDef NVIC_InitStructure ;  	   		//�����жϽṹ��          
    USART_InitTypeDef USART_InitStructure;    		//���崮�ڽṹ��    
    DMA_InitTypeDef DMA_InitStructure;  	   		//����DMA�ṹ��  
    
    /* ����IO����*/
    ConfigGPIO_Uart1();   

    
    /* �������ڲ������� */ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  	  //�򿪴��ڶ�Ӧ������ʱ��     
    USART_InitStructure.USART_BaudRate = baudrate;  
	USART_InitStructure.USART_WordLength = GetDataBits(dataBits);    
    USART_InitStructure.USART_StopBits = GetStopBits(stopBits);    
    USART_InitStructure.USART_Parity = GetParityBits(parityBits);    
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;               
    USART_Init(USART1,&USART_InitStructure);    				  //��ʼ������  
      
    /* �����ж����� */  
    USART_ITConfig(USART1,USART_IT_TC,DISABLE);  
    USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);  
    USART_ITConfig(USART1,USART_IT_TXE,DISABLE);  
    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);     		  	  //�򿪿����ж�
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;             //ͨ������Ϊ�����ж�    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USER_INT_PRIORITY_UART;  //�ж�ռ�ȵȼ�  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_SUB_PRIORITY_0;         //�ж���Ӧ���ȼ�   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //���ж�    
    NVIC_Init(&NVIC_InitStructure);     
          
    /* Enable USART1 DMA Rx Tx request */
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);  
    
    /* �������� */   
    USART_Cmd(USART1, ENABLE);      
	
	/* ���ڷ���DMA���� */ 	 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  		//����DMAʱ�� 
    /* DMA�����ж����� */ 
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;  		
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USER_INT_PRIORITY_UART;  		
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_SUB_PRIORITY_1;  			
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  				
    NVIC_Init(&NVIC_InitStructure);  								
    // UART1 TX: DMA2 stream7, channel 4
    DMA_DeInit(DMA2_Stream7);  									//DMAͨ������ 
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;   				   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR); //�����ַ      
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)g_UartData[ARMUart1].Send_Buffer; //�ڴ��ַ     
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  			//dma���䷽��     
    DMA_InitStructure.DMA_BufferSize = MAX_SEND_LEN_BUFFER;  				//����DMA�ڴ���ʱ�������ĳ���     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//����DMA���������ģʽ��һ������      
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			//����DMA���ڴ����ģʽ     
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���������ֳ�      
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte; //�ڴ������ֳ�      
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						//����DMA�Ĵ���ģʽ    
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  				//����DMA�����ȼ���          
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      	    //ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ       
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;   //ָ����FIFO��ֵˮƽ              
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;         //ָ����Burstת�������ڴ洫��      
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //ָ����Burstת��������Χת��               
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);    						//����DMA2��ͨ��      
    DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);     					//ʹ���ж� 
    
    /* ��3�������ڽ���DMA���� */     																    //����DMAʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
    // UART1 RX: DMA2 stream5, channel 4
    DMA_DeInit(DMA2_Stream5);  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  					 //DMAͨ������     
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);  //�����ַ      
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)g_UartData[ARMUart1].Recv_Buffer;//�ڴ��ַ      
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  			 //dma���䷽��    
    DMA_InitStructure.DMA_BufferSize = MAX_RECV_LEN_BUFFER;  				 //����DMA�ڴ���ʱ�������ĳ���   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	 //����DMA���������ģʽ��һ������    
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			 //����DMA���ڴ����ģʽ    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���������ֳ�       
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  	 //�ڴ������ֳ�     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						 //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  			 //����DMA�����ȼ���         
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      		 //ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ       
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;    //ָ����FIFO��ֵˮƽ              
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;          //ָ����Burstת�������ڴ洫��  
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  //ָ����Burstת��������Χת��                   
    DMA_Init(DMA2_Stream5, &DMA_InitStructure);    						 //����DMA2��ͨ��  
    
    DMA_Cmd(DMA2_Stream5,ENABLE);  	
    
}

void USART2_DMA_Init(u32 baudrate, DataBits dataBits, StopBits stopBits, ParityBits parityBits, Uart_Hander hander, u8 isTxEnable, u8 isRxEnable)
{
    NVIC_InitTypeDef NVIC_InitStructure ;  	   		//�����жϽṹ��        
    USART_InitTypeDef USART_InitStructure;    		//���崮�ڽṹ��        
    DMA_InitTypeDef DMA_InitStructure;  	   		//����DMA�ṹ��  
    
    /* ����IO���� */    
    ConfigGPIO_Uart2();  
    
    /* �������ڲ������� */ 
    RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  	  //�򿪴��ڶ�Ӧ������ʱ��     
    USART_InitStructure.USART_BaudRate = baudrate;  
	USART_InitStructure.USART_WordLength = GetDataBits(dataBits);    
    USART_InitStructure.USART_StopBits = GetStopBits(stopBits);    
    USART_InitStructure.USART_Parity = GetParityBits(parityBits);    
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;               
    USART_Init(USART2,&USART_InitStructure);    				  //��ʼ������  
      
    /* �����ж����� */  
    USART_ITConfig(USART2,USART_IT_TC,DISABLE);  
    USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);  
    USART_ITConfig(USART2,USART_IT_TXE,DISABLE);  
    USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);     		  	  //�򿪿����ж�
       
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;             //ͨ������Ϊ�����ж�    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USER_INT_PRIORITY_UART;  //�ж�ռ�ȵȼ�  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_SUB_PRIORITY_0;         //�ж���Ӧ���ȼ�   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //���ж�    
    NVIC_Init(&NVIC_InitStructure);     
          
    /* Enable USART1 DMA Rx Tx request */
    USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);  
    USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);  
    
    /* �������� */   
    USART_Cmd(USART2, ENABLE);      
	
	/* ���ڷ���DMA���� */ 	 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  		//����DMAʱ�� 
    /* DMA�����ж����� */ 
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;  		
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USER_INT_PRIORITY_UART;  		
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_SUB_PRIORITY_1;  			
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  				
    NVIC_Init(&NVIC_InitStructure);  								
    // UART2 TX: DMA1 stream6, channel 4
    DMA_DeInit(DMA1_Stream6);  									//DMAͨ������ 
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;   				   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR); //�����ַ      
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)g_UartData[ARMUart2].Send_Buffer; //�ڴ��ַ     
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  			//dma���䷽��     
    DMA_InitStructure.DMA_BufferSize = MAX_SEND_LEN_BUFFER;  				//����DMA�ڴ���ʱ�������ĳ���     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//����DMA���������ģʽ��һ������      
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			//����DMA���ڴ����ģʽ     
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���������ֳ�      
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte; //�ڴ������ֳ�      
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						//����DMA�Ĵ���ģʽ    
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  				//����DMA�����ȼ���          
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      	    //ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ       
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;   //ָ����FIFO��ֵˮƽ              
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;         //ָ����Burstת�������ڴ洫��      
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //ָ����Burstת��������Χת��               
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);    						//����DMA2��ͨ��      
    DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,ENABLE);     					//ʹ���ж� 
    
    /* ��3�������ڽ���DMA���� */     																    //����DMAʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  
     // UART2 RX: DMA1 stream5, channel 4
    DMA_DeInit(DMA1_Stream5);  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  					 //DMAͨ������     
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);  //�����ַ      
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)g_UartData[ARMUart2].Recv_Buffer;//�ڴ��ַ      
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  			 //dma���䷽��    
    DMA_InitStructure.DMA_BufferSize = MAX_RECV_LEN_BUFFER;  				 //����DMA�ڴ���ʱ�������ĳ���   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	 //����DMA���������ģʽ��һ������    
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			 //����DMA���ڴ����ģʽ    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���������ֳ�       
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  	 //�ڴ������ֳ�     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						 //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  			 //����DMA�����ȼ���         
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      		 //ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ       
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;    //ָ����FIFO��ֵˮƽ              
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;          //ָ����Burstת�������ڴ洫��  
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  //ָ����Burstת��������Χת��                   
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);    						 //����DMA2��ͨ��  
    
    DMA_Cmd(DMA2_Stream5,ENABLE);  	
    
}

void USART3_DMA_Init(u32 baudrate, DataBits dataBits, StopBits stopBits, ParityBits parityBits, Uart_Hander hander, u8 isTxEnable, u8 isRxEnable)
{
    NVIC_InitTypeDef NVIC_InitStructure ;  	   		//�����жϽṹ��             
    USART_InitTypeDef USART_InitStructure;    		//���崮�ڽṹ��    
    DMA_InitTypeDef DMA_InitStructure;  	   		//����DMA�ṹ��  
    
    /* ����IO���� */    
    ConfigGPIO_Uart3();  
    
    /* �������ڲ������� */ 
    RCC_APB2PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  	  //�򿪴��ڶ�Ӧ������ʱ��     
    USART_InitStructure.USART_BaudRate = baudrate;  
	USART_InitStructure.USART_WordLength = GetDataBits(dataBits);    
    USART_InitStructure.USART_StopBits = GetStopBits(stopBits);    
    USART_InitStructure.USART_Parity = GetParityBits(parityBits);    
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;               
    USART_Init(USART3,&USART_InitStructure);    				  //��ʼ������  
      
    /* �����ж����� */  
    USART_ITConfig(USART3,USART_IT_TC,DISABLE);  
    USART_ITConfig(USART3,USART_IT_RXNE,DISABLE);  
    USART_ITConfig(USART3,USART_IT_TXE,DISABLE);  
    USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);     		  	  //�򿪿����ж�
       
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;             //ͨ������Ϊ�����ж�    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USER_INT_PRIORITY_UART;  //�ж�ռ�ȵȼ�  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_SUB_PRIORITY_0;         //�ж���Ӧ���ȼ�   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //���ж�    
    NVIC_Init(&NVIC_InitStructure);     
          
    /* Enable USART1 DMA Rx Tx request */
    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);  
    USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);  
    
    /* �������� */   
    USART_Cmd(USART3, ENABLE);      
	
	/* ���ڷ���DMA���� */ 	 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  		//����DMAʱ�� 
    /* DMA�����ж����� */ 
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;  		
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USER_INT_PRIORITY_UART;  		
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART_SUB_PRIORITY_1;  			
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  				
    NVIC_Init(&NVIC_InitStructure);  								
    // UART2 TX: DMA3 stream6, channel 4
    DMA_DeInit(DMA1_Stream3);  									//DMAͨ������ 
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;   				   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR); //�����ַ      
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)g_UartData[ARMUart3].Send_Buffer; //�ڴ��ַ     
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  			//dma���䷽��     
    DMA_InitStructure.DMA_BufferSize = MAX_SEND_LEN_BUFFER;  				//����DMA�ڴ���ʱ�������ĳ���     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//����DMA���������ģʽ��һ������      
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			//����DMA���ڴ����ģʽ     
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���������ֳ�      
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte; //�ڴ������ֳ�      
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						//����DMA�Ĵ���ģʽ    
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  				//����DMA�����ȼ���          
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      	    //ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ       
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;   //ָ����FIFO��ֵˮƽ              
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;         //ָ����Burstת�������ڴ洫��      
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //ָ����Burstת��������Χת��               
    DMA_Init(DMA1_Stream3, &DMA_InitStructure);    						//����DMA2��ͨ��      
    DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);     					//ʹ���ж� 
    
    /* ��3�������ڽ���DMA���� */     																    //����DMAʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  
     // UART3 RX: DMA1 stream1, channel 4
    DMA_DeInit(DMA1_Stream1);  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  					 //DMAͨ������     
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);  //�����ַ      
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)g_UartData[ARMUart3].Recv_Buffer;//�ڴ��ַ      
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  			 //dma���䷽��    
    DMA_InitStructure.DMA_BufferSize = MAX_RECV_LEN_BUFFER;  				 //����DMA�ڴ���ʱ�������ĳ���   
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	 //����DMA���������ģʽ��һ������    
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  			 //����DMA���ڴ����ģʽ    
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //���������ֳ�       
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  	 //�ڴ������ֳ�     
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  						 //����DMA�Ĵ���ģʽ  
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  			 //����DMA�����ȼ���         
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      		 //ָ�����FIFOģʽ��ֱ��ģʽ������ָ������ �� ��ʹ��FIFOģʽ       
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;    //ָ����FIFO��ֵˮƽ              
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;          //ָ����Burstת�������ڴ洫��  
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  //ָ����Burstת��������Χת��                   
    DMA_Init(DMA1_Stream1, &DMA_InitStructure);    						 //����DMA2��ͨ��  
    
    DMA_Cmd(DMA2_Stream1,ENABLE);  	
    
}

/***********************************************************ARM UART1**********************************************************/

void USART1_DMA_Send_Once_Data(uint8_t *data,uint16_t size)  
{  
    /* æ������ */ 
    if (USART1_TX_BUSY) return; 
    /* �ر�DMA */
    DMA_Cmd(DMA2_Stream7,DISABLE);
    /* �������� */ 
    memcpy(g_UartData[ARMUart1].Send_Buffer,data,size);
    /* ���ô������ݳ���  */
    DMA_SetCurrDataCounter(DMA2_Stream7,size);  
    /* ��DMA,��ʼ���� */ 
    DMA_Cmd(DMA2_Stream7,ENABLE);  
    USART1_TX_BUSY = 1;
}  

void DMA2_Stream7_IRQHandler(void) 	  
{  
    if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7) != RESET)   
    {  
        /* �����־λ */ 
        DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);  
        /* �ر�DMA */ 
        DMA_Cmd(DMA2_Stream7,DISABLE);  
        /* �򿪷�������ж�,������������ֽ� */ 
        USART_ITConfig(USART1,USART_IT_TC,ENABLE);  
    }  
}  

uint8_t USART1_RX_Finish_IRQ()
{
    uint16_t len = 0;

    USART1->SR;
    USART1->DR; 									//��USART_IT_IDLE��־   
    DMA_Cmd(DMA2_Stream5, DISABLE);				    //�ر�DMA   
    DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5);	//�����־λ

    /* ��ý���֡֡�� */
    len = MAX_RECV_LEN_BUFFER - DMA_GetCurrDataCounter(DMA2_Stream5);
    memcpy(g_UartData[ARMUart1].Cache_Buffer, g_UartData[ARMUart1].Recv_Buffer, len);
    /* ���ô������ݳ��� */
    DMA_SetCurrDataCounter(DMA2_Stream5, MAX_RECV_LEN_BUFFER);
    /* ��DMA */
    DMA_Cmd(DMA2_Stream5, ENABLE);
    return len;
}
  
void USART1_IRQHandler(void) 		   
{  
    /* ��������жϴ��� */ 
    if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)  
    {  
        /* �رշ�������ж� */ 
        USART_ITConfig(USART1,USART_IT_TC,DISABLE);  
        /* ������� */ 
        USART1_TX_BUSY = 0;  
    }   
      
    /* ��������жϴ��� */ 
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{ 
    	g_UartData[ARMUart1].Recv_Length = USART1_RX_Finish_IRQ();
        if(g_UartData[ARMUart1].Recv_Length > 0)
        {
            g_UartData[ARMUart1].Recv_Status = 0x01;
        }        
	}  
}  

/***********************************************************ARM UART2**********************************************************/
 
void USART2_DMA_Send_Once_Data(uint8_t *data,uint16_t size)  
{  
    /* æ������ */ 
    if (USART2_TX_BUSY) return;
    /* �ر�DMA */
    DMA_Cmd(DMA1_Stream6,DISABLE);
    /* �������� */ 
    memcpy(g_UartData[ARMUart2].Send_Buffer,data,size);
    /* ���ô������ݳ���  */
    DMA_SetCurrDataCounter(DMA1_Stream6,size);  
    /* ��DMA,��ʼ���� */ 
    DMA_Cmd(DMA1_Stream6,ENABLE);  
    USART2_TX_BUSY = 1;
}  

void DMA1_Stream6_IRQHandler(void) 	  
{  
    if(DMA_GetITStatus(DMA1_Stream6,DMA_IT_TCIF6) != RESET)   
    {  
        /* �����־λ */ 
        DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);  
        /* �ر�DMA */ 
        DMA_Cmd(DMA1_Stream6,DISABLE);  
        /* �򿪷�������ж�,������������ֽ� */ 
        USART_ITConfig(USART2,USART_IT_TC,ENABLE);  
    }  
} 

uint8_t USART2_RX_Finish_IRQ()
{
    uint16_t len = 0;

    USART2->SR;
    USART2->DR; 									//��USART_IT_IDLE��־   
    DMA_Cmd(DMA1_Stream5, DISABLE);				    //�ر�DMA   
    DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);	//�����־λ

    /* ��ý���֡֡�� */
    len = MAX_RECV_LEN_BUFFER - DMA_GetCurrDataCounter(DMA1_Stream5);
    memcpy(g_UartData[ARMUart2].Cache_Buffer, g_UartData[ARMUart2].Recv_Buffer, len);
    /* ���ô������ݳ��� */
    DMA_SetCurrDataCounter(DMA1_Stream5, MAX_RECV_LEN_BUFFER);
    /* ��DMA */
    DMA_Cmd(DMA1_Stream5, ENABLE);
    return len;

}
     
void USART2_IRQHandler(void) 		   
{  
    /* ��������жϴ��� */ 
    if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)  
    {  
        /* �رշ�������ж� */ 
        USART_ITConfig(USART2,USART_IT_TC,DISABLE);  
        /* ������� */ 
        USART2_TX_BUSY = 0;  
    }   
      
    /* ��������жϴ��� */ 
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{ 
    	g_UartData[ARMUart2].Recv_Length = USART2_RX_Finish_IRQ();
        if(g_UartData[ARMUart2].Recv_Length > 0)
        {
            g_UartData[ARMUart2].Recv_Status = 0x01;
        }
	}  
}  


/***********************************************************ARM UART3**********************************************************/

void USART3_DMA_Send_Once_Data(uint8_t *data,uint16_t size)  
{  
    /* æ������ */ 
    if (USART3_TX_BUSY) return;
    /* �ر�DMA */
    DMA_Cmd(DMA1_Stream3,DISABLE);
    /* �������� */ 
    memcpy(g_UartData[ARMUart3].Send_Buffer,data,size);
    /* ���ô������ݳ���  */
    DMA_SetCurrDataCounter(DMA1_Stream3,size);  
    /* ��DMA,��ʼ���� */ 
    DMA_Cmd(DMA1_Stream3,ENABLE);  
    USART3_TX_BUSY = 1;
}  

void DMA1_Stream3_IRQHandler(void) 	  
{  
    if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3) != RESET)   
    {  
        /* �����־λ */ 
        DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);  
        /* �ر�DMA */ 
        DMA_Cmd(DMA1_Stream3,DISABLE);  
        /* �򿪷�������ж�,������������ֽ� */ 
        USART_ITConfig(USART3,USART_IT_TC,ENABLE);  
    }  
} 

uint8_t USART3_RX_Finish_IRQ()
{
    uint16_t len = 0;

    USART3->SR;
    USART3->DR; 									//��USART_IT_IDLE��־   
    DMA_Cmd(DMA1_Stream1, DISABLE);				    //�ر�DMA   
    DMA_ClearFlag(DMA1_Stream1, DMA_FLAG_TCIF1);	//�����־λ

    /* ��ý���֡֡�� */
    len = MAX_RECV_LEN_BUFFER - DMA_GetCurrDataCounter(DMA1_Stream1);
    memcpy(g_UartData[ARMUart3].Cache_Buffer, g_UartData[ARMUart3].Recv_Buffer, len);
    /* ���ô������ݳ��� */
    DMA_SetCurrDataCounter(DMA1_Stream1, MAX_RECV_LEN_BUFFER);
    /* ��DMA */
    DMA_Cmd(DMA1_Stream1, ENABLE);
    return len;

}
     
void USART3_IRQHandler(void) 		   
{  
    /* ��������жϴ��� */ 
    if(USART_GetITStatus(USART3, USART_IT_TC) != RESET)  
    {  
        /* �رշ�������ж� */ 
        USART_ITConfig(USART3,USART_IT_TC,DISABLE);  
        /* ������� */ 
        USART3_TX_BUSY = 0;  
    }   
      
    /* ��������жϴ��� */ 
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{ 
    	g_UartData[ARMUart3].Recv_Length = USART3_RX_Finish_IRQ();
        if(g_UartData[ARMUart3].Recv_Length > 0)
        {
            g_UartData[ARMUart3].Recv_Status = 0x01;
        }
	}  
}

u16 FPGA_Get_BaudRate(u32 baudrate)
{
    if(baudrate == 2400) return 0x00;
    else if(baudrate == 4800) return 0x01;
    else if(baudrate == 9600) return 0x02;
    else if(baudrate == 19200) return 0x03;
    else if(baudrate == 38400) return 0x04;
    else if(baudrate == 57600) return 0x05;
    else if(baudrate == 115200) return 0x06;
    else if(baudrate == 230400) return 0x07;
    else if(baudrate == 384000) return 0x08;
    else if(baudrate == 460800) return 0x09;
    else return 0x06;
}

u16 FPGA_Get_Config(StopBits stopBits, ParityBits parityBits, u8 enable)
{
    u16 config = 0x00;
    if(stopBits == StopBits_2)
    {
        config |= UART_STOPBIT_TWO;
    }
    else
    {
        config |= UART_STOPBIT_ONE;
    }
    
    if(parityBits == ParityBits_None)
    {
        config |= UART_PARITY_NO;
    }
    else if(parityBits == ParityBits_Odd)
    {
        config |= UART_PARITY_ODD;
    }
    else if(parityBits == ParityBits_Even)
    {
        config |= UART_PARITY_EVEN;
    }
    
    if(enable)
    {
        config |= UART_ENABLE;
    }
    else
    {
        config |= UART_DISABLE;
    }
    
    return config;
}

bool FPGA_UART_Init(UartType uartType, u32 baudrate, StopBits stopBits, ParityBits parityBits, u8 isTxEnable, u8 isRxEnable)
{
    // RX
    _UART_RXBUFF_(uartType - FPGAUart1) = 0x0001;
    _UART_RXBAUD_(uartType - FPGAUart1) = FPGA_Get_BaudRate(baudrate);
    _UART_RCONFIG_(uartType - FPGAUart1) = FPGA_Get_Config(stopBits, parityBits, isRxEnable);
    
    //TX
    _UART_TXBAUD_(uartType - FPGAUart1) = FPGA_Get_BaudRate(baudrate);
    _UART_TCONFIG_(uartType - FPGAUart1) = FPGA_Get_Config(stopBits, parityBits, isTxEnable);
    
    return true;
}

bool FPGA_UART_Send(UartType uartType, u8 *data, u32 len)
{
    int i = 0;
    
    for(i = 0; i<len; i++)
    {
        _UART_TXBUFF_(uartType - FPGAUart1) = data[i];       
    }
   
    return true; 
}

u32 FPGA_UART_Recv(UartType uartType, u8 *data)
{
    u16 rxLen = _UART_RSTATE(uartType - FPGAUart1) & 0xFF;
    u32 i = 0;
    
    for(i = 0; i<rxLen; i++)
    {
        data[i] = _UART_RXBUFF_(uartType - FPGAUart1);
    }
       
    return rxLen;
}

u32 RecvUartMsg(u16 uartType, u8 *buf)
{
    u32 retValue = 0;
    
    switch(uartType)
    {
        case ARMUart1:
            break;
        case ARMUart2:
            break;
        case ARMUart3:
            break;
        case FPGAUart1:
            retValue = FPGA_UART_Recv(FPGAUart1, buf);
            break;
        case FPGAUart2:
            retValue = FPGA_UART_Recv(FPGAUart2, buf);
            break;
        case FPGAUart3:
            retValue = FPGA_UART_Recv(FPGAUart3, buf);
        default:
            break;
    }
    
    return retValue;
}


bool Uart_Init(UartType uartType, u32 baudrate, DataBits dataBits, StopBits stopBits, ParityBits parityBits, Uart_Hander hander, u8 isTxEnable, u8 isRxEnable)
{
    switch(uartType)
    {
        case ARMUart1:
            USART1_DMA_Init(baudrate, dataBits, stopBits, parityBits, hander, isTxEnable, isRxEnable);
            break;
        case ARMUart2:
            USART2_DMA_Init(baudrate, dataBits, stopBits, parityBits, hander, isTxEnable, isRxEnable);
            break;
        case ARMUart3:
            USART3_DMA_Init(baudrate, dataBits, stopBits, parityBits, hander, isTxEnable, isRxEnable);
            break;
        case FPGAUart1:
            FPGA_UART_Init(FPGAUart1, baudrate, stopBits, parityBits, isTxEnable, isRxEnable);
            break;
        case FPGAUart2:
            FPGA_UART_Init(FPGAUart2, baudrate, stopBits, parityBits, isTxEnable, isRxEnable);
            break;
        case FPGAUart3:
            FPGA_UART_Init(FPGAUart3, baudrate, stopBits, parityBits, isTxEnable, isRxEnable);
            break;
        default:
            break;
    }
    return true;
}

bool SendUartMsg(u16 uartType, u8 start, u8 len, u8 *buf)
{
    if(len == 0) return false;
    
    switch(uartType)
    {
        case ARMUart1:
            USART1_DMA_Send_Once_Data(buf, len);
            break;
        case ARMUart2:
            USART2_DMA_Send_Once_Data(buf, len);
            break;
        case ARMUart3:
            USART3_DMA_Send_Once_Data(buf, len);
            break;
        case FPGAUart1:
            FPGA_UART_Send(FPGAUart1, buf, len);
            break;
        case FPGAUart2:
            FPGA_UART_Send(FPGAUart2, buf, len);
            break;
        case FPGAUart3:
            FPGA_UART_Send(FPGAUart3, buf, len);
        default:
            break;
    }
    return true;
}


