/**
******************************************************************************
* @file         types.h
* @version      V1.0
* @date         2020-06-05
* @brief        ���������ض���
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
#ifndef _TYPE_H_
#define _TYPE_H_

#define MAX_SOCK_NUM 8 // Maxmium number of socket

typedef unsigned char uint8_t;

typedef char int8;

typedef volatile char vint8;

typedef unsigned char uint8;

typedef volatile unsigned char vuint8;

typedef int int16;

typedef unsigned short uint16;

typedef long int32;

typedef unsigned long uint32;

typedef uint8  u_char;    /**< 8-bit value */
typedef uint8  SOCKET;
typedef uint16 u_short;   /**< 16-bit value */
typedef uint16 u_int;     /**< 16-bit value */
typedef uint32 u_long;    /**< 32-bit value */

typedef union _un_l2cval 
{
 u_long lVal;
 u_char cVal[4];
}un_l2cval;

typedef union _un_i2cval 
{
 u_int iVal;
 u_char cVal[2];
}un_i2cval;

#endif /* _TYPE_H_ */
