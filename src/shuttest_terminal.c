/**
 * @file    shuttest_terminal.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-11
 * @brief   File contains implementation for console interface 
 *          for shutter tester
 * 
 */
#include "shuttest_terminal.h"
#include <stdint.h>
#include "terminal.h"
#include "max44009.h"
#include "shuttest.h"

static uint8_t buffer[16];

static void SHTEST_term_parse_char(uint8_t ch);
static void SHTEST_term_print_help();
static void SHTEST_term_print_current_light();
static void SHTEST_term_print_max44009_regs();
static void SHTEST_term_print_result_table();
static void SHTEST_term_print_exposure(uint32_t time_ms);
void SHTEST_term_init() {
}


void SHTEST_term_parser() {
    int rc, i;
    rc = TERM_get_input_buf(buffer, sizeof(buffer));
    for(i =0; i < rc; i++) {
        SHTEST_term_parse_char(buffer[i]);
    }
}


static void SHTEST_term_parse_char(uint8_t ch) {
    switch(ch) {
    case 'h':
    case '?':
        SHTEST_term_print_help();
        break;
    case ' ':
        TERM_debug_print("Stop\r\n");
        st_stop();
        SHTEST_term_print_result_table();
        break;
    case '\r':
        TERM_debug_print("Start\r\n");
        st_start();
        break;
    case 'l':
    case 'L':
        SHTEST_term_print_current_light();
        break;
    case 'd':
        SHTEST_term_print_max44009_regs();
        break;
    default:
        TERM_debug_print("Unknown command\r\n");
        SHTEST_term_print_help();
        break;
    };
}

static void SHTEST_term_print_help() {
    TERM_debug_print("Help:\r\n");
    TERM_debug_print("<enter>: start \r\n");
    TERM_debug_print("<space>: stop and print results \r\n");
    TERM_debug_print("<l>: print current light level \r\n");
}

static void SHTEST_term_print_current_light() {
    max44009_dev_lux lux;
    TERM_debug_print("Current light: ");
    if(max44009_get_dev_lux(&lux) == 0) {
        TERM_debug_print_int(lux.mantissa);
        TERM_debug_print("e+");
        TERM_debug_print_int(lux.exponent);
    } else {
        TERM_debug_print("error");
    }
    TERM_debug_print("\r\n");
}

static void SHTEST_term_print_max44009_regs() {
    uint8_t regs[MAX44009_MAX_REG_NUM+1];
    int i;
    TERM_debug_print("max44009 regs: \r\n");
    if(max44009_debug_get_all_regs(regs) > 0) {
        for(i = 0; i<= MAX44009_MAX_REG_NUM; i++) {
            TERM_debug_print_int(i);
            TERM_debug_print(" :");
            TERM_debug_print_int(regs[i]);
            TERM_debug_print("\r\n");
        }
    } else {
        TERM_debug_print("error\r\n");
    }

}

static void SHTEST_term_print_result_table() {
    const uint32_t *result;
    const uint32_t test_number = st_get_result(&result);

    for(int i = 0; i < test_number; i++) {
        TERM_debug_print_int(result[i]/10);
        TERM_debug_print(".");
        TERM_debug_print_int(result[i]%10);
        TERM_debug_print("ms :");

        SHTEST_term_print_exposure(result[i]);
        TERM_debug_print("\r\n");
    }
}
static void SHTEST_term_print_exposure(uint32_t time) {
    #define ONE_SECOND 10000
    int error_percents = 0;
    if(time >= ONE_SECOND) {
        uint32_t s = time / ONE_SECOND;
        uint32_t ms = time % ONE_SECOND;

        if(ms > ONE_SECOND / 2) {
            s++;
            error_percents = -(ONE_SECOND - ms);
        } else {
            error_percents = ms;
        }
        TERM_debug_print_int(s);
        TERM_debug_print("s ");
    } else if (time != 0) {
        /* 1/2 1/4 1/8 1/15 1/30 1/60 1/125 1/250 1/500 1/1000 
          500  250 125  64   32  16    8      4     2    1
        */
        uint32_t fraction=0;
        const uint32_t ms = time/10;
        for(int i = 1; i < 32; i++) {
            if((1000 >> i) < ms) {
                if((ms - (1000 >> i)) > ((1000 >> (i-1)) - ms))
                {
                    fraction = i-1;
                    error_percents = -((1000 >> (i-1)) - ms)*10 - (10 - time%10);
                } else {
                    fraction = i;
                    error_percents = ms - (1000 >> (i))*10 + (time%10);
                }
                break;
            }
        }
        TERM_debug_print("1/");
        TERM_debug_print_int(1<<fraction);
        TERM_debug_print(" ");
    } else {
        TERM_debug_print("0 ");
        error_percents = 0;
    }
    if(error_percents >= 0) {
        TERM_debug_print("+");
    } else {
        TERM_debug_print("-");
        error_percents = -error_percents;
    }
    TERM_debug_print_int(error_percents/100);
    TERM_debug_print(".");
    TERM_debug_print_int(error_percents%100);
    TERM_debug_print("%");
}
