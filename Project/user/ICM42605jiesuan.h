#ifndef _ICM42605JIESUAN_H_
#define _ICM42605JIESUAN_H_

#define T 0.001
#define a 0.95238

extern float yaw;
extern float pitch;
extern float roll;

// 声明新的整体解算函数
void IMU_Update(int16 ax, int16 ay, int16 az, int16 gx, int16 gy, int16 gz);

#endif