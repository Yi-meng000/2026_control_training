/**
 * @file    CanQueue.c
 * @brief   CAN TX queue implementation, ported from H7 (FDCAN) to F4 (bxCAN).
 *
 * 纯发送队列:ZdriveEnqueue / VESC 入队,TIM2 中断 CAN_DequeueTx 出队。
 * 接收不再经过软件队列,反馈帧在 CAN 接收中断里直接解析。
 */
#include "CanQueue.h"
#include <string.h>

CAN_SendQueueType CAN1_Txqueue;
CAN_SendQueueType CAN2_Txqueue;

static bool s_can_queue_initialized;

void CAN_InitSendQueue(void)
{
    if (s_can_queue_initialized)
    {
        return; /* do not reset queues that may already contain frames */
    }

    CAN1_Txqueue.Front = CAN1_Txqueue.Rear = 0;
    CAN2_Txqueue.Front = CAN2_Txqueue.Rear = 0;

    CAN1_Txqueue.Canx = &hcan1;
    CAN2_Txqueue.Canx = &hcan2;

    s_can_queue_initialized = true;
}


bool CAN_Queue_IfEmpty(CAN_SendQueueType *queue)
{
    return (queue->Front == queue->Rear);
}

bool CAN_Queue_IfFull(CAN_SendQueueType *queue)
{
    return (((uint16_t)queue->Rear + 1U) % CAN_QUEUESIZE == queue->Front);
}

bool CAN_DequeueTx(CAN_SendQueueType *queue)
{
    CAN_TxHeaderTypeDef tx_message;
    uint8_t tx_data[8] = {0};
    uint32_t tx_mailbox = 0;

    if (CAN_Queue_IfEmpty(queue))
    {
        return false;
    }

    tx_message.IDE = queue->CAN_DataSend[queue->Front].IDE;
    tx_message.RTR = CAN_RTR_DATA;
    tx_message.DLC = queue->CAN_DataSend[queue->Front].DLC;
    if (tx_message.DLC > 8U)
    {
        tx_message.DLC = 8U; /* classic CAN 8-byte frames */
    }
    tx_message.TransmitGlobalTime = DISABLE;

    if (tx_message.IDE == CAN_ID_STD)
    {
        tx_message.StdId = queue->CAN_DataSend[queue->Front].ID;
    }
    else
    {
        tx_message.ExtId = queue->CAN_DataSend[queue->Front].ID;
    }

    memcpy(tx_data, queue->CAN_DataSend[queue->Front].Data,
           (size_t)tx_message.DLC * sizeof(uint8_t));

    if (HAL_CAN_AddTxMessage(queue->Canx, &tx_message, tx_data, &tx_mailbox) != HAL_OK)
    {
        return false; /* keep the frame in queue, retry next period */
    }

    queue->Front = (uint8_t)(((uint16_t)queue->Front + 1U) % CAN_QUEUESIZE);
    return true;
}

void CAN_Enqueue(CAN_SendQueueType *queue, CAN_RxHeaderTypeDef Rxheader, uint8_t Rxdata[])
{
    if (CAN_Queue_IfFull(queue))
    {
        return;
    }

    queue->CAN_DataSend[queue->Rear].DLC = (Rxheader.DLC > 8U) ? 8U : (uint8_t)Rxheader.DLC;
    queue->CAN_DataSend[queue->Rear].IDE = Rxheader.IDE;
    queue->CAN_DataSend[queue->Rear].ID = (Rxheader.IDE == CAN_ID_STD) ? Rxheader.StdId : Rxheader.ExtId;
    memcpy(queue->CAN_DataSend[queue->Rear].Data, Rxdata,
           (size_t)queue->CAN_DataSend[queue->Rear].DLC * sizeof(uint8_t));

    queue->Rear = (uint8_t)(((uint16_t)queue->Rear + 1U) % CAN_QUEUESIZE);
}

void HeaderPrepare(uint32_t sendCode, uint32_t datalen, CAN_RxHeaderTypeDef *rxheader)
{
    rxheader->IDE = CAN_ID_EXT;
    rxheader->ExtId = sendCode;
    rxheader->DLC = datalen;
}
