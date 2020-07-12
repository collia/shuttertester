/**
 * @file    shuttest_terminal.c
 * @author  Nikolay
 * @license MIT
 * @date    2020-07-11
 * @brief   File contains implementation for console interface 
 *          for shutter tester
 *
 * 
 */
#include "shuttest_terminal.h"
#include <stdint.h>
#include "terminal.h"
#include "max44009.h"

static uint8_t buffer[16];

static void SHTEST_term_parse_char(uint8_t ch);
static void SHTEST_term_print_help();
static void SHTEST_term_print_current_light();
static void SHTEST_term_print_max44009_regs();

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
        max44009_disable_irq();
        break;
    case '\r':
        TERM_debug_print("Start\r\n");
        max44009_set_up_treshold();
        max44009_enable_irq();
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
