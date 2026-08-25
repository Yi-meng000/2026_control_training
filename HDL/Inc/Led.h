#ifndef LED_H
#define LED_H

/* ------------------------------------- Include --------------------------- */
#include "main.h"
#include "gpio.h"

/* ------------------------------------- Define ---------------------------- */

#define LED_ON(x) HAL_GPIO_WritePin(LED_##x##_GPIO_Port, LED_##x##_Pin, GPIO_PIN_SET)
#define LED_OFF(x) HAL_GPIO_WritePin(LED_##x##_GPIO_Port, LED_##x##_Pin, GPIO_PIN_RESET)
#define LED_TOGGLE(x) HAL_GPIO_TogglePin(LED_##x##_GPIO_Port, LED_##x##_Pin)


/* ------------------------------------- Function -------------------------- */

void Led_Water(void);


#endif /* LED_H */