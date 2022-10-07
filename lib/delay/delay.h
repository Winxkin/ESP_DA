#ifndef DELAY_H
#define DELAY_H
#include <stdio.h>
#include <esp_log.h>
#include <driver/gpio.h>

void delay_us(int us);
void delay_ms(int ms);
void delay_s(int s);

#endif
