#include "zf_common_headfile.h"
#include "ICM42605jiesuan.h"
#include "menu.h"
uint16 page=1,arrow=0;
int16 ax=0, ay=0, az=0, gx=0, gy=0, gz=0;
void menu_init(){
    ips114_set_dir(IPS114_CROSSWISE);
    ips114_init();
    ips114_clear(RGB565_BLACK);
//    if(icm42605_init())
//    {
//        while(1);
//    }
}

void menu(){
//    ICM_Get_Raw_data(&ax, &ay, &az, &gx, &gy, &gz);
//    IMU_Update(ax, ay, az, gx, gy, gz);
    ips114_show_string(0,arrow,"->");
    ips114_show_string(20,0,"ax:");ips114_show_int16(40,0,ax);
    ips114_show_string(20,20,"ay:");ips114_show_int16(40,20,ay);
    ips114_show_string(20,40,"az:");ips114_show_int16(40,40,az);
    ips114_show_string(20,60,"gx:");ips114_show_int16(40,60,gx);
    ips114_show_string(20,80,"gy:");ips114_show_int16(40,80,gy);  
    ips114_show_string(20,100,"gz:");ips114_show_int16(40,100,gz);
//    ips114_show_string(120,0,"yaw");ips114_show_float(160,0,yaw,3,3);
//    ips114_show_string(120,20,"pitch");ips114_show_float(160,20,pitch,3,3);
//    ips114_show_string(120,40,"roll");ips114_show_float(160,40,roll,3,3);
}

