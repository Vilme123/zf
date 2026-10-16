#include "init.h"
//初始化按键
void key_init(){
    gpio_init(P70,GPI,1,GPI_PULL_UP);//key_up按键
    gpio_init(P71,GPI,1,GPI_PULL_UP);//按键2
    gpio_init(P73,GPI,1,GPI_PULL_UP);//key_down按键
    gpio_init(P72,GPI,1,GPI_PULL_UP);//按键4
}
// 初始化菜单
void menu_init(){
    ips114_set_dir(IPS114_CROSSWISE);
    ips114_init();
    ips114_clear(RGB565_BLACK);
//    if(icm42605_init())
//    {
//        while(1);
//    }
}

