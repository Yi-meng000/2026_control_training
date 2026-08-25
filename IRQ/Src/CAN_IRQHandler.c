/*
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-13 10:00:13
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-08-25 16:43:21
 * @FilePath: \f4_show\IRQ\Src\CAN_IRQHandler.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "CAN_IRQHandler.h"
#include "DJmotor.h"
#include "ZDrive.h"


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];    

    if (hcan->Instance == CAN1) 
    {
        // 从 FIFO 0 把数据捞出来，存到 RxData 数组里
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
        #if USE_DJ && (MOTOR_DJI_CAN_BUS == 0U)
            DJmotor_Receive(RxHeader, RxData);
        #endif
        }
    } else if (hcan->Instance == CAN2) 
    {
        
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            // 处理 CAN2 的消息...
        }
    }
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[8];

    if (hcan->Instance == CAN1)
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK)
        {

        }
    }
    else if (hcan->Instance == CAN2)
    {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &RxHeader, RxData) == HAL_OK)
        {
        #if USE_ZMDR
            ZdriveReceive(RxHeader, RxData, 1U);
        #endif
        }
    }
}
