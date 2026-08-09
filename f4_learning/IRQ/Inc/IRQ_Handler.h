#ifndef IRQ_HANDLER_H
#define IRQ_HANDLER_H

/*--------------------------- Include -------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
/*--------------------------- Define --------------------------*/

/*--------------------------- Function ------------------------*/
void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void UART_Start_Receive(void);

/*--------------------------- Variable ------------------------*/
extern volatile uint8_t Beep_Trigger;

#endif 
