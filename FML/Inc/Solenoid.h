#ifndef __SOLENOID_H
#define __SOLENOID_H

#include "main.h"

typedef struct Solenoid_t
{
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin_sda;
    uint16_t gpio_pin_clk;
    uint8_t data_prve;
} Solenoid_t;

void solenoid_init(uint8_t usart_channel);
void solenoid_on(uint8_t usart_channel, uint8_t cmd);
#endif /* __SOLENOID_H */
