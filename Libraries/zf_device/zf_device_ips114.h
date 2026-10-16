/*********************************************************************************************************************
* STC32G Opensourec Library 即（STC32G 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是STC 开源库的一部分
*
* STC32G 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          MDK FOR C251
* 适用平台          STC32G
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者           备注
* 2024-08-01        大W            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* 接线定义：
*                   ------------------------------------
*                   模块管脚             单片机管脚
*                   SCL/SCK              查看 zf_device_ips114.h 中 IPS114_SCL_PIN 宏定义
*                   SDA/SDI/MOSI         查看 zf_device_ips114.h 中 IPS114_SDA_PIN 宏定义
*                   SDO/MISO             【不需要连接】 ips114 只写不读 驱动初始化时 MISO 传的是 SPI_MISO_NULL
*                   RST                  查看 zf_device_ips114.h 中 IPS114_RST_PIN 宏定义
*                   DC                   查看 zf_device_ips114.h 中 IPS114_DC_PIN  宏定义
*                   CS                   查看 zf_device_ips114.h 中 IPS114_CS_PIN  宏定义
*                   BLK                  查看 zf_device_ips114.h 中 IPS114_BLK_PIN 宏定义
*                   VCC                 3.3V电源
*                   GND                 电源地
*                   最大分辨率 135 * 240
*                   ------------------------------------
*                   本工程(1.14 寸 IPS 135*240 ST7789V)实际接线：
*                   模块管脚        单片机管脚      说明
*                   SCK             P2.5            硬件 SPI_2 第 2 通道时钟 SPI2_CH2_SCLK_P25
*                   SDI/MOSI        P2.3            硬件 SPI_2 第 2 通道主出从入 SPI2_CH2_MOSI_P23
*                   CS              P2.2            软件控制 低电平有效
*                   DC              P2.1            命令/数据选择 0=命令 1=数据
*                   RST             P2.0            复位 低电平有效
*                   BLK             P2.7            背光 高电平点亮
*                   SDO/MISO        --              悬空不接 本屏不需要读回数据
*                   VCC             3.3V
*                   GND             GND
*                   ------------------------------------
*                   注意：STC32G 的硬件 SPI 引脚组是固定的 SPI2_CH2 只能使用
*                         SCK=P2.5 / MOSI=P2.3 / MISO=P2.4 三者不能拆开单独换脚
*                         因此 P2.1 不能作为本组 SPI 的 MISO 使用
*                         P2.1 在本工程中用作 IPS 屏的 DC(命令/数据)引脚
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_ips114_h_
#define _zf_device_ips114_h_

#include "zf_common_typedef.h"
#include "zf_device_type.h"

#define IPS114_USE_INTERFACE             (HARDWARE_SPI)                                     // 默认使用硬件 SPI 方式驱动 建议使用硬件 SPI 方式驱动
#if (IPS114_USE_INTERFACE==SOFT_SPI)                                                         // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 SPI 驱动====================================================
	// 暂不支持
//====================================================软件 SPI 驱动====================================================
#elif (IPS114_USE_INTERFACE==HARDWARE_SPI)
//====================================================硬件 SPI 驱动====================================================
	#define IPS114_SPI_SPEED                ((uint32)20 * 1000 * 1000U)             // 硬件 SPI 速率 这里设置为系统时钟二分频
	#define IPS114_SPI                      (SPI_2)                                 // 硬件 SPI 号
	#define IPS114_SCL_PIN                  (SPI2_CH2_SCLK_P25)                     // 硬件 SPI SCK 引脚
	#define IPS114_SDA_PIN                  (SPI2_CH2_MOSI_P23)                     // 硬件 SPI MOSI 引脚
//====================================================硬件 SPI 驱动====================================================
#endif

//====================================================IPS 屏引脚分配 与实物接线一一对应====================================================
//  屏 SCK      -> P2.5   硬件 SPI_2 第 2 通道时钟
//  屏 SDI/MOSI -> P2.3   硬件 SPI_2 第 2 通道主出从入
//  屏 SDO/MISO -> 悬空   本屏只写不读 ips114_init() 里 spi_init() 传入的 MISO 为 SPI_MISO_NULL
//  屏 CS       -> P2.2   软件控制 低电平有效
//  屏 DC       -> P2.1   命令/数据选择 0=命令 1=数据
//  屏 RST      -> P2.0   复位 低电平有效
//  屏 BLK      -> P2.7   背光 高电平点亮
//--------------------------------------------------------------------------------------------------------------------------------------
#define IPS114_RST_PIN                  (P20 )                                   // 液晶复位引脚定义
#define IPS114_DC_PIN                   (P21 )                                   // 液晶命令位引脚定义 本工程占用 P2.1
#define IPS114_CS_PIN                   (P22 )                                   // CS 片选引脚
#define IPS114_BLK_PIN                  (P27 )                                   // 液晶背光引脚定义

#define IPS114_DEFAULT_DISPLAY_DIR      (IPS114_CROSSWISE_180)                  // 默认的显示方向
#define IPS114_DEFAULT_PENCOLOR         (RGB565_RED)                            // 默认的画笔颜色
#define IPS114_DEFAULT_BGCOLOR          (RGB565_WHITE)                          // 默认的背景颜色
//#define IPS114_DEFAULT_DISPLAY_FONT     (IPS114_8X16_FONT)                      // 默认的字体模式

#define IPS114_DC(x)                    ( IPS114_DC_PIN  = x )
#define IPS114_RST(x)                   ( IPS114_RST_PIN = x )
#define IPS114_CS(x)                    ( IPS114_CS_PIN  = x )
#define IPS114_BLK(x)                   ( IPS114_BLK_PIN = x )

typedef enum
{
    IPS114_PORTAIT                      = 0,                                    // 竖屏模式
    IPS114_PORTAIT_180                  = 1,                                    // 竖屏模式  旋转180
    IPS114_CROSSWISE                    = 2,                                    // 横屏模式
    IPS114_CROSSWISE_180                = 3,                                    // 横屏模式  旋转180
}ips114_dir_enum;



void    ips114_init                     (void);
void    ips114_clear                    (uint16 color);
void    ips114_set_dir                  (ips114_dir_enum dir);
void    ips114_set_color                (uint16 pen, uint16 bgcolor);
void    ips114_draw_point               (uint16 x, uint16 y, uint16 color);
															 
void    ips114_show_char                (uint16 x, uint16 y, const char dat);
void    ips114_show_string              (uint16 x, uint16 y, const char dat[]);
void    ips114_show_int8                (uint16 x, uint16 y, int8 dat);
void    ips114_show_uint8               (uint16 x, uint16 y, uint8 dat);
void    ips114_show_int16               (uint16 x, uint16 y, int16 dat);
void    ips114_show_uint16              (uint16 x, uint16 y, uint16 dat);
void    ips114_show_int32               (uint16 x, uint16 y, int32 dat, uint8 num);
void    ips114_show_float               (uint16 x, uint16 y, double dat, uint8 num, uint8 pointnum);

void    ips114_show_wave                (uint16 x, uint16 y, uint8 *p, uint16 width, uint16 value_max, uint16 dis_width, uint16 dis_value_max);

void    ips114_init                     (void);

#endif
