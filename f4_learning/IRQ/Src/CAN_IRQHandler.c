/*
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-10 17:23:40
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-08-10 17:24:18
 * @FilePath: \f4\f4_learning\IRQ\Src\CAN_IRQHandler.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "IRQ_Handler.h"
#include "can.h"

CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    // 确保是 CAN1 触发的中断
    if (hcan->Instance == CAN1) 
    {
        // 从 FIFO 0 把数据捞出来，存到 RxData 数组里
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            if (RxHeader.StdId == 0x201) {
                Beep_Trigger = 1; 
            }
            else if (RxHeader.StdId == 0x202) {
                // 这是 2 号电机发来的反馈...
            }
        }
    } else if (hcan->Instance == CAN2) 
    {
        
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            // 处理 CAN2 的消息...
        }
    }
}
