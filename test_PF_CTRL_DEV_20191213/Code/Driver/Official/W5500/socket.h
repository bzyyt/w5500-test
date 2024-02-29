/**
******************************************************************************
* @file    			socket.c
* @version 			V1.2
* @date         2020-06-05
* @brief 				socket.c的头文件
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
#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#include "stm32f4xx.h"
#include "Types.h"

/*Socket 端口选择，可按自己的习惯定义*/
#define SOCK_ARP              0
#define SOCK_PING             0
#define SOCK_UDPS             0
#define SOCK_TCPC             1
#define SOCK_TCPS             2
#define SOCK_DHCP             3
#define SOCK_DNS              4
#define SOCK_HTTPC            5
#define SOCK_HTTPS            5
#define SOCK_NETBIOS          6
#define SOCK_NTP              6
#define SOCK_WEBSOCKET        7

extern uint8 socket(SOCKET s, uint8 protocol, uint16 port, uint8 flag);                      // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close(SOCKET s);                                                                 // Close socket
extern uint8 connect(SOCKET s, uint8 * addr, uint16 port); 																	 // Establish TCP connection (Active connection)
extern void disconnect(SOCKET s);                                                            // disconnect the connection
extern uint8 listen(SOCKET s);	                                                             // Establish TCP connection (Passive connection)
extern uint16 send(SOCKET s, const uint8 * buf, uint16 len);                                 // Send data (TCP)
extern uint16 recv(SOCKET s, uint8 * buf, uint16 len);	                                     // Receive data (TCP)
extern uint16 sendto(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port);    // Send data (UDP/IP RAW)
extern uint16 recvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16  *port);      // Receive data (UDP/IP RAW)

#ifdef __MACRAW__
void macraw_open(void);
uint16 macraw_send( const uint8 * buf, uint16 len );                                         //Send data (MACRAW)
uint16 macraw_recv( uint8 * buf, uint16 len );                                               //Recv data (MACRAW)
#endif

#endif
/* _SOCKET_H_ */
