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
 *   1. 原来用 GPIO 模拟的 SPI 通信(SPI_SoftInit / SPI_SoftReadWriteNbyte)改为调用逐飞库硬件 SPI
 *      即 zf_driver_spi.c 中的 spi_init() 与 spi_transfer_8bit()
 *   2. 头文件由龙邱的 include.h 改为逐飞库的 zf_common_headfile.h
 *   3. 延时函数 delayms() 改为逐飞库的 system_delay_ms()
 *   4. 龙邱 OLED / TFT1.8 显示相关代码已移除，调试信息统一用 printf 输出
 *   5. ICM42605 与单片机的接线见 LQ_ICM42605.h 内 ICM42605_SPI_xxx 宏定义
 *   6. 其余寄存器操作、数据换算逻辑与原工程完全一致，未做改动
 *-----------------------------------------------------------------------------------------------------------------*/
#include "zf_common_headfile.h"
#include "LQ_ICM42605.h"

/*************************************************************************
*  函数名称：void icm42605_spi_init(void)
*  功能说明：逐飞库硬件 SPI 初始化(原为模拟SPI口初始化 SPI_SoftInit)
*  参数说明：无
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：需要在 clock_init() 之后调用
*************************************************************************/
void icm42605_spi_init(void)
{
  gpio_init(ICM42605_SPI_CS_PIN, GPO, 1, GPO_PUSH_PULL);        // CS 片选引脚 推挽输出 初始拉高(空闲状态)

  spi_init(ICM42605_SPI,                                        // SPI 模块号
           ICM42605_SPI_MODE,                                   // SPI 模式
           ICM42605_SPI_SPEED,                                  // SPI 速率
           ICM42605_SPI_SCK_PIN,                                // SCK  引脚
           ICM42605_SPI_MOSI_PIN,                               // MOSI 引脚
           ICM42605_SPI_MISO_PIN,                               // MISO 引脚
           SPI_CS_NULL);                                        // CS 由软件控制 所以这里填 SPI_CS_NULL
}
/*************************************************************************
*  函数名称：void icm42605_spi_read_write(uint8 *lqbuff, uint16 len)
*  功能说明：SPI读写数据及长度(原为模拟SPI收发 SPI_SoftReadWriteNbyte)
*  参数说明：uint8 *lqbuff数据指针,uint16 len长度
*  函数返回：
*  修改时间：2020年8月10日
*  备    注：发送的同时接收，接收到的数据覆盖回原缓冲区，与原模拟SPI行为一致
*************************************************************************/
void icm42605_spi_read_write(uint8 *lqbuff, uint16 len)
{
  ICM42605_SPI_CS(0);                                           // 片选拉低 选中 ICM42605
  spi_transfer_8bit(ICM42605_SPI, lqbuff, lqbuff, len);         // 逐飞库硬件SPI收发 收发共用同一个缓冲区
  ICM42605_SPI_CS(1);                                           // 片选拉高 释放 ICM42605
}
/*************************************************************************
*  函数名称：void ICM_Read_Len(uint8 reg,uint8 len,uint8 *buf)
*  功能说明：uint8 reg起始寄存器,uint8 *buf数据指针,uint16 len长度
*  参数说明：无
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：读回的数据存放在 buf[1] 开始的 len 个字节中，buf 至少要 len+1 个字节
*************************************************************************/
void ICM_Read_Len(uint8 reg,uint8 len,uint8 *buf)
{   
  buf[0] = reg | 0x80;
  
  icm42605_spi_read_write(buf, len + 1);  
}
/*************************************************************************
*  函数名称：uint8 icm42605_read_reg(uint8 reg)
*  功能说明：读取数据
*  参数说明：uint8 reg寄存器地址
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：也可以直接使用逐飞库的 spi_read_8bit_register()，但那样取回来的数据
*            是函数返回值而不是 buf[1]，为保持原工程写法此处仍用收发缓冲区的方式
*************************************************************************/
uint8 icm42605_read_reg(uint8 reg)
{
  uint8 lqbuff[2];
  lqbuff[0] = reg | 0x80;          //先发送寄存器
  
  icm42605_spi_read_write(lqbuff, 2);
  
  
  return lqbuff[1];
}
/*************************************************************************
*  函数名称：uint8 icm42605_write_reg(uint8 reg,uint8 value)
*  功能说明：向寄存器写数据
*  参数说明：uint8 reg寄存器,uint8 value数据
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：也可以直接使用逐飞库的 spi_write_8bit_register()
*************************************************************************/
uint8 icm42605_write_reg(uint8 reg,uint8 value)
{
  uint8 lqbuff[2];
  
  lqbuff[0] = reg;          //先发送寄存器
  lqbuff[1] = value;        //再发送数据
  
  icm42605_spi_read_write(lqbuff, 2);//发送buff里数据，并采集到 buff里
  return 0;
}
/*************************************************************************
*  函数名称：uint8 icm42605_read_regs(uint8 reg,uint8 *buf,uint16 len)
*  功能说明：连续读取数据
*  参数说明：uint8 reg起始寄存器,uint8 *buf数据指针,uint16 len长度
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：读回的数据存放在 buf[1] 开始的 len 个字节中，buf 至少要 len+1 个字节
*************************************************************************/
uint8 icm42605_read_regs(uint8 reg,uint8 *buf,uint16 len)
{   
  buf[0] = reg | 0x80;
  /* 写入要读的寄存器地址 */
  icm42605_spi_read_write(buf, len + 1);
  return 0;
}
/*************************************************************************
*  函数名称：void ICM_Get_Gyroscope(int16 *gx,int16 *gy,int16 *gz)
*  功能说明：
*  参数说明：无
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：   
*************************************************************************/
void ICM_Get_Gyroscope(int16 *gx,int16 *gy,int16 *gz)
{
  uint8 buf[7]; 
  ICM_Read_Len(ICM_GYRO_XOUTH_REG,6,buf);
  
  *gx=((uint16)buf[1]<<8)|buf[2];  
  *gy=((uint16)buf[3]<<8)|buf[4];  
  *gz=((uint16)buf[5]<<8)|buf[6];
  
}
/*************************************************************************
*  函数名称：void ICM_Get_Accelerometer(int16 *ax,int16 *ay,int16 *az)
*  功能说明：
*  参数说明：无
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：   
*************************************************************************/
void ICM_Get_Accelerometer(int16 *ax,int16 *ay,int16 *az)
{
  uint8 buf[7];  
  ICM_Read_Len(ICM_ACCEL_XOUTH_REG,6,buf);
  
  *ax=((uint16)buf[1]<<8)|buf[2];  
  *ay=((uint16)buf[3]<<8)|buf[4];  
  *az=((uint16)buf[5]<<8)|buf[6];  
}

/*************************************************************************
*  函数名称：void ICM_Get_Raw_data(int16 *ax,int16 *ay,int16 *az,int16 *gx,int16 *gy,int16 *gz)
*  功能说明：读取加速度陀螺仪数据
*  参数说明：六轴
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：   
*************************************************************************/
void ICM_Get_Raw_data(int16 *ax,int16 *ay,int16 *az,int16 *gx,int16 *gy,int16 *gz)
{
  uint8 buf[13];  
  ICM_Read_Len(ICM_ACCEL_XOUTH_REG,12,buf);
  
  *ax=((uint16)buf[1]<<8)|buf[2];  
  *ay=((uint16)buf[3]<<8)|buf[4];  
  *az=((uint16)buf[5]<<8)|buf[6];
  *gx=((uint16)buf[7]<<8)|buf[8];  
  *gy=((uint16)buf[9]<<8)|buf[10];  
  *gz=((uint16)buf[11]<<8)|buf[12];  
}

/*************************************************************************
*  函数名称：uint8 icm42605_init(void)
*  功能说明：ICM42605初始化
*  参数说明：无
*  函数返回：0 成功；1失败
*  修改时间：2020年8月10日
*  备    注：printf 需要在 clock_init() 之后调用 debug_init() 才能正常输出
*************************************************************************/
uint8 icm42605_init(void)
{   
  uint8 reg_val;
  
  icm42605_spi_init(); 
  system_delay_ms(200);  
  
  reg_val = icm42605_read_reg(who_am_i);//who_am_i
  printf("\r\nICM42605 ID:0x%02X",reg_val);
  
  icm42605_write_reg(reg_bank_sel,0x00);//Set to bank 0
  icm42605_write_reg(reg_bank_sel,0x00);//Set to bank 0
  icm42605_write_reg(device_config_reg,bit_soft_reset_chip_config);//chip soft reset
  system_delay_ms(100);  
  
  if(reg_val==0x42)
  {
    icm42605_write_reg(reg_bank_sel,0x01);//Change to bank 1
    icm42605_write_reg(intf_config4,0x02);//4 wire spi mode
    
    icm42605_write_reg(reg_bank_sel,0x00);        
    icm42605_write_reg(fifo_config_reg,0x40);//Stream-to-FIFO Mode
    
    reg_val = icm42605_read_reg(int_source0_reg);      
    icm42605_write_reg(int_source0_reg,0x00);    
    icm42605_write_reg(fifo_config2_reg,0x00);// watermark
    icm42605_write_reg(fifo_config3_reg,0x02);// watermark
    icm42605_write_reg(int_source0_reg, reg_val); 
    icm42605_write_reg(fifo_config1_reg,0x63);// Enable the accel and gyro to the FIFO
    
    icm42605_write_reg(reg_bank_sel,0x00);
    icm42605_write_reg(int_config_reg,0x36);   
    
    icm42605_write_reg(reg_bank_sel, 0x00);
    reg_val = (icm42605_read_reg(int_source0_reg)|bit_int_fifo_ths_int1_en);      
    icm42605_write_reg(int_source0_reg, reg_val);
    
    icm42605_write_reg(reg_bank_sel, 0x00);
    reg_val = ((icm42605_read_reg(accel_config0_reg)&0x1F)|(bit_accel_ui_fs_sel_8g));//8g
    icm42605_write_reg(accel_config0_reg, reg_val);
    
    icm42605_write_reg(reg_bank_sel, 0x00);
    reg_val = ((icm42605_read_reg(accel_config0_reg)&0xF0)|bit_accel_odr_50hz);
    icm42605_write_reg(accel_config0_reg, reg_val); 
    
    icm42605_write_reg(reg_bank_sel, 0x00);
    reg_val = ((icm42605_read_reg(gyro_config0_reg)&0x1F)|(bit_gyro_ui_fs_sel_1000dps));
    icm42605_write_reg(gyro_config0_reg,reg_val);
    
    icm42605_write_reg(reg_bank_sel, 0x00);      
    reg_val = ((icm42605_read_reg(gyro_config0_reg)&0xF0)|bit_gyro_odr_50hz);
    icm42605_write_reg(gyro_config0_reg, reg_val); 
    
    icm42605_write_reg(reg_bank_sel, 0x00);
    reg_val = icm42605_read_reg(pwr_mgmt0_reg)|(bit_accel_mode_ln); // Accel on in LNM
    icm42605_write_reg(pwr_mgmt0_reg, reg_val);  
    system_delay_ms(1);   
    
    icm42605_write_reg(reg_bank_sel, 0x00);
    reg_val = icm42605_read_reg(pwr_mgmt0_reg)|(bit_gyro_mode_ln); // Gyro on in LNM
    icm42605_write_reg(pwr_mgmt0_reg, reg_val);  
    system_delay_ms(1);   
    printf("\r\nICM42605 init ok");
    return 0;
  }
  else 
  {
    printf("\r\nICM42605 init fail");
    return 1;
  }   
}
/*************************************************************************
*  函数名称：void icm42605_read_fifo(Sample_data_type_t *fifo_data,uint16 len)
*  功能说明：读取FIFO
*  参数说明：无
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：   
*************************************************************************/
uint8  fifocount_l, fifocount_h;
uint16 fifocount;

void icm42605_read_fifo(Sample_data_type_t *fifo_data,uint16 len)
{
  uint8 reg_val;
  uint8 i;
  uint8 tempbuff[512]={0};
  reg_val = icm42605_read_reg(int_source0_reg);      
  icm42605_write_reg(int_source0_reg,0x00); 
  
  fifocount_h = icm42605_read_reg(fifo_byte_count_h_res); // Read the FIFO size
  fifocount_l = icm42605_read_reg(fifo_byte_count_l_res);
  fifocount = (fifocount_h<<8)|fifocount_l;
  
  icm42605_read_regs(fifo_data_port,tempbuff,len);
  if(fifocount>=fifo_packet_size) // If we have a complete packet in the FIFO
  {
    for(i=0;i<32;i++)
    {
      if((tempbuff[i*16]&fifo_accel_en)&&(tempbuff[i*16]&fifo_gyro_en))
      {
        fifo_data->Sample_accdata[0+i*3] = ((int16)((tempbuff[1+i*16] << 8) | tempbuff[2+i*16]))*acc_ssl;
        fifo_data->Sample_accdata[1+i*3] = ((int16)((tempbuff[3+i*16] << 8) | tempbuff[4+i*16]))*acc_ssl;
        fifo_data->Sample_accdata[2+i*3] = ((int16)((tempbuff[5+i*16] << 8) | tempbuff[6+i*16]))*acc_ssl;
        fifo_data->Sample_gyrdata[0+i*3] = ((int16)((tempbuff[7+i*16] << 8) | tempbuff[8+i*16]))/gyr_ssl;
        fifo_data->Sample_gyrdata[1+i*3] = ((int16)((tempbuff[9+i*16] << 8) | tempbuff[10+i*16]))/gyr_ssl;
        fifo_data->Sample_gyrdata[2+i*3] = ((int16)((tempbuff[11+i*16]<< 8) | tempbuff[12+i*16]))/gyr_ssl;
      }
    }
  }
  icm42605_write_reg(int_source0_reg, reg_val); 
}
/*************************************************************************
*  函数名称：void icm42605_stop(void)
*  功能说明：停止工作
*  参数说明：无
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：   
*************************************************************************/
void icm42605_stop(void)
{
  uint8 reg_val; 
  icm42605_write_reg(reg_bank_sel, 0x00);
  reg_val = icm42605_read_reg(pwr_mgmt0_reg)&(~bit_accel_mode_ln); // Accel off
  icm42605_write_reg(pwr_mgmt0_reg, reg_val);
  system_delay_ms(1);   
  
  icm42605_write_reg(reg_bank_sel, 0x00);
  reg_val = icm42605_read_reg(pwr_mgmt0_reg)|(bit_gyro_mode_ln); // Gyro on in LNM
  icm42605_write_reg(pwr_mgmt0_reg, reg_val);   
  system_delay_ms(400);  
}

/*************************************************************************
*  函数名称：void Test_ICM42605FIFO(void)
*  功能说明：测试，读取加速度陀螺仪数据
*  参数说明：无
*  函数返回：无
*  修改时间：2020年8月10日
*  备    注：原龙邱版本使用 OLED / TFT1.8 显示，此处统一改为逐飞库的 printf 输出，
*            使用时请先 clock_init()、debug_init()，然后调用本函数，数据会一直循环打印
*************************************************************************/
void Test_ICM42605FIFO(void)
{
  int16 aacx,aacy,aacz;	        //加速度传感器原始数据
  int16 gyrox,gyroy,gyroz;        //陀螺仪原始数据

  if(icm42605_init())
  {
    printf("\r\nICM42605 init fail.");
    return;
  }

  while(1)
  {
    ICM_Get_Raw_data(&aacx,&aacy,&aacz,&gyrox,&gyroy,&gyroz);	//得到加速度传感器数据  
    printf("\r\nLQ ICM42605 Test");
    printf("\r\nAX: %6d",aacx); 
    printf("\r\nAY: %6d",aacy);
    printf("\r\nAZ: %6d",aacz); 
    printf("\r\nGX: %6d",gyrox);
    printf("\r\nGY: %6d",gyroy); 
    printf("\r\nGZ: %6d",gyroz);
    system_delay_ms(100);
    printf("\r\n*********************\n");
  } 
}
