#ifndef __CANQUEUE_H
#define __CANQUEUE_H

#include <stdbool.h>
#include "main.h"
#include "can.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define CAN_QUEUESIZE 12U

    typedef struct
    {
        uint32_t ID;
        uint8_t DLC;
        uint32_t IDE;
        uint8_t Data[8];
        bool InConGrpFlag;
    } CAN_DataStruct;

    typedef struct
    {
        uint8_t Front;
        uint8_t Rear;
        CAN_HandleTypeDef *Canx;
        CAN_DataStruct CAN_DataSend[CAN_QUEUESIZE];
    } CAN_SendQueueType;

    extern CAN_SendQueueType CAN1_Txqueue;
    extern CAN_SendQueueType CAN2_Txqueue;

    void CAN_InitSendQueue(void);
    bool CAN_Queue_IfEmpty(CAN_SendQueueType *queue);
    bool CAN_Queue_IfFull(CAN_SendQueueType *queue);
    bool CAN_DequeueTx(CAN_SendQueueType *queue);
    void CAN_Enqueue(CAN_SendQueueType *queue, CAN_RxHeaderTypeDef Rxheader, uint8_t Rxdata[]);
    void HeaderPrepare(uint32_t sendCode, uint32_t datalen, CAN_RxHeaderTypeDef *rxheader);

#ifdef __cplusplus
}
#endif


#endif
