#include "delay.h"

void delay_us(int us)
{
    ets_delay_us(us);
}

void delay_ms(int ms)
{
    int i;
    for (i = 0; i < ms; i++)
    {
        ets_delay_us(1000);
    }
}

void delay_s(int s)
{
    int i;
    for (i = 0; i < (s * 1000); i++)
    {
        ets_delay_us(1000);
    }
}