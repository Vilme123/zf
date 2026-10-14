#include "zf_common_headfile.h"
#include "LQ_ICM42605.h"
#include "ICM42605jiesuan.h"
#include <math.h>

float roll = 0.0;
float pitch = 0.0;
float yaw = 0.0;

// 将解算合并为一个函数，由定时器中断严格按 T (0.001s) 周期调用
void IMU_Update(int16 ax, int16 ay, int16 az, int16 gx, int16 gy, int16 gz)
{
    float pitch_a, roll_a;
    
    // 1. 将陀螺仪原始数据转换为真实的角速度 (单位: 度/秒)
    // 【修改点】除数改为 32.8f，匹配 ±1000 dps 的硬件量程
    float gyro_x = (float)gx / 32.8f;
    float gyro_y = (float)gy / 32.8f;
    float gyro_z = (float)gz / 32.8f;
    
    // 【新增点】2. Yaw 抗漂移死区处理
    // 过滤掉静止状态下的传感器底噪 (±0.3度/秒以内的数值强制归零)
    if(gyro_z > -0.3f && gyro_z < 0.3f) {
        gyro_z = 0.0f;
    }
    
    // 3. 通过加速度计计算出俯仰和滚转角 (注意单位是度)
    pitch_a = atan2(ax, az) / 3.1415927f * 180.0f; 
    roll_a  = atan2(ay, az) / 3.1415927f * 180.0f; 
    
    // 4. 互补滤波：上一次的融合角度 + 陀螺仪积分的增量
    pitch = a * (pitch + gyro_y * T) + (1.0f - a) * pitch_a;
    roll  = a * (roll  + gyro_x * T) + (1.0f - a) * roll_a;
    
    // 5. Yaw 角只能靠陀螺仪 Z 轴积分
    yaw = yaw + gyro_z * T;
}