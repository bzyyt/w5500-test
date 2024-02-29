/**
******************************************************************************
* @file         w5500_conf.c
* @version      V1.0
* @date         2020-06-05
* @brief        ����MCU����ֲw5500������Ҫ�޸ĵ��ļ�������w5500��MAC��IP��ַ
*        
* @company      ��������Ƽ����޹�˾
* @information  WIZnet W5500�ٷ������̣�ȫ�̼���֧�֣��۸����ƴ�
* @website      www.wisioe.com
* @forum        www.w5500.cn
* @qqGroup      579842114
* @Support      QQ:2571856470��Mob:18998931307
* @Email        support@wisioe.com
******************************************************************************
*/
#include <stdio.h> 
#include <string.h>
#include "w5500_conf.h"
#include "utility.h"
#include "w5500_control.h"
#include "socket.h"
#include "SPIAdapter.h"
#include "DelayAdapter.h"

CONFIG_MSG  ConfigMsg, RecvMsg;                             // ���ýṹ��

uint16 rxlen=0;
uint8 rxbuf[1024];

/*MAC��ַ���ֽڱ���Ϊż��
  ������w5500�����������ͬһ�ֳ���������ʹ�ò�ͬ��MAC��ַ
*/
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};

/*����Ĭ��IP��Ϣ*/
uint8 local_ip[4] = {192,168,1,232};                        // ����w5500Ĭ��IP��ַ
uint8 subnet[4] = {255,255,255,0};                          // ����w5500Ĭ����������
uint8 gateway[4] = {192,168,1,1};                           // ����w5500Ĭ������
uint8 dns_server[4] = {114,114,114,114};                    // ����w5500Ĭ��DNS

uint16 local_port = 8080;                                   // ���屾�ض˿�

/*����Զ��IP��Ϣ*/
uint8  remote_ip[4] = {192,168,1,100};                      // Զ��IP��ַ
uint16 remote_port = 8080;                                  // Զ�˶˿ں�

/*IP���÷���ѡ��������ѡ��*/
uint8  ip_from=IP_FROM_DEFINE;      

uint32 ms=0;                                                // �������
// vu8    ntptimer = 0;                                        // NPT�����

/**
*@brief  ��ʼ��W5500�ĸ�λ���жϹܽ�
*@param  ��
*@return ��
*/
void reset_break_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);
  __HAL_RCC_GPIOH_CLK_ENABLE(); // ����GPIOHʱ��
  /* PH_01 -> RST */
  GPIO_InitStructure.Pin = GPIO_PIN_1;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
  /* PH_05 -> INT */
  GPIO_InitStructure.Pin = GPIO_PIN_5;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
*@brief  ����W5500��IP��ַ
*@param  ��
*@return ��
*/
void set_w5500_netinfo(void)
{
#ifdef  _HTTP_SERVER_H_
	uint8 temp;
#endif
  // ���ƶ����������Ϣ�����ýṹ��
  memcpy(ConfigMsg.mac, mac, 6);
  memcpy(ConfigMsg.lip,local_ip,4);
  memcpy(ConfigMsg.sub,subnet,4);
  memcpy(ConfigMsg.gw,gateway,4);
  memcpy(ConfigMsg.dns,dns_server,4);
#ifdef  _HTTP_SERVER_H_
/*����ʹ��HTTP Server����ʱ��Ч��ʹ������"http_server.h"ͷ�ļ�������*/
  ReadFlashNBtye(0,&temp,1);
  if(temp != 255)
    ip_from = IP_FROM_FLASH;
  else
    ip_from = IP_FROM_DEFINE;
#endif
  switch(ip_from)
  {
    case IP_FROM_DEFINE:
      printf(" ʹ�ö����IP��Ϣ����W5500��\r\n");
      break;
#ifdef  _HTTP_SERVER_H_
    case IP_FROM_FLASH:
      printf(" ʹ��Flash��IP��Ϣ����W5500��\r\n");
      ReadFlashNBtye(0,ConfigMsg.lip,12);
      break;
#endif
  }
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  getSIPR (local_ip);
  printf(" W5500 IP��ַ   : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
  getSUBR(subnet);
  printf(" W5500 �������� : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
  getGAR(gateway);
  printf(" W5500 ����     : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
}

/**
*@brief  ����W5500��MAC��ַ
*@param  ��
*@return ��
*/
void set_w5500_mac(void)
{
  // ����������Ϣ��������Ҫѡ��
  ConfigMsg.sw_ver[0]=FW_VER_HIGH;
  ConfigMsg.sw_ver[1]=FW_VER_LOW;
  
  memcpy(ConfigMsg.mac, mac, 6);
  setSHAR(ConfigMsg.mac);
  getSHAR(mac);
  printf(" W5500 MAC��ַ  : %02x.%02x.%02x.%02x.%02x.%02x\r\n", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}


/**
*@brief  W5500��λ���ú���
*@param  ��
*@return ��
*/
void reset_w5500(void)
{
  // GPIO_ResetBits(GPIOB, GPIO_Pin_1);
  HAL_GPIO_WritePin(GPIOH,GPIO_PIN_1,GPIO_PIN_RESET);	//RST PH01��0
  delay_ms(500);
  // GPIO_SetBits(GPIOB, GPIO_Pin_1);
  HAL_GPIO_WritePin(GPIOH,GPIO_PIN_1,GPIO_PIN_SET);	//RST PH01��1
  delay_ms(1500);
}

/**
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_csoff(void)
{
	cs_low();
}

/**
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_cson(void)
{	
   cs_high();
}

/**
*@brief		STM32 SPI1��д8λ����
*@param		dat��д���8λ����
*@return	��
*/
uint8 IINCHIP_SpiSendData(uint8 dat)
{
	return(spi_read_send_byte(dat));
}

/**
*@brief		д��һ��8λ���ݵ�W5500
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data��д���8λ����
*@return	��
*/
void IINCHIP_WRITE(uint32 addrbsb,uint8 data)
{
	iinchip_csoff();
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);
	IINCHIP_SpiSendData(data);
	iinchip_cson();
}

/**
*@brief		��W5500����һ��8λ����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data����д��ĵ�ַ����ȡ����8λ����
*@return	��
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
	uint8 data = 0;
	iinchip_csoff();
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
	data = IINCHIP_SpiSendData(0x00);
	iinchip_cson();
	return data;
}

/**
*@brief		��W5500д��len�ֽ�����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   buf��д���ַ���
*@param   len���ַ�������
*@return	len�������ַ�������
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
	uint16 idx = 0;
	if(len == 0) printf(" Unexpected2 length 0\r\n");
	iinchip_csoff();
	IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
	IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
	IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
	for(idx = 0; idx < len; idx++)
	{
		IINCHIP_SpiSendData(buf[idx]);
	}
	iinchip_cson();
	return len;
}

/**
*@brief		��W5500����len�ֽ�����
*@param		addrbsb: ��ȡ���ݵĵ�ַ
*@param 	buf����Ŷ�ȡ����
*@param		len���ַ�������
*@return	len�������ַ�������
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  if(len == 0)
  {
    printf(" Unexpected2 length 0\r\n");
  }
  iinchip_csoff();
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));
  for(idx = 0; idx < len; idx++)
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();
  return len;
}

/**
*@brief  STM32ϵͳ��λ����
*@param  ��
*@return ��
*/
typedef __IO uint32_t  vu32;
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  printf(" ϵͳ�����С���\r\n");
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}

// /**
// *@brief  dhcp�õ��Ķ�ʱ����ʼ��
// *@param  ��
// *@return ��
// */
// void dhcp_timer_init(void)
// {
//   timer2_init();
// }

// /**
// *@brief  ntp�õ��Ķ�ʱ����ʼ��
// *@param  ��
// *@return ��
// */
// void ntp_timer_init(void)
// {
//   timer2_init();
// }

// /**
// *@brief  STM32��ʱ��3��ʼ��
// *@param  ��
// *@return ��
// */
// void timer2_init(void)
// {
//   TIM2_Configuration();                                        //  TIM2 ��ʱ����
//   TIM2_NVIC_Configuration();                                   //  ��ʱ�����ж����ȼ�
//   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);        //  TIM2 ���¿�ʱ�ӣ���ʼ��ʱ 
// }

// /**
// *@brief  ��ʱ��2�жϺ���
// *@param  ��
// *@return ��
// */
// void timer3_isr(void)
// {
//   ms++;  
//   if(ms>=1000)
//   {  
//     ms=0;
//     dhcp_time++;             // DHCP��ʱ��1S
// // #ifndef __NTP_H__
// //     ntptimer++;              // NTP����ʱ���1S
// // #endif
//   }
// }

/**
*@brief  ������������
*@param  ��
*@return ��
*/
void PHY_check(void)
{
  uint8 PHY_connect=0;
  PHY_connect=0x01&getPHYStatus();
  if(PHY_connect==0)
  {
    printf(" \r\n ���������Ƿ�����?\r\n");
    PHY_connect = 0;
    do
    {
      close(PHY_connect++);
    }while(PHY_connect < 4);
    PHY_connect = 0;
    while(PHY_connect == 0) 
		{
      PHY_connect=0x01&getPHYStatus();
      printf(" .");
      delay_ms(500);
    }
    printf(" \r\n");
  }
}

/**
*@brief  ��ȡPHY�Ĵ�������
*@param  ��
*@return PHYSR�Ĵ�����ֵ
*/
uint8 getPHYStatus( void )
{
  return IINCHIP_READ(PHYCFGR);
}

/**
*@brief  ����KPALVIR�Ĵ���
*@param  i�����õ�socket����
*@param  val�����õ�ʱ��(5s��λ)
*@return ��
*/
void setKPALVTR(SOCKET i,uint8 val)
{
  IINCHIP_WRITE(Sn_KPALVTR(i),val);
}
