/**
******************************************************************************
* @file         w5500_conf.h
* @version      V1.0
* @date         2020-06-05
* @brief        w5500_conf.c��ͷ�ļ�
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
#ifndef _W5500_CONF_H_
#define _W5500_CONF_H_

#include "stm32f4xx.h"
#include "stdio.h"
#include "types.h"
#define __GNUC__

typedef  void (*pFunction)(void);

extern uint16 rxlen;
extern uint8  rxbuf[1024];

extern uint8  local_ip[4];                             // ����w5500Ĭ��IP��ַ
extern uint8  subnet[4];                               // ����w5500Ĭ����������
extern uint8  gateway[4];                              // ����w5500Ĭ������
extern uint8  dns_server[4];                           // ����DNS Server IP��ַ
	
extern uint8  remote_ip[4];                            // Զ��IP��ַ
extern uint16 remote_port;                             // Զ�˶˿ں�
extern uint16 local_port;                              // ���屾�ض˿�

extern uint32 dhcp_time;                               // DHCP���м���
// extern vu8    ntptimer;                                // NTP���м���


#define TX_RX_MAX_BUF_SIZE      2048

#define IP_FROM_DEFINE          0                      // ʹ�ó�ʼ�����IP��Ϣ
#define IP_FROM_DHCP            1                      // ʹ��DHCP��ȡIP��Ϣ
#define IP_FROM_FLASH           2                      // ��FLASH�ж�ȡIP��Ϣ
    
#define FW_VER_HIGH             1
#define FW_VER_LOW              0

extern uint8 ip_from;                                  // ѡ��IP��Ϣ����Դ


#pragma pack(1)
/*�˽ṹ�嶨����w5500�ɹ����õ���Ҫ����*/
typedef struct _CONFIG_MSG                      
{
  uint8 mac[6];            // MAC��ַ
  uint8 lip[4];            // local IP����IP��ַ
  uint8 sub[4];            // ��������
  uint8 gw[4];             // ����  
  uint8 dns[4];            // DNS��������ַ
  uint8 rip[4];            // remote IPԶ��IP��ַ
  uint8 sw_ver[2];         // ����汾��
}CONFIG_MSG;

#pragma pack()

extern CONFIG_MSG ConfigMsg;

/*MCU������غ���*/
void reboot(void);

/*w5500����������غ���*/
void reset_break_gpio_init(void);
void reset_w5500(void);
void iinchip_csoff(void);
void iinchip_cson(void);
uint8 IINCHIP_SpiSendData(uint8 dat);
void IINCHIP_WRITE(uint32 addrbsb,uint8 data);
uint8 IINCHIP_READ(uint32 addrbsb);
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len);
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len);
void set_w5500_netinfo(void);
void set_w5500_mac(void);
void dhcp_timer_init(void);
void ntp_timer_init(void);
void timer2_init(void);
void timer2_isr(void);
uint8 getPHYStatus( void );
void PHY_check(void);
void setKPALVTR(SOCKET i,uint8 val);
#endif //_w5500_CONF_H_
