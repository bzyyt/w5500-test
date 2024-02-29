/**
******************************************************************************
* @file         tcp_server.c
* @version      V1.0
* @date         2020-06-05
* @brief        tcp server���ݻػ�����
*        
* @company      ��������Ƽ����޹�˾
* @information  WIZnet W5500�ٷ������̣�ȫ�̼���֧�֣��۸����ƴ�
* @website      www.wisioe.com
* @forum        www.w5500.cn
* @Support      QQ:2571856470��Mob:18998931307
* @Email        support@wisioe.com
******************************************************************************
*/
#include <stdio.h>
#include "tcp_server.h"
#include "socket.h"
#include "w5500_control.h"
#include "w5500_conf.h"

uint8_t tcp_server_buff[DATA_BUF_SIZE];  

void do_tcp_server(void)
{  
  uint16 len=0;  
  switch(getSn_SR(SOCK_TCPS))                             // ��ȡsocket��״̬
  {
    case SOCK_CLOSED:                                     // socket���ڹر�״̬
      socket(SOCK_TCPS ,Sn_MR_TCP,local_port,Sn_MR_ND);   // ��socket
      break;     
    
    case SOCK_INIT:                                       // socket�ѳ�ʼ��״̬
      listen(SOCK_TCPS);                                  // socket��������
      break;
    
    case SOCK_ESTABLISHED:                                // socket�������ӽ���״̬
    
      if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
      {
        setSn_IR(SOCK_TCPS, Sn_IR_CON);                   // ��������жϱ�־λ
      }
      len=getSn_RX_RSR1(SOCK_TCPS);                        // ����lenΪ�ѽ������ݵĳ���
      if(len>0)
      {
        recv(SOCK_TCPS,tcp_server_buff,len);                         // ��������Client������
        tcp_server_buff[len]=0x00;                                   // ����ַ���������
        printf("%s\r\n",tcp_server_buff);
        send(SOCK_TCPS,tcp_server_buff,len);                         // ��Client��������
      }
      break;
    
    case SOCK_CLOSE_WAIT:                                 // socket���ڵȴ��ر�״̬
			disconnect(SOCK_TCPS);                              // �Ͽ���ǰTCP����
      close(SOCK_TCPS);                                   // �رյ�ǰ��ʹ��socket
      break;
  }
}
