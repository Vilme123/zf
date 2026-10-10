/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】北京龙邱智能科技K66核心板
【编    写】chiusir
【E-mail  】chiusir@163.com
【软件版本】V1.1 版权所有，单位使用请先联系授权
【最后更新】2020年8月10日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://longqiu.taobao.com
------------------------------------------------
【dev.env.】IAR/KEIL
【Target 】 K60
【Crystal】 50.000Mhz
【SYS PLL】 180MHz
________________________________________________________________

QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
/*-------------------------------------------------------------------------------------------------------------------
 * 适配说明(已适配成都逐飞科技 STC32G 开源库)：
 *   1. SPI 通信由 GPIO 模拟 SPI 改为逐飞库硬件 SPI，即调用 zf_driver_spi.c 中的
 *      spi_init() 与 spi_transfer_8bit()，对应原 SPI_SoftInit() / SPI_SoftReadWriteNbyte()
 *   2. 数据类型由 uint8_t / uint16_t / int16_t 改为逐飞库的 uint8 / uint16 / int16
 *   3. 头文件由龙邱的 include.h 改为逐飞库的 zf_common_headfile.h
 *   4. 延时函数 delayms() 改为逐飞库的 system_delay_ms()
 *   5. 龙邱 OLED / TFT1.8 显示代码已移除，调试信息统一用 printf 输出
 *   6. 旧函数名保留为宏定义(见文件末尾)，原有调用代码无需修改
 *-----------------------------------------------------------------------------------------------------------------*/
#ifndef __LQ_ICM42605_H
#define __LQ_ICM42605_H

#include "zf_common_typedef.h"
#include "zf_driver_spi.h"

//====================================================逐飞库 硬件 SPI 配置====================================================
// ICM42605 与单片机接线：SCLK->P25  MOSI(SDI)->P23  MISO(SDO)->P24  CS->P26  VCC->3.3V  GND->GND
#define ICM42605_SPI                        (SPI_0)                             // 使用的 SPI 模块 参照 zf_driver_spi.h 内 spi_index_enum
#define ICM42605_SPI_MODE                   (SPI_MODE3)                         // SPI 模式 ICM42605 支持模式0与模式3 此设置与原模拟SPI时序一致
#define ICM42605_SPI_SPEED                  (1 * 1000 * 1000)                   // 期望的 SPI 速率 实际速率会被适配成 系统时钟/4 8 16 2 中最接近的一个
#define ICM42605_SPI_SCK_PIN                (SPI0_CH2_SCLK_P25)                 // 硬件 SPI SCK  引脚 参照 zf_driver_spi.h 内 spi_pin_enum
#define ICM42605_SPI_MOSI_PIN               (SPI0_CH2_MOSI_P23)                 // 硬件 SPI MOSI 引脚
#define ICM42605_SPI_MISO_PIN               (SPI0_CH2_MISO_P24)                 // 硬件 SPI MISO 引脚
#define ICM42605_SPI_CS_PIN                 (IO_P26)                            // CS 片选引脚 参照 zf_driver_gpio.h 内 gpio_pin_enum
#define ICM42605_SPI_CS(x)                  (P26 = x)                           // CS 片选控制 低电平有效
//====================================================逐飞库 硬件 SPI 配置====================================================

#define fifo_packet_structure_size   16
#define fifo_packet_num              32
#define fifo_packet_size             fifo_packet_structure_size*fifo_packet_num

#define who_am_i                     0x75
#define ICM42605_ID	             0x42	

#define ICM_TEMP_OUTH_REG		0X1D	//温度值高八位寄存器
#define ICM_TEMP_OUTL_REG		0X1E	//温度值低8位寄存器

#define ICM_ACCEL_XOUTH_REG		0X1F	//加速度值,X轴高8位寄存器
#define ICM_ACCEL_XOUTL_REG		0X20	//加速度值,X轴低8位寄存器
#define ICM_ACCEL_YOUTH_REG		0X21	//加速度值,Y轴高8位寄存器
#define ICM_ACCEL_YOUTL_REG		0X22	//加速度值,Y轴低8位寄存器
#define ICM_ACCEL_ZOUTH_REG		0X23	//加速度值,Z轴高8位寄存器
#define ICM_ACCEL_ZOUTL_REG		0X24	//加速度值,Z轴低8位寄存器

#define ICM_GYRO_XOUTH_REG		0X25	//陀螺仪值,X轴高8位寄存器
#define ICM_GYRO_XOUTL_REG		0X26	//陀螺仪值,X轴低8位寄存器
#define ICM_GYRO_YOUTH_REG		0X27	//陀螺仪值,Y轴高8位寄存器
#define ICM_GYRO_YOUTL_REG		0X28	//陀螺仪值,Y轴低8位寄存器
#define ICM_GYRO_ZOUTH_REG		0X29	//陀螺仪值,Z轴高8位寄存器
#define ICM_GYRO_ZOUTL_REG		0X2A	//陀螺仪值,Z轴低8位寄存器


#define reg_bank_sel                 0x76
#define device_config_reg            0x11
#define bit_spi_mode                 0x10
#define bit_soft_reset_chip_config   0x01

#define intf_config4                 0x7A
#define pwr_mgmt0_reg                0x4E
//#define bit_temp_dis               0x20
#define bit_idle                     0x10
#define bit_gyro_mode_mask           ((0x03)<<2)
#define bit_gyro_mode_off            ((0x00)<<2)
#define bit_gyro_mode_standby        ((0x01)<<2)
//#define bit_gyro_mode_lp           ((0x02)<<2)
#define bit_gyro_mode_ln             ((0x03)<<2)
#define bit_accel_mode_mask          ((0x03)<<0)
#define bit_accel_mode_off           0x00
#define bit_accel_mode_lp            0x02
#define bit_accel_mode_ln            0x03

#define gyro_config0_reg             0x4F
#define bit_gyro_ui_fs_sel_shift     5
#define bit_gyro_ui_fs_sel_2000dps   ((0x00)<<bit_gyro_ui_fs_sel_shift)
#define bit_gyro_ui_fs_sel_1000dps   ((0x01)<<bit_gyro_ui_fs_sel_shift)
#define bit_gyro_ui_fs_sel_mask      ((0x07)<<bit_gyro_ui_fs_sel_shift)
#define bit_gyro_odr_100hz           ((0x08)<<0)
#define bit_gyro_odr_50hz            ((0x09)<<0)
#define bit_gyro_odr_nonflame_mask   ((0x0F)<<0)
	
#define accel_config0_reg            0x50
#define bit_accel_ui_fs_sel_shift    5
#define bit_accel_ui_fs_sel_8g       ((0x01)<<bit_accel_ui_fs_sel_shift)
#define bit_accel_ui_fs_sel_mask     ((0x07)<<bit_accel_ui_fs_sel_shift)
#define bit_accel_odr_100hz          ((0x08)<<0)
#define bit_accel_odr_50hz           ((0x09)<<0)
#define bit_accel_odr_nonflame_mask  ((0x0F)<<0)
	
#define int_source0_reg               0x65	
#define bit_int_ui_fsync_int1_en      0x40
#define bit_int_pll_rdy_int1_en       0x20
#define bit_int_reset_done_int1_en    0x10
#define bit_int_ui_drdy_int1_en       0x08
#define bit_int_fifo_ths_int1_en      0x04//FIFO threshold interrupt
#define bit_int_fifo_full_int1_en     0x02
#define bit_int_ui_agc_rdy_int1_en    0x01

#define sensor_selftest_reg           0x6B
#define bit_accel_st_result           0x08
#define bit_gyro_st_result            0x04
#define bit_accel_st_status           0x02
#define bit_gyro_st_status            0x01
	
#define int_config_reg                0x14
#define bit_int2_mode                 0x20
#define bit_int2_drive_circuit        0x10
#define bit_int2_polarity             0x08
#define bit_int1_mode                 0x04
#define bit_int1_drive_circuit        0x02
#define bit_int1_polarity             0x01
	
#define fifo_config_reg               0x16
#define bit_fifo_mode_ctrl_mask       ((0x03)<<6)
#define bit_fifo_mode_ctrl_bypass     ((0x00)<<6)
#define bit_fifo_mode_ctrl_stream     ((0x01)<<6)
#define bit_fifo_mode_ctrl_snapshot   ((0x02)<<6)
	
#define tmst_config_reg                0x54
#define bit_fifo_ram_iso_ena           0x40
#define bit_en_dreg_fifo_d2a           0x20
#define bit_tmst_to_regs_en            0x10
#define bit_tmst_resol                 0x08
#define bit_tmst_delta_en              0x04
#define bit_tmst_fsync_en              0x02
#define bit_tmst_en                    0x01
	
#define fifo_config2_reg               0x60	
#define fifo_config3_reg               0x61

#define fsync_config_reg               0x62				
#define bit_fsync_ui_sel_mask          ((0x07)<<4)
#define bit_fsync_ui_sel_tag_temp      ((0x01)<<4)
#define bit_fsync_ui_flag_clear_sel    0x02

#define fifo_config1_reg               0x5F
#define bit_fifo_resume_partial        0x40
#define bit_fifo_wm_gt_th              0x20
#define bit_fifo_hires_en              0x10
#define bit_fifo_tmst_fsync_en         0x08
#define bit_fifo_temp_en               0x04
#define bit_fifo_gyro_en               0x02
#define bit_fifo_accel_en              0x01
	
#define int_config0_reg                0x63
#define int_config1_reg                0x64
#define bit_int_asy_rst_disable        0x10

#define fifo_byte_count_h_res          0x2E
#define fifo_byte_count_l_res          0x2F

#define fifo_accel_en                  0x40
#define fifo_gyro_en                   0x20

#define fifo_data_port                 0x30


#define gyr_ssl                        32.8f
#define acc_ssl                        0.244f


typedef struct
{
	int16 Sample_accdata[3*fifo_packet_num];
	int16 Sample_gyrdata[3*fifo_packet_num];	
}Sample_data_type_t;


//====================================================函数声明====================================================
void    icm42605_spi_init           (void);                                     // 逐飞库硬件 SPI 初始化(原 SPI_SoftInit)
void    icm42605_spi_read_write     (uint8 *lqbuff, uint16 len);                // 逐飞库硬件 SPI 全双工收发(原 SPI_SoftReadWriteNbyte)

void    ICM_Read_Len                (uint8 reg, uint8 len, uint8 *buf);
uint8   icm42605_read_reg           (uint8 reg);
uint8   icm42605_write_reg          (uint8 reg, uint8 value);
uint8   icm42605_read_regs          (uint8 reg, uint8 *buf, uint16 len);
void    ICM_Get_Gyroscope           (int16 *gx, int16 *gy, int16 *gz);
void    ICM_Get_Accelerometer       (int16 *ax, int16 *ay, int16 *az);
void    ICM_Get_Raw_data            (int16 *ax, int16 *ay, int16 *az, int16 *gx, int16 *gy, int16 *gz);
uint8   icm42605_init               (void);
void    icm42605_read_fifo          (Sample_data_type_t *fifo_data, uint16 len);
void    icm42605_stop               (void);
void    Test_ICM42605FIFO           (void);

//====================================================兼容原来的函数名====================================================
// 原来的模拟 SPI 函数名，现在指向逐飞库硬件 SPI 的实现，旧代码不用修改也能编译
#define SPI_SoftInit()                          icm42605_spi_init()
#define SPI_SoftReadWriteNbyte(lqbuff, len)     icm42605_spi_read_write((lqbuff), (len))

#endif
