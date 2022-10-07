#ifndef UART_H
#define UART_H	
#include <stdio.h>
#include <esp_log.h>
#include <string.h>
#include <driver/gpio.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_intr_alloc.h"

typedef void (*uart_callback_t) (void);

void start_uart_init(uart_port_t uart_num);
void start_uart_init_config_pin(uart_port_t uart_num, int tx_io, int rx_io);
void setcallback_uart(void *cb);
void sentdata(uart_port_t uart_num,char *data);

void cleardata (uint8_t *data, int len);


#endif
