#include "debug.h"
#include "menu.h"

// PID参数定义
// static struct PID pid_speed;   // 速度环 PID
// static struct PID pid_angle;   // 角度环 PID

// 状态机相关变量
static enum {
    STATE_IDLE,
    STATE_GOT_HASH,
    STATE_GOT_P,
    STATE_GET_ID,
    STATE_GET_VALUE,
} parse_state = STATE_IDLE;

static unsigned char param_id = 0;
static float param_value = 0.0f;
static unsigned char value_buf[16];
static unsigned char value_len = 0;
static unsigned char dot_flag = 0;
static unsigned char decimal_places = 0;
static signed char sign = 1;

// 辅助函数：将暂存的字符串转换为浮点数
static float parse_value_buffer(void)
{
    float result = 0.0f;
    unsigned char i;
    unsigned char dot_pos = 0;
    unsigned char frac_len;
    float frac;

    for (i = 0; i < value_len; i++) {
        if (value_buf[i] == '.') {
            dot_pos = i;
            break;
        }
    }

    if (dot_pos == 0) {
        for (i = 0; i < value_len; i++) {
            result = result * 10.0f + (value_buf[i] - '0');
        }
    } else {
        for (i = 0; i < dot_pos; i++) {
            result = result * 10.0f + (value_buf[i] - '0');
        }
        frac_len = value_len - dot_pos - 1;
        frac = 0.0f;
        for (i = dot_pos + 1; i < value_len; i++) {
            frac = frac * 10.0f + (value_buf[i] - '0');
        }
        while (frac_len--) {
            frac /= 10.0f;
        }
        result += frac;
    }
    return result * (float)sign;
}

// 处理一个字节
void VOFA_ProcessByte(unsigned char rx_byte)
{
    switch (parse_state) {
        case STATE_IDLE:
            if (rx_byte == '#') {
                parse_state = STATE_GOT_HASH;
            }
            break;

        case STATE_GOT_HASH:
            if (rx_byte == 'P') {
                param_id = 0;
                parse_state = STATE_GOT_P;
            } else {
                parse_state = STATE_IDLE;
            }
            break;

        case STATE_GOT_P:
            if (rx_byte >= '0' && rx_byte <= '9') {
                param_id = param_id * 10 + (rx_byte - '0');
            } else if (rx_byte == '=') {
                value_len = 0;
                dot_flag = 0;
                sign = 1;
                parse_state = STATE_GET_VALUE;
            } else {
                param_id = 0;
                parse_state = STATE_IDLE;
            }
            break;

        case STATE_GET_ID:
            // 不再使用，多位ID已在STATE_GOT_P处理
            break;

        case STATE_GET_VALUE:
            if (rx_byte == '!') {
                if (value_len > 0) {
                    param_value = parse_value_buffer();
                    switch (param_id) {

                        case  1 : ax = param_value ; break;
                        // case  2 :  car_pid.left.Ki = param_value;car_pid.right.Ki = param_value; break;
                        // case  3 :  car_pid.angular.Kp = param_value; break;
                        // case  4 :  car_pid.angular.Kd = param_value; break;
                        // case  5 :  car_pid.small_diff=param_value; break;
                        // case  6 :  car_pid.large_diff= param_value; break;
                        // case  7 :  car_pid.em_pd.Kp = param_value ; break ;
                        // case  8 :  car_pid.em_pd.Kd = param_value ; break ;
                        
                        // case 10: car_state = (uint8)param_value; printf("P10=%d\r\n", car_state); break;
                    }
                }
                parse_state = STATE_IDLE;
            } else if (rx_byte == '-') {
                if (value_len == 0) {
                    sign = -1;
                }
            } else if (rx_byte == '.') {
                if (!dot_flag && value_len < sizeof(value_buf)-1) {
                    dot_flag = 1;
                    value_buf[value_len++] = '.';
                }
            } else if (rx_byte >= '0' && rx_byte <= '9') {
                if (value_len < sizeof(value_buf)-1) {
                    value_buf[value_len++] = rx_byte;
                }
            }
            break;

        default:
            parse_state = STATE_IDLE;
            break;
    }
}

void mydebug_init(void)
{
    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);
    uart_rx_interrupt(UART_INDEX, ZF_ENABLE);
    uart1_irq_handler = uart_rx_interrupt_handler;
}

void uart_rx_interrupt_handler(unsigned char dat)
{
    VOFA_ProcessByte(dat);
}