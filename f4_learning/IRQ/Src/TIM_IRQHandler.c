#include "IRQ_Handler.h"
#include "math.h"
#include "string.h"

static uint16_t tim_count = 0;
static uint8_t dir = 1;
uint16_t time = 0;
float angle = 0;
uint8_t tail[4] = {0x00, 0x00, 0x80, 0x7f};
uint8_t tx_buffer[8] = {0}; 

void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM2){

        // time++;
        // angle = sin(time * 0.01);
        // memcpy(tx_buffer, &angle, sizeof(angle));
        // memcpy(tx_buffer + sizeof(angle), tail, sizeof(tail));
        // HAL_UART_Transmit_DMA(&huart1, tx_buffer, sizeof(tx_buffer));
        if (dir == 1) {
            tim_count++;                 
        } else {
            tim_count--;                
        }

        if (tim_count >= 1000) {
            dir = 0;             
        } else if (tim_count == 0) {
            dir = 1;             
        }
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, tim_count); 
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, tim_count); 
        
    }
}

