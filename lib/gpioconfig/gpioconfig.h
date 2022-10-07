#ifndef GPIOCONFIG_H
#define GPIOCONFIG_H
#include <stdio.h>
#include <esp_log.h>
#include <driver/gpio.h>

typedef enum
{
    L_to_H = 1,
    H_to_L = 2,
    ANY_EDLE = 3
} interrupt_type_edle_t;

typedef void (*input_callback_t)(int);

void input_io_create(gpio_num_t gpio_num, interrupt_type_edle_t type);
void input_io_get_level(gpio_num_t gpio_num);
void input_set_callback(void *cb);
void output_io_creat(gpio_num_t gpio_num);
void output_io_toggle(gpio_num_t gpio_num);

#endif
