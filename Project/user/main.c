#include "zf_common_headfile.h"

void main(void)
{
    clock_init(SYSTEM_CLOCK_30M);
    debug_init();               // 初始化官方 debug 串口以支持 printf
    mydebug_init();
    menu_init();
    key_init();   

    while (1)
    {
        key_up = get_key(P70);
        key_down = get_key(P73);
        key_action();
        menu();
        printf("%d,%d,%d,%d,%d,%d\r\n", ax, ay, az, gx, gy, gz);
        system_delay_ms(100);
    }
}