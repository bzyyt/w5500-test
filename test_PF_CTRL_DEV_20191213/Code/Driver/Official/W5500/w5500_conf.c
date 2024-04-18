/**
******************************************************************************
* @file         w5500_conf.c
* @version      V1.0
* @date         2020-06-05
* @brief        配置MCU，移植w5500程序需要修改的文件，配置w5500的MAC和IP地址
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
#include <stdio.h>
#include <string.h>
#include "w5500_conf.h"
#include "utility.h"
#include "w5500_control.h"
#include "socket.h"
#include "SPIAdapter.h"
#include "DelayAdapter.h"
#include "UartAdapter.h"

CONFIG_MSG ConfigMsg, RecvMsg; // 配置结构体

uint16 rxlen = 0;
uint8 rxbuf[1024];

/*MAC地址首字节必须为偶数
  如果多块w5500网络适配板在同一现场工作，请使用不同的MAC地址
*/
uint8 mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x11, 0x11};

/*定义默认IP信息*/
uint8 local_ip[4] = {192, 168, 1, 150};     // 定义w5500默认IP地址
uint8 subnet[4] = {255, 255, 255, 0};       // 定义w5500默认子网掩码
uint8 gateway[4] = {192, 168, 1, 1};        // 定义w5500默认网关
uint8 dns_server[4] = {114, 114, 114, 114}; // 定义w5500默认DNS

uint16 local_port = 8080; // 定义本地端口

/*定义远端IP信息*/
// uint8 remote_ip[4] = {169, 254, 253, 117}; // 远端IP地址
uint8 remote_ip[4] = {192, 168, 1, 30}; // 远端IP地址
uint16 remote_port = 8080;               // 远端端口号

/*IP配置方法选择，请自行选择*/
uint8 ip_from = IP_FROM_DEFINE;

uint32 ms = 0; // 毫秒计数
// vu8    ntptimer = 0;                                        // NPT秒计数

/**
 *@brief  初始化W5500的复位与中断管脚
 *@param  无
 *@return 无
 */
void reset_break_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); // 开启GPIOH时钟
  /* PH_01 -> RST */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOH, &GPIO_InitStructure);
  /* PH_02 -> INT */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(GPIOH, &GPIO_InitStructure);
}

/**
 *@brief  配置W5500的IP地址
 *@param  无
 *@return 无
 */
void set_w5500_netinfo(void)
{
#ifdef _HTTP_SERVER_H_
  uint8 temp;
#endif
  // 复制定义的配置信息到配置结构体
  memcpy(ConfigMsg.mac, mac, 6);
  memcpy(ConfigMsg.lip, local_ip, 4);
  memcpy(ConfigMsg.sub, subnet, 4);
  memcpy(ConfigMsg.gw, gateway, 4);
  memcpy(ConfigMsg.dns, dns_server, 4);
#ifdef _HTTP_SERVER_H_
  /*仅在使用HTTP Server例程时生效，使用请检查"http_server.h"头文件的引用*/
  ReadFlashNBtye(0, &temp, 1);
  if (temp != 255)
    ip_from = IP_FROM_FLASH;
  else
    ip_from = IP_FROM_DEFINE;
#endif
  switch (ip_from)
  {
  case IP_FROM_DEFINE:
    // printf(" 使用定义的IP信息配置W5500：\r\n");
    SendUartMsg(FPGAUart1, 0, 16, "IP_FROM_DEFINE\r\n");
    break;
#ifdef _HTTP_SERVER_H_
  case IP_FROM_FLASH:
    printf(" 使用Flash内IP信息配置W5500：\r\n");
    ReadFlashNBtye(0, ConfigMsg.lip, 12);
    break;
#endif
  }
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  getSIPR(local_ip);
  // printf(" W5500 IP地址   : %d.%d.%d.%d\r\n", local_ip[0], local_ip[1], local_ip[2], local_ip[3]);
  getSUBR(subnet);
  // printf(" W5500 子网掩码 : %d.%d.%d.%d\r\n", subnet[0], subnet[1], subnet[2], subnet[3]);
  getGAR(gateway);
  // printf(" W5500 网关     : %d.%d.%d.%d\r\n", gateway[0], gateway[1], gateway[2], gateway[3]);
}

/**
 *@brief  配置W5500的MAC地址
 *@param  无
 *@return 无
 */
void set_w5500_mac(void)
{
  // 以下配置信息，根据需要选用
  ConfigMsg.sw_ver[0] = FW_VER_HIGH;
  ConfigMsg.sw_ver[1] = FW_VER_LOW;

  memcpy(ConfigMsg.mac, mac, 6);
  setSHAR(ConfigMsg.mac);
  getSHAR(mac);
  // printf(" W5500 MAC地址  : %02x.%02x.%02x.%02x.%02x.%02x\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/**
 *@brief  W5500复位设置函数
 *@param  无
 *@return 无
 */
void reset_w5500(void)
{
  GPIO_ResetBits(GPIOH, GPIO_Pin_1);
  // HAL_GPIO_WritePin(GPIOH,GPIO_PIN_1,GPIO_PIN_RESET);	//RST PH01置0
  Delay_ms(500);
  GPIO_SetBits(GPIOH, GPIO_Pin_1);
  // HAL_GPIO_WritePin(GPIOH,GPIO_PIN_1,GPIO_PIN_SET);	//RST PH01置1
  Delay_ms(1500);
}

/**
 *@brief		设置W5500的片选端口SCSn为低
 *@param		无
 *@return	无
 */
void iinchip_csoff(void)
{
  // cs_low();
  Spi_CS(SPI_3, Bit_RESET);
}

/**
 *@brief		设置W5500的片选端口SCSn为高
 *@param		无
 *@return	无
 */
void iinchip_cson(void)
{
  //  cs_high();
  Spi_CS(SPI_3, Bit_SET);
}

/**
 *@brief		STM32 SPI1读写8位数据
 *@param		dat：写入的8位数据
 *@return	无
 */
uint8 IINCHIP_SpiSendData(uint8 dat)
{
  // return(spi_read_send_byte(dat));
  return (Spi_ReadWriteData(SPI_3, dat));
}

/**
 *@brief		写入一个8位数据到W5500
 *@param		addrbsb: 写入数据的地址
 *@param   data：写入的8位数据
 *@return	无
 */
void IINCHIP_WRITE(uint32 addrbsb, uint8 data)
{
  iinchip_csoff();
  IINCHIP_SpiSendData((addrbsb & 0x00FF0000) >> 16);
  IINCHIP_SpiSendData((addrbsb & 0x0000FF00) >> 8);
  IINCHIP_SpiSendData((addrbsb & 0x000000F8) + 4);
  IINCHIP_SpiSendData(data);
  iinchip_cson();
}

/**
 *@brief		从W5500读出一个8位数据
 *@param		addrbsb: 写入数据的地址
 *@param   data：从写入的地址处读取到的8位数据
 *@return	无
 */
uint8 IINCHIP_READ(uint32 addrbsb)
{
  uint8 data = 0;
  iinchip_csoff();
  IINCHIP_SpiSendData((addrbsb & 0x00FF0000) >> 16);
  IINCHIP_SpiSendData((addrbsb & 0x0000FF00) >> 8);
  IINCHIP_SpiSendData((addrbsb & 0x000000F8));
  data = IINCHIP_SpiSendData(0x00);
  iinchip_cson();
  return data;
}

/**
 *@brief		向W5500写入len字节数据
 *@param		addrbsb: 写入数据的地址
 *@param   buf：写入字符串
 *@param   len：字符串长度
 *@return	len：返回字符串长度
 */
uint16 wiz_write_buf(uint32 addrbsb, uint8 *buf, uint16 len)
{
  uint16 idx = 0;
  // if (len == 0)
    // printf(" Unexpected2 length 0\r\n");
  iinchip_csoff();
  IINCHIP_SpiSendData((addrbsb & 0x00FF0000) >> 16);
  IINCHIP_SpiSendData((addrbsb & 0x0000FF00) >> 8);
  IINCHIP_SpiSendData((addrbsb & 0x000000F8) + 4);
  for (idx = 0; idx < len; idx++)
  {
    IINCHIP_SpiSendData(buf[idx]);
  }
  iinchip_cson();
  return len;
}

/**
 *@brief		从W5500读出len字节数据
 *@param		addrbsb: 读取数据的地址
 *@param 	buf：存放读取数据
 *@param		len：字符串长度
 *@return	len：返回字符串长度
 */
uint16 wiz_read_buf(uint32 addrbsb, uint8 *buf, uint16 len)
{
  uint16 idx = 0;
  // if (len == 0)
  // {
  //   printf(" Unexpected2 length 0\r\n");
  // }
  iinchip_csoff();
  IINCHIP_SpiSendData((addrbsb & 0x00FF0000) >> 16);
  IINCHIP_SpiSendData((addrbsb & 0x0000FF00) >> 8);
  IINCHIP_SpiSendData((addrbsb & 0x000000F8));
  for (idx = 0; idx < len; idx++)
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();
  return len;
}

/**
 *@brief  STM32系统软复位函数
 *@param  无
 *@return 无
 */
typedef __IO uint32_t vu32;
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  // printf(" 系统重启中……\r\n");
  JumpAddress = *(vu32 *)(0x00000004);
  Jump_To_Application = (pFunction)JumpAddress;
  Jump_To_Application();
}

/**
 *@brief  检测物理层连接
 *@param  无
 *@return 无
 */
void PHY_check(void)
{
  uint8 PHY_connect = 0;
  PHY_connect = 0x01 & getPHYStatus();
  if (PHY_connect == 0)
  {
    // printf(" \r\n 请检查网线是否连接?\r\n");
    SendUartMsg(FPGAUart1, 0, 22, "cable not connected\r\n");
    PHY_connect = 0;
    do
    {
      close(PHY_connect++);
    } while (PHY_connect < 4);
    PHY_connect = 0;
    while (PHY_connect == 0)
    {
      PHY_connect = 0x01 & getPHYStatus();
      // printf(" .");
      Delay_ms(500);
    }
    // printf(" \r\n");
  }
}

/**
 *@brief  读取PHY寄存器数据
 *@param  无
 *@return PHYSR寄存器数值
 */
uint8 getPHYStatus(void)
{
  return IINCHIP_READ(PHYCFGR);
}

/**
 *@brief  配置KPALVIR寄存器
 *@param  i：配置的socket号码
 *@param  val：设置的时间(5s单位)
 *@return 无
 */
void setKPALVTR(SOCKET i, uint8 val)
{
  IINCHIP_WRITE(Sn_KPALVTR(i), val);
}

/**
 * @brief w5500初始化函数
 * @param 无
 * @return 无
 */
void w5500Init()
{
  reset_w5500();                   // w5500硬件复位
  PHY_check();                     // 网线检测程序
  set_w5500_mac();                 // 设置w5500MAC地址
  set_w5500_netinfo();             // 设置w5500网络参数
  socket_buf_init(txsize, rxsize); // 初始化8个Socket的发送接收缓存大小
}
