#include "IRQ_Handler.h"

volatile uint8_t Beep_Trigger = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    Beep_Trigger = 1;
}
