#include "gpioconfig.h"

input_callback_t input_callback = NULL;

static void IRAM_ATTR gpio_input_handler(void *arg)
{
    int gpio_num = (uint32_t)arg;
    input_callback(gpio_num);
}

void input_io_create(gpio_num_t gpio_num, interrupt_type_edle_t type)
{
    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(gpio_num, GPIO_MODE_INPUT);
    gpio_set_pull_mode(gpio_num, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(gpio_num, type);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, gpio_input_handler, (void *)gpio_num);
}

void input_io_get_level(gpio_num_t gpio_num)
{
    return gpio_get_level(gpio_num);
}

void input_set_callback(void *cb)
{
    input_callback = cb;
}

void output_io_creat(gpio_num_t gpio_num)
{
    gpio_pad_select_gpio(gpio_num);
    gpio_set_direction(gpio_num, GPIO_MODE_OUTPUT);
}

void output_io_toggle(gpio_num_t gpio_num)
{
    gpio_set_level(gpio_num, 1);
    ets_delay_us(1000);
    gpio_set_level(gpio_num, 0);
    ets_delay_us(1000);
}
