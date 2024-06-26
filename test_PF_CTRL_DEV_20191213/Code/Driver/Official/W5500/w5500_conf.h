/**
******************************************************************************
* @file         w5500_conf.h
* @version      V1.0
* @date         2020-06-05
* @brief        w5500_conf.c的头文件
*        
* @company      深圳炜世科技有限公司
* @information  WIZnet W5500官方代理商，全程技术支持，价格优势大！
* @website      www.wisioe.com
* @forum        www.w5500.cn
* @qqGroup      579842114
* @Support      QQ:2571856470；Mob:18998931307
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

extern uint8  local_ip[4];                             // 定义w5500默认IP地址
extern uint8  subnet[4];                               // 定义w5500默认子网掩码
extern uint8  gateway[4];                              // 定义w5500默认网关
extern uint8  dns_server[4];                           // 定义DNS Server IP地址
	
extern uint8  remote_ip[4];                            // 远端IP地址
extern uint16 remote_port;                             // 远端端口号
extern uint16 local_port;                              // 定义本地端口

extern uint32 dhcp_time;                               // DHCP运行计数
// extern vu8    ntptimer;                                // NTP运行计数


#define TX_RX_MAX_BUF_SIZE      2048

#define IP_FROM_DEFINE          0                      // 使用初始定义的IP信息
#define IP_FROM_DHCP            1                      // 使用DHCP获取IP信息
#define IP_FROM_FLASH           2                      // 从FLASH中读取IP信息
    
#define FW_VER_HIGH             1
#define FW_VER_LOW              0

extern uint8 ip_from;                                  // 选择IP信息配置源


#pragma pack(1)
/*此结构体定义了w5500可供配置的主要参数*/
typedef struct _CONFIG_MSG                      
{
  uint8 mac[6];            // MAC地址
  uint8 lip[4];            // local IP本地IP地址
  uint8 sub[4];            // 子网掩码
  uint8 gw[4];             // 网关  
  uint8 dns[4];            // DNS服务器地址
  uint8 rip[4];            // remote IP远程IP地址
  uint8 sw_ver[2];         // 软件版本号
}CONFIG_MSG;

#pragma pack()

extern CONFIG_MSG ConfigMsg;

/*MCU配置相关函数*/
void reboot(void);

/*w5500基本配置相关函数*/
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
void w5500Init(void);
#endif //_w5500_CONF_H_
