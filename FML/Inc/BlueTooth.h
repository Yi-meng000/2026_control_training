#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "MathFunc.h"

#include <main.h>
#include "usart.h"

// “接收数据包”内数据的类型与个数
#define RX_BOOL_NUM 18
#define RX_BYTE_NUM 3
#define RX_SHORT_NUM 0
#define RX_INT_NUM 0
#define RX_FLOAT_NUM 7

// “发送数据包”内数据的类型与个数
#define TX_BYTE_NUM 0
#define TX_SHORT_NUM 0
#define TX_INT_NUM 1
#define TX_FLOAT_NUM 10

#define DATA_LENGTH 128

// 蓝牙调试器的前缀和后缀
#define DEBUG_PREFIX 0xA5
#define DEBUG_SUFFIX 0x5A

// 数据包大小，以主控板的收发为主体视角
#define TX_PACK_SIZE (TX_BYTE_NUM + (TX_SHORT_NUM << 1) + (TX_INT_NUM << 2) + (TX_FLOAT_NUM << 2))

#define RX_PACK_SIZE                                                                               \
    (((RX_BOOL_NUM + 7) >> 3) + RX_BYTE_NUM + (RX_SHORT_NUM << 1) + (RX_INT_NUM << 2)              \
     + (RX_FLOAT_NUM << 2))

/*----------------------------------enum-----------------------------------*/

typedef enum {
    Debug_wait,
    Debug_rx,
    Debug_busy,
} DEUBG_STATUS;
// 与蓝牙调试器通信的状态

/*----------------------------------struct-----------------------------------*/

typedef struct {
    u8 RxData[DATA_LENGTH];
    u8 TxData[DATA_LENGTH];

    u8 Prefix;
    u8 Suffix;

    u8 RX_Size;
    u8 TX_Size;

    bool RXsuffix_flag;
    bool TX_flag;
    DEUBG_STATUS Debug_status;
} PACK_MSG; // 蓝牙调试器接收发送数据包

// 接收的数据
typedef struct _rxmsgpack {
#if RX_BOOL_NUM > 0
    u8 bools[RX_BOOL_NUM];
#endif
#if RX_BYTE_NUM > 0
    char bytes[RX_BYTE_NUM];
#endif
#if RX_SHORT_NUM > 0
    short shorts[RX_SHORT_NUM];
#endif
#if RX_INT_NUM > 0
    int ints[RX_INT_NUM];
#endif
#if RX_FLOAT_NUM > 0
    float floats[RX_FLOAT_NUM];
#endif
    char space;
} RX_MSGPACK;

// 发送的数据
typedef struct _txmsgpack {
#if TX_BYTE_NUM > 0
    char bytes[TX_BYTE_NUM];
#endif
#if TX_SHORT_NUM > 0
    short shorts[TX_SHORT_NUM];
#endif
#if TX_INT_NUM > 0
    int ints[TX_INT_NUM];
#endif
#if TX_FLOAT_NUM > 0
    float floats[TX_FLOAT_NUM];
#endif
    char space;
} TX_MSGPACK;

extern RX_MSGPACK RxMsgPack;
extern TX_MSGPACK TxMsgPack;

static bool Debug_Receive(RX_MSGPACK *RxMsgPack);

bool Deal_RxPack(u8 data);
bool Deal_TxPack(TX_MSGPACK *TxMsgPack);
#endif /* __BLUETOOTH_H__ */
