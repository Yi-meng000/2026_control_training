/*
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-12 09:22:42
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-08-24 11:22:57
 * @FilePath: \f4_show\IRQ\Src\UART_IRQHandler.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "UART_IRQHandler.h"
#include "EXTI_IRQHandler.h"

uint8_t rx_buffer[100] = {0};
uint8_t tx_buffer[100] = {0};

void UART_Start_Recieve(void)
{
    // HAL_UART_Receive_IT(&huart1, rx_buffer, 5);
    // HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer));
    // HAL_UART_Receive_DMA(&huart1, rx_buffer, 5);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));
    // HAL_UART_Receive_IT(&huart2, rx_buffer, 5);
    // HAL_UARTEx_ReceiveToIdle_IT(&huart2, rx_buffer, sizeof(rx_buffer));
    // HAL_UART_Receive_DMA(&huart2, rx_buffer, 5);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer, sizeof(rx_buffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {

        // HAL_UART_Receive_IT(&huart1, rx_buffer, 5);
        HAL_UART_Receive_DMA(&huart1, rx_buffer, 5);

    }
    if (huart->Instance == USART2)
    {

        // HAL_UART_Receive_IT(&huart2, rx_buffer, 5);
        HAL_UART_Receive_DMA(&huart2, rx_buffer, 5);

    }

}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1) 
    {

        // HAL_UARTEx_ReceiveToIdle_IT(&huart1, rx_buffer, sizeof(rx_buffer));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer, sizeof(rx_buffer));

    }
    if (huart->Instance == USART2) 
    {
        
        // HAL_UARTEx_ReceiveToIdle_IT(&huart2, rx_buffer, sizeof(rx_buffer));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_buffer, sizeof(rx_buffer));

    }
}
