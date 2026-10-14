#include "zf_common_headfile.h"
#include "LQ_ICM42605.h"
#include "ICM42605jiesuan.h"

volatile int16 ax, ay, az, gx, gy, gz;

int print_count = 0; 

void pit_handler(void)
{
    ICM_Get_Raw_data(&ax, &ay, &az, &gx, &gy, &gz);
    IMU_Update(ax, ay, az, gx, gy, gz);
}

void main(void)
{
    clock_init(SYSTEM_CLOCK_30M);
    debug_init();
    
    pit_ms_init(TIM0_PIT, 1);
    if(icm42605_init()){
        while(1);
    }
    
    tim0_irq_handler = pit_handler; 
    
    // 【新增点】进入死循环前，暴力清除开机晃动带来的任何历史杂波角度
    yaw = 0.0f;
    pitch = 0.0f;
    roll = 0.0f;
    
    while(1)
    {
        print_count++;
        if(print_count >= 50) 
        {
            print_count = 0;
            printf("%.3f,%.3f,%.3f \n", yaw, pitch, roll);
        }
        
        system_delay_ms(1);
    }
}