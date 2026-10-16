#ifndef _MENU_H_
#define _MENU_H_
#define MENU_MAX 100
#define MENU_MIN 0
extern int16 ax, ay, az, gx, gy, gz;
void menu_init(void);
void menu(void);
void key_action(void);

#endif