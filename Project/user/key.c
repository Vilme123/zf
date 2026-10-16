#include "key.h"
int16 key_up = 1,key_down = 1;
//»ñÈ¡°´¼ü×´Ì¬
int16 get_key(gpio_pin_enum pin){
    int16 key;
    key = gpio_get_level(pin);
    return key;
}