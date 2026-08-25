#ifndef TIM_IRQHandler_H
#define TIM_IRQHandler_H

#include "main.h"
#include "tim.h"
#include "EXTI_IRQHandler.h"

void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif