
#ifndef __TERMINAL_H
#define __TERMINAL_H

#include "usbd_cdc.h"

typedef void (*TERM_tx_handler)(const uint8_t* buff, uint32_t len);
typedef int (*TERM_rx_handler)(const uint8_t* buff, uint32_t max_len);

void TERM_init(TERM_tx_handler tx_handler, TERM_rx_handler rx_handler);
void TERM_deinit(void);
void TERM_get_config(USBD_CDC_LineCodingTypeDef* cfg);
void TERM_set_config(USBD_CDC_LineCodingTypeDef* cfg);
void TERM_debug_print(const char *line);
void TERM_debug_print_int(const int value);

int TERM_get_input_buf(unsigned char* buf, int max_size);
#endif
