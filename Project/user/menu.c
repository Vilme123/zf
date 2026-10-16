#include "zf_common_headfile.h"

uint16 page = 1;
int16 arrow = 0; // 改为有符号 int16，防止无符号下溢
int16 ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;

// 菜单界面显示
void menu(){
    ips114_show_string(0, arrow, "->");
    ips114_show_string(20, 0,   "ax:"); ips114_show_int16(50, 0,   ax);
    ips114_show_string(20, 20,  "ay:"); ips114_show_int16(50, 20,  ay);
    ips114_show_string(20, 40,  "az:"); ips114_show_int16(50, 40,  az);
    ips114_show_string(20, 60,  "gx:"); ips114_show_int16(50, 60,  gx);
    ips114_show_string(20, 80,  "gy:"); ips114_show_int16(50, 80,  gy);  
    ips114_show_string(20, 100, "gz:"); ips114_show_int16(50, 100, gz);
}

// 箭头选中参数
void key_action(){
    if(key_up == 0){
        system_delay_ms(200);
        if(arrow < MENU_MAX){       // 严格小于 MENU_MAX(100)，防止变成 120 越界
            arrow += 20;
        }
        ips114_clear(RGB565_BLACK);
    }

    if(key_down == 0){
        system_delay_ms(200);
        if(arrow > MENU_MIN){       // 严格大于 MENU_MIN(0)，防止 0-20 下溢
            arrow -= 20;
        }
        ips114_clear(RGB565_BLACK);
    }
}