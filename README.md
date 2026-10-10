# STC32G12K128 逐飞开源库工程（含 ICM-42605 六轴驱动）

基于 **成都逐飞科技 STC32G 开源库 V3.2.4** 的 STC32G12K128 工程，在库的基础上增加了一颗
**ICM-42605 六轴 IMU** 的驱动（`LQ_ICM42605`），并把它的 SPI 通信从龙邱（LongQiu）平台的
**GPIO 模拟 SPI** 移植到了**逐飞库的硬件 SPI**。

* MCU：STC32G12K128
* 系统时钟：30MHz（`clock_init(SYSTEM_CLOCK_30M)`）
* 编译环境：Keil MDK FOR C251（uVision5）
* 编译状态：**0 Error / 0 Warning**
  `Program Size: data=8.4  edata+hdata=2060  xdata=7967  const=4561  code=26456`

---

## 一、ICM-42605 接线（重要）

| ICM-42605 | STC32G12K128 | 说明 |
| :--- | :--- | :--- |
| SCLK / SPC | **P2.5** | 硬件 SPI_0 CH2 时钟 |
| SDI / MOSI | **P2.3** | 主出从入 |
| SDO / MISO | **P2.4** | 主入从出 |
| CS | **P2.6** | 片选，软件控制，低电平有效 |
| VCC | 3.3V | 不要接 5V |
| GND | GND | |

> ⚠️ **STC32G 的硬件 SPI 只能使用固定的引脚组**（P1.3/1.4/1.5、P2.3/2.4/2.5、P3.2/3.3/3.4、
> P4.0/4.1/4.3、P6.5/6.6/6.7、P7.5/7.6/7.7）。原龙邱代码里的 `PTE2~PTE5` 做不了硬件 SPI，
> 所以接线必须按上表调整。

**SPI 参数**：模块 `SPI_0` ／ 模式 `SPI_MODE3` ／ 期望速率 1MHz
（库会把速率适配成 系统时钟/4、/8、/16、/2 中最接近的一档，30MHz 下实际约 **1.875MHz**）

改引脚、模式、速率只需编辑 `Project/user/LQ_ICM42605.h` 顶部的 `ICM42605_SPI_*` 宏：

```c
#define ICM42605_SPI            (SPI_0)                  // SPI 模块
#define ICM42605_SPI_MODE       (SPI_MODE3)              // ICM-42605 支持模式0/模式3
#define ICM42605_SPI_SPEED      (1 * 1000 * 1000)        // 期望速率
#define ICM42605_SPI_SCK_PIN    (SPI0_CH2_SCLK_P25)      // SCK
#define ICM42605_SPI_MOSI_PIN   (SPI0_CH2_MOSI_P23)      // MOSI
#define ICM42605_SPI_MISO_PIN   (SPI0_CH2_MISO_P24)      // MISO
#define ICM42605_SPI_CS_PIN     (IO_P26)                 // CS
#define ICM42605_SPI_CS(x)      (P26 = x)                // CS 电平控制
```

---

## 二、目录结构

```
Libraries/                    逐飞科技 STC32G 开源库 V3.2.4（原样引用，未修改）
  zf_common/                  公共层：时钟、延时、调试串口、FIFO、字体…
  zf_driver/                  外设驱动层：spi / gpio / uart / adc / pwm / timer / exti…
  zf_device/                  产品驱动层：imu660ra/rb/rc、imu963ra、icm20602、ips 屏…
  zf_components/              组件层：逐飞助手（虚拟示波器 / 图像上位机）
  doc/                        库版本说明
Project/
  user/                       用户代码（Keil 的 user 分组）
    main.c                    主程序
    isr.c / isr.h             中断服务函数
    LQ_ICM42605.c / .h        ICM-42605 六轴驱动（已适配逐飞库硬件 SPI）
  mdk/                        Keil MDK 工程
    seekfree.uvproj           工程文件（双击打开）
    seekfree.uvopt            工程选项
  code/                       预留的用户代码目录（已在 Keil 包含路径中）
.vscode/                      VS Code 配置：GB2312 编码、IntelliSense 头文件路径
.gitignore                    排除 Keil 编译产物等
```

---

## 三、编译

**图形界面**：用 Keil uVision5 打开 `Project/mdk/seekfree.uvproj`，直接 Build。

**命令行**（批处理编译，日志写到 build.log）：

```bat
"D:\Keil_v5\UV4\UV4.exe" -b "Project\mdk\seekfree.uvproj" -o build.log
```

---

## 四、ICM-42605 驱动用法

```c
#include "zf_common_headfile.h"
#include "LQ_ICM42605.h"

void main(void)
{
    int16 ax, ay, az, gx, gy, gz;          // Keil C251 不支持在语句后声明变量，先声明

    clock_init(SYSTEM_CLOCK_30M);
    debug_init();                          // 初始化调试串口，printf 才有效

    if(icm42605_init())                    // 返回 0 成功、1 失败（ID 会 printf 出来）
    {
        while(1);
    }

    while(1)
    {
        ICM_Get_Raw_data(&ax, &ay, &az, &gx, &gy, &gz);
        printf("\r\nAX:%6d AY:%6d AZ:%6d GX:%6d GY:%6d GZ:%6d", ax, ay, az, gx, gy, gz);
        system_delay_ms(100);
    }
}
```

也可以直接调用 `Test_ICM42605FIFO();`，它会一直循环打印六轴数据。

### 对外接口

| 函数 | 说明 |
| :--- | :--- |
| `void icm42605_spi_init(void)` | 逐飞库硬件 SPI 初始化（原 `SPI_SoftInit`），需在 `clock_init()` 之后调用 |
| `void icm42605_spi_read_write(uint8 *buf, uint16 len)` | 硬件 SPI 全双工收发、收发共用缓冲区（原 `SPI_SoftReadWriteNbyte`） |
| `uint8 icm42605_init(void)` | 传感器初始化与寄存器配置，返回 0 成功 / 1 失败 |
| `uint8 icm42605_read_reg(uint8 reg)` | 读单个寄存器 |
| `uint8 icm42605_write_reg(uint8 reg, uint8 value)` | 写单个寄存器 |
| `uint8 icm42605_read_regs(uint8 reg, uint8 *buf, uint16 len)` | 连续读寄存器 |
| `void ICM_Read_Len(uint8 reg, uint8 len, uint8 *buf)` | 连续读寄存器（数据放在 `buf[1]` 起） |
| `void ICM_Get_Accelerometer(int16 *ax, int16 *ay, int16 *az)` | 读三轴加速度原始值 |
| `void ICM_Get_Gyroscope(int16 *gx, int16 *gy, int16 *gz)` | 读三轴陀螺仪原始值 |
| `void ICM_Get_Raw_data(int16 *ax, int16 *ay, int16 *az, int16 *gx, int16 *gy, int16 *gz)` | 一次读全六轴（推荐） |
| `void icm42605_read_fifo(Sample_data_type_t *data, uint16 len)` | 突发读取 FIFO（每包 16 字节） |
| `void icm42605_stop(void)` | 关闭加速度计 |
| `void Test_ICM42605FIFO(void)` | 测试函数，循环打印六轴数据 |

### 当前初始化配置（见 `icm42605_init()`）

* 加速度计：**±8g**，ODR **50Hz**
* 陀螺仪：**±1000dps**，ODR **50Hz**
* FIFO：Stream-to-FIFO 模式，每包 16 字节、最多 32 包，使能 accel + gyro
* 换算系数：`acc_ssl = 0.244f`（mg/LSB）、`gyr_ssl = 32.8f`（LSB/dps）

---

## 五、从龙邱平台移植过来做了哪些改动

| 项目 | 原来（龙邱 K66） | 现在（逐飞 STC32G 库） |
| :--- | :--- | :--- |
| SPI 实现 | `SPI_SoftInit()` 逐个配置 GPIO + 循环翻 IO 口模拟时序 | `spi_init()` 初始化硬件 SPI + `spi_transfer_8bit()` 收发 |
| 片选 CS | 宏直接操作 PTE5 | 宏直接操作 `P26`，`spi_init()` 传 `SPI_CS_NULL` 由软件控制 |
| 头文件 | `#include "include.h"`（龙邱库） | `#include "zf_common_headfile.h"` |
| 数据类型 | `uint8_t` / `uint16_t` / `int16_t` | 逐飞库的 `uint8` / `uint16` / `int16` |
| 延时 | `delayms()` | `system_delay_ms()` |
| 调试显示 | OLED / TFT1.8 屏显示 | 统一改为 `printf`（经 `debug_init()` 的串口输出） |
| 旧函数名 | `SPI_SoftInit()` / `SPI_SoftReadWriteNbyte()` | 保留为宏，旧代码无需修改即可编译 |

寄存器初始化序列、六轴数据解析、`buf[1]` 起始的缓冲区约定**全部保持原样**，未做改动。

---

## 六、注意事项

1. **缓冲区约定**：`ICM_Read_Len()` 与 `icm42605_read_regs()` 读回的数据从 `buf[1]` 开始存放，
   调用方缓冲区**至少要 `len+1` 个字节**，否则会越界。
2. **SPI 模式**：`SPI_MODE3` 与原模拟 SPI 的时序一致（空闲高、下降沿出数、上升沿采样）。
   如果通信异常，可改成 `SPI_MODE0` —— ICM-42605 两种模式都支持。
3. **编译产物不入库**：`Project/mdk/out_file/`（.obj/.crf/.hex/.map 等）已在 `.gitignore` 中排除，
   需要固件时用 Keil 重新编译即可。
4. **Keil C251 的两个坑**（本工程已规避）：`data` 是 C251 的存储器类型关键字，不能用作变量名；
   也不支持 `for(uint8 i = 0; ...)` 这种在循环里声明变量的写法。
5. **VS Code 中文乱码**：工程内部分文件是 GB2312/GBK 编码，`.vscode/settings.json` 里已设置
   `"files.encoding": "gb2312"` 和 `"files.autoGuessEncoding": true`。

---

## 七、版权说明

* `Libraries/` 目录为**成都逐飞科技有限公司**的 STC32G 开源库，遵循 **GPL-3.0** 协议。
* `Project/user/LQ_ICM42605.c/.h` 源自**北京龙邱智能科技**的 ICM-42605 驱动（原作者 chiusir），
  原文件声明「版权所有，单位使用请先联系授权」，本工程仅对其 SPI 通信部分做了逐飞库适配移植。
* 其余用户代码版权归本仓库作者所有。
