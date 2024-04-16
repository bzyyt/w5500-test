/**
******************************************************************************
* @file         tcp_client.c
* @version      V1.0
* @date         2020-06-05
* @brief        tcp clinet���ݻػ�����
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
#include "tcp_client.h"
#include "socket.h"
#include "w5500_control.h"
#include "w5500_conf.h"

uint8_t tcp_client_buff[DATA_BUF_SIZE];	

void do_tcp_client(void)
{
  uint16 len=0;	
  uint8 status = getSn_SR(SOCK_TCPC);
  switch(status)                           // ��ȡsocket��״̬
  {
    case SOCK_CLOSED:                                   // socket���ڹر�״̬
      socket(SOCK_TCPC,Sn_MR_TCP,local_port,Sn_MR_ND);
      break;

    case SOCK_INIT:                                     // socket���ڳ�ʼ��״̬
      connect(SOCK_TCPC,remote_ip,remote_port);         // socket���ӷ����� 
      break;

    case SOCK_ESTABLISHED:                              // socket�������ӽ���״̬
      if(getSn_IR(SOCK_TCPC) & Sn_IR_CON)
      {
        setSn_IR(SOCK_TCPC, Sn_IR_CON);                 // ��������жϱ�־λ
      }

      len=getSn_RX_RSR1(SOCK_TCPC);                      // ����lenΪ�ѽ������ݵĳ���
      if(len>0)
      {
        recv(SOCK_TCPC,tcp_client_buff,len);                       // ��������Server������
        tcp_client_buff[len]=0x00;                                 // ����ַ���������
        // printf("%s\r\n",tcp_client_buff);
        send(SOCK_TCPC,tcp_client_buff,len);                       // ��Server��������
      }		  
      break;

    case SOCK_CLOSE_WAIT:                               // socket���ڵȴ��ر�״̬
      disconnect(SOCK_TCPC);                            // �Ͽ���ǰTCP����
      close(SOCK_TCPC);                                 // �رյ�ǰ��ʹ��socket
      break;
  }
}
