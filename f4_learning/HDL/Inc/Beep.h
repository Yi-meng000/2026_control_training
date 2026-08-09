#ifndef BEEP_H
#define BEEP_H

/*----------------------- include ---------------------*/
#include "gpio.h"
#include "main.h"

/*----------------------- Define ---------------------*/
#define BEEP_ON() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET)
#define BEEP_OFF() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET)

/*----------------------- Function ---------------------*/
void Beep_Init(void);
void Beep_Alarm(uint8_t times);

#endif