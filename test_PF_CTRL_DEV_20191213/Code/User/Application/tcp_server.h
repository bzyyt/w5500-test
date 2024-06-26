/**
******************************************************************************
* @file         tcp_server.h
* @version      V1.0
* @date         2020-06-05
* @brief        tcp_server.c的头文件
*        
* @company      深圳炜世科技有限公司
* @information  WIZnet W5500官方代理商，全程技术支持，价格优势大！
* @website      www.wisioe.com
* @forum        www.w5500.cn
* @Support      QQ:2571856470；Mob:18998931307
* @Email        support@wisioe.com
******************************************************************************
*/
#ifndef _TCP_ERVER_H_
#define _TCP_ERVER_H_

#include <stdint.h>

/* DATA_BUF_SIZE define for Loopback example */
#ifndef DATA_BUF_SIZE
#define DATA_BUF_SIZE			2048
#endif

/* TCP server Loopback test example */
void do_tcp_server(void);

#endif
