/**
 * @file    ZDrive.h
 * @brief   ZDrive J60/Z-Smart motor CAN driver, ported from R2 chassis.
 */
#ifndef ZDRIVE_H
#define ZDRIVE_H

#include <stdbool.h>
#include <stdlib.h>
#include "main.h"
#include "CanQueue.h"
#include "motor_config.h"
#include "MathFunc.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define USE_ZDRIVE_NUM MOTOR_ZDRIVE_COUNT

#define POU 10000.f
#define POD -10000.f
#define Velocity_Limit 150.f
#define Current_Limit 40.f

    typedef enum
    {
        Zdrive_Disable = 0,              // 0失能
        Zdrive_Current,                  // 1扭矩模式
        Zdrive_Speed,                    // 2速度模式
        Zdrive_Postion,                  // 3位置模式
        Zdrive_Test,                     // 4测试模式
        Zdrive_RVCalibration,            // 5电阻电感校准
        Zdrive_EncoderLineCalibration,   // 6编码器线性补偿
        Zdrive_EncoudeOffsetCalibration, // 7编码器偏移校准
        Zdrive_VKCalibration,            // 8VK校准
        Zdrive_SaveSetting,              // 9保存配置
        Zdrive_EraseSetting,             // 10擦除配置
        Zdrive_ClearErr,                 // 11擦除错误
        Zdrive_Brake                     // 12刹车
    } ZdriveMode;

    typedef enum
    {
        Zdrive_Well = 0,
        Zdrive_InsufficientVoltage,
        Zdrive_OverVoltage,
        Zdrive_InstabilityCurrent,
        Zdrive_OverCurrent,
        Zdrive_OverSpeed,
        Zdrive_ExcessiveR,
        Zdrive_ExcessiveInductence,
        Zdrive_LoseEncoder1,
        Zdrive_PolesErr,
        Zdrive_VKCalibrationErr,
        Zdrive_ModeErr,
        Zdrive_ParameterErr,
        Zdrive_Hot
    } ZdriveErr;

    typedef enum
    {
        SES = 0x01,
        ENCODER,
        ENL,
        NodeID,
        CAN_HZ,
        HEARTBEAT,
        Volta_LL,
        Pos_UL,
        Pos_LL,
        Vel_Limit,
        Poles_Num,
        CurLimit,
        CurrCAL,
        Start_Mode,  // 启动模式
        Answer_Mode, // 响应模式: 0:无反馈立即执行, 1:有反馈立即执行, 2:无反馈队列执行, 3:有反馈队列执行
        FilterCoeff,
        ToleranceCoeff,
        Pos_PID_P, // 位置环 P,[0...20]
        Pos_PID_D, // 位置环 D,[0...20]
        Vel_PID_P, // 速度环 P,[0...100]
        Vel_PID_I, // 速度环 I,[0...100]
        Acc_Acu,
        Acc_Dec,
        Pos_Vel_TimeGap,
        Mode = 0x1F,
        Warning,
        Err,
        CurIn,
        VelIn,
        PosIn,
        LIF,
        Cur_M = 0x2B,
        Vel = 0x2D, // 当前速度
        Pur = 0x2E, // 当前位置,可以设置
        MIT_Frame = 0x38,
        PVT_Frame = 0x39,
    } ZdriveCmd;

    typedef struct
    {
        float speed_rpm;   /* 速度(rpm) */
        float pos_deg;     /* 位置(deg) */
        float posIn_deg;   /* 当前位置(角度,deg,Pur 命令码) */
        float current_A;   /* 电流(A) */
        float accAcu_rps2; /* 加速度(rps²) */
        float accDec_rps2; /* 减速度(rps²) */
    } ZdriveValue;

    typedef struct
    {
        float velLimit_rpm;      /* 速度限制(rpm) */
        float curLimit_A;        /* 电流限制(A) */
        float posLimit_up_deg;   /* 位置限制(deg) */
        float posLimit_down_deg; /* 位置限制(deg) */
    } ZdriveLimit;

    typedef struct
    {
        float GearRatio;
        float ReductionRatio;
        uint8_t zdrive_id; /* 电机 ID,1..14 */
        float kpPos;       /* 位置环 P */
        float kdPos;       /* 位置环 D */
        float kpVel;       /* 速度环 P */
        float kiVel;       /* 速度环 I */
    } ZdriveParam;

    typedef struct
    {
        bool PVTflag;
        bool PVTinitflag;
        float deltaT;
        uint8_t answer_mode; /* 响应模式: 0:无反馈立即执行, 1:有反馈立即执行, 2:无反馈队列执行, 3:有反馈队列执行 */
    } ZdrivePVTParam;

    typedef struct
    {
        volatile bool Begin; /* 初始化完成标志:false 时 Func 跳过该电机,由任务层置 true */
        ZdriveMode mode;     /* 目标模式,任务层写;Disable 即停止 */
        ZdriveMode modeRead; /* 驱动确认的当前模式,由 RX 更新 */
        ZdriveValue valSetNow;
        ZdriveValue valReal;
        ZdrivePVTParam pvtparam;
        ZdriveErr err;
        ZdriveValue valSetPre;
        ZdriveValue valPre;
        ZdriveParam param;
        ZdriveLimit limit;
    } Zdrive;

#if USE_ZMDR
    extern Zdrive Zmotor[USE_ZDRIVE_NUM];

    void ZdriveInit(void);
    void ZdriveFunc(void);
    void ZdriveReceive(CAN_RxHeaderTypeDef Rxheader, uint8_t *Rx_data, uint8_t bus);
    void ZdriveDequeue(uint8_t bus);
    bool Zdrive_IsOurs(const CAN_RxHeaderTypeDef *Rxheader, uint8_t bus);

    void ZdriveSet(float data, uint8_t id, uint8_t set_code);
    void ZdriveAsk(uint8_t id, uint8_t ask_code);

    // 使用 PVT/ MIT 轨迹帧时,请注意:
    // 1. PVT/ MIT 帧不走 ZdriveSet,请直接调用 ZdriveSetPVT/ ZdriveSetMIT
    // 2. FUNC函数没有对 PVT/ MIT 帧做很好的适配,使用的时候需要自行调用set函数
    // 3. 函数的单位以协议为准,如速度为rps,位置为n.但是单位这一块还有问题,要使用请自行调整

    /** PVT 轨迹帧:一次下发速度+位置,8 字节双值,不走 ZdriveSet */
    void ZdriveSetPVT(float speed, float angle, uint8_t id);
    /** MIT 轨迹帧:一次下发速度+位置+电流+位置环/速度环 PID,8 字节多值,不走 ZdriveSet */
    void ZdriveSetMIT(float speed, float angle, float current, float pos_kp, float vel_kp, uint8_t id);
    /** 覆盖指定电机的 param,位置环/速度环 PID 哪一项改动就下发哪一项命令 */
    void ZdriveParamConfig(uint8_t id, ZdriveParam param);
#endif /* USE_ZMDR */

#ifdef __cplusplus
}
#endif

#endif /* ZDRIVE_H */
