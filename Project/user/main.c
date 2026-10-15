#include "zf_common_headfile.h"
#include "LQ_ICM42605.h"
#include "menu.h"
#include "debug.h"

void main(void)
{

    clock_init(SYSTEM_CLOCK_30M);
    mydebug_init();
    menu_init();

    while (1)
    {
        menu();
			printf("%d,%d,%d,%d,%d,%d\n",ax,ay,az,gx,gy,gz);
        system_delay_ms(100);
    }
}
