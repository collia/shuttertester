
#include "string.h"
#include "stdbool.h"
#include "terminal.h"
#include "main.h"
//#include "termgpio.h"

USBD_CDC_LineCodingTypeDef LineCoding = {
  115200,                       /* baud rate */
  0x00,                         /* stop bits-1 */
  0x00,                         /* parity - none */
  0x08                          /* nb. of bits 8 */
};

static TERM_tx_handler low_tx_handler;
static TERM_rx_handler low_rx_handler;

static volatile bool TERM_inited = false;

static int TERM_itona(const int value, char *str, const int max_size );

void TERM_init(TERM_tx_handler tx_handler, TERM_rx_handler rx_handler)
{
    low_tx_handler = tx_handler;
    low_rx_handler = rx_handler;
    TERM_inited = true;
}

void TERM_deinit()
{
    if(!TERM_inited)
        return;
}

void TERM_get_config(USBD_CDC_LineCodingTypeDef* cfg)
{
    if(!TERM_inited)
        return;
    if(cfg)
    {
        memcpy(cfg, &LineCoding,sizeof(LineCoding));
    }
}

void TERM_set_config(USBD_CDC_LineCodingTypeDef* cfg)
{
    if(!TERM_inited)
        return;
    if(cfg)
    {
        memcpy(&LineCoding, cfg,sizeof(LineCoding));
    } 
}

void TERM_debug_print(const char *line)
{
    if(!TERM_inited)
        return;
    low_tx_handler((uint8_t*)line, strlen(line));
}

void TERM_debug_print_int(const int value)
{
    char buffer[50];
    char *bp = buffer;
    if(!TERM_inited)
        return;

    bp += TERM_itona(value, bp, &buffer[50]-bp-2);
    //*bp++ = '\r';
    //*bp++ = '\n';
    *bp++ = 0;
    TERM_debug_print(buffer);
}

int TERM_get_input_buf(unsigned char* buf, int max_size)
{
    while(!TERM_inited);

    int rc;
    rc = low_rx_handler(buf, max_size);
    return rc;
}




/**
 * @brif Convert int to string
 * @param[in] value value that will be converted to string
 * @param[in/out] str - buffer where will be stored result
 * @param[in] max_size - size of buffer, when this value 
 *                       be reached will be exit from function
 * @return - len of result string
 */
static int TERM_itona(const int value, char *str, const int max_size )
{
    char *s = str;
    int len = 0;
    int absvalue = value;
    int tmp=value;
    int res;

    if(value == 0)
    {
        *s = '0';
        return 1;
    }
    if(value < 0)
    {
        if(max_size > 0)
        {
            *s = '-';
            ++s;
        }
        absvalue = -value;
    }

    while(tmp!=0)
    {
        tmp = tmp / 10;
        if((len + (s-str)) < max_size)
            len++;
    }
    res = &s[len] - str;
    tmp = absvalue;
    while(tmp!=0)
    {
        if((&s[len-1] - str) < max_size)
            s[len-1] = '0' + tmp % 10;
        tmp /= 10;
        len--;
    }

    return res;
}
