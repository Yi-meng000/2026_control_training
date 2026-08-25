/**
 * @file    DJmotor.c
 * @brief   DJI M2006/M3508 CAN motor driver.
 *
 * Ported from R2_Chassis-chassis_main/Motor/src/DJmotor.c.  The control flow
 * and PID structure are kept, only the bus selection and a few safety details
 * were generalized for this template.
 */
#include "DJmotor.h"

#if USE_DJ

DJMotor DJmotor[USE_DJNUM];

/* 按 MOTOR_DJI_CAN_BUS 取总线句柄(0=CAN1,1=CAN2) */
static inline CAN_HandleTypeDef *DJmotor_GetCanHandle(void)
{
    switch (MOTOR_DJI_CAN_BUS)
    {
    case 0:
        return &hcan1;
    case 1:
        return &hcan2;
    default:
        return 0;
    }
}

static inline float Get_Total_Ratio(DJMotorPointer motor)
{
    return motor->param.Gear_ratio * motor->param.Reduction_ratio;
}
#if M3508_NUM > 0
static inline bool is_M3508(uint8_t ID)
{
    return (ID > M2006_NUM);
}
#else
static inline bool is_M3508(uint8_t ID)
{
    return false;
}
#endif
void DJmotor_Init(void)
{
    DJmotorParam dj2006_param;
    DJmotorParam dj3508_param;
    DJmotorLimit limit;
    DJmotorStatus statusFlag;
    DJmotorArgum argum;
    DJmotorError error;

    dj2006_param.ParamID = 0x1ffU;
    dj2006_param.Gear_ratio = 1.0f;
    dj2006_param.Reduction_ratio = M2006_RATIO;
    dj2006_param.PulsePerRound = 8191U;
    dj2006_param.CurrentLimit_raw = 4500;

    dj3508_param.ParamID = 0x200U;
    dj3508_param.Gear_ratio = 1.0f;
    dj3508_param.Reduction_ratio = M3508_RATIO;
    dj3508_param.PulsePerRound = 8191U;
    dj3508_param.CurrentLimit_raw = 10000;

    limit.CurrentLimitFlag = true;
    limit.IsLooseStuck = false;

    limit.MaxAngle_deg = 270.0f;
    limit.MinAngle_deg = -270.0f;
    limit.PosAngleLimitFlag = false;
    limit.PosRPMFlag = true;
    limit.PosRPMLimit = 430;

    limit.RPMLimitFlag = false;
    limit.SpeedRPMLimit = 400;
    limit.ZeroCurrentLimit_raw = 3000;
    limit.ZeroRPMLimit = 50;

    statusFlag.IsSetZero = true;
    statusFlag.Overtimeflag = false;
    statusFlag.StuckFlag = false;
    statusFlag.ZeroFlag = false;

    argum.pulseLock = 0;
    argum.zeroCnt = 0;
    argum.GapCnt = 0;

    error.lastRxTime = 0;
    error.stuckCount = 0;
    error.timeoutCount = 0;

    for (uint32_t i = 0; i < USE_DJNUM; i++)
    {
        DJmotor[i].Begin = false;
        DJmotor[i].MODE_Set = DJ_Disable; /* 上电失能:发 0 电流 */
        DJmotor[i].statusFlag = statusFlag;
        DJmotor[i].limit = limit;
        DJmotor[i].argum = argum;
        DJmotor[i].error = error;
        DJmotor[i].valSet.current_raw = 0;
        DJmotor[i].valSet.angle_deg = 0.0f;
        DJmotor[i].valSet.speed_rpm = 0;
        DJmotor[i].valSet.PulseTotal = 0;
        DJmotor[i].valNow.PulseTotal = 0;
        DJmotor[i].valPre.PulseRead = 0;
    }

    // 这里可以使用表封装的参数进行替换赋值
    for (uint32_t i = 0; i < M2006_NUM; i++)
    {
        DJmotor[i].ID = (uint8_t)(i + 1U);
        DJmotor[i].param = dj2006_param;
    }

    for (uint32_t i = 0; i < M3508_NUM; i++)
    {
        DJmotor[i + M2006_NUM].ID = (uint8_t)(i + M2006_NUM + 1U);
        DJmotor[i + M2006_NUM].param = dj3508_param;
    }

    for (uint32_t i = 0; i < USE_DJNUM; i++)
    {
        PID_Init(&DJmotor[i].posPID, 0.07f, 0.0005f, 0.0f, PIDPOS);
        PID_Init(&DJmotor[i].velPID, 5.5f, 0.3f, 0.01f, PIDINC);
    }
}

void DJmotor_PID_Reload(DJMotorPointer motor, DJmotorPID pid_reload)
{
    PID_Init(&motor->posPID, pid_reload.posPID.KP, pid_reload.posPID.KI, pid_reload.posPID.KD, pid_reload.posPID.mode);
    PID_Init(&motor->velPID, pid_reload.velPID.KP, pid_reload.velPID.KI, pid_reload.velPID.KD, pid_reload.velPID.mode);
}

void DJmotor_SetZero(DJMotorPointer motor)
{
    motor->statusFlag.IsSetZero = false;
    motor->valNow.angle_deg = 0.0f;
    motor->valNow.PulseTotal = 0;
    motor->argum.pulseLock = 0;
}

void DJmotor_AngleCalculate(DJMotorPointer motor)
{
    motor->valNow.PulseGap = (int16_t)(motor->valNow.PulseRead - motor->valPre.PulseRead);

    if (ABS(motor->valNow.PulseGap) > 4096)
    {
        motor->valNow.PulseGap = (int16_t)(motor->valNow.PulseGap -
                                           GetSign(motor->valNow.PulseGap) *
                                               (int32_t)motor->param.PulsePerRound);
    }

    motor->valNow.PulseTotal += motor->valNow.PulseGap;
    motor->valNow.angle_deg = (float)motor->valNow.PulseTotal * 360.0f /
                              ((float)motor->param.PulsePerRound * Get_Total_Ratio(motor));

    if (motor->Begin) // 废弃字段
    {
        motor->argum.pulseLock = motor->valNow.PulseTotal;
    }

    if (motor->statusFlag.IsSetZero)
    {
        DJmotor_SetZero(motor);
        motor->statusFlag.IsSetZero = false;
    }

    motor->valPre = motor->valNow;
}

void DJmotor_Receive(CAN_RxHeaderTypeDef Rxheader, uint8_t *Rx_data)
{
    if ((Rxheader.IDE != CAN_ID_STD) ||
        (Rxheader.RTR != CAN_RTR_DATA) ||
        (Rxheader.StdId < 0x201U) || (Rxheader.StdId > 0x208U))
    {
        return;
    }

    uint8_t card_id = (uint8_t)(Rxheader.StdId - 0x200U); /* 1..8 */

    /* Init 保证 ID = 索引 + 1,直接索引免循环查找 */
    if (card_id > USE_DJNUM)
    {
        return;
    }

    DJMotorPointer motor = &DJmotor[card_id - 1U];

    motor->valNow.PulseRead = (int16_t)(((uint16_t)Rx_data[0] << 8) | Rx_data[1]);
    int16_t speed_raw = (int16_t)(((uint16_t)Rx_data[2] << 8) | Rx_data[3]);
    motor->valNow.current_raw = (int16_t)(((uint16_t)Rx_data[4] << 8) | Rx_data[5]);

    if (is_M3508(motor->ID))
    {
        motor->valNow.temperature_C = (int8_t)Rx_data[6];
        motor->valNow.current_A = (float)motor->valNow.current_raw * 0.0012207f;
    }
    else
    {
        motor->valNow.current_A = (float)motor->valNow.current_raw / 10000.0f * 10.0f;
    }

    motor->valNow.speed_rpm = (float)speed_raw / Get_Total_Ratio(motor);

    motor->error.lastRxTime = 0;
    DJmotor_AngleCalculate(motor);
}

void DJmotor_CurrentTransmit(DJMotorPointer motor)
{
    static uint8_t tx_data[8] = {0};
    CAN_TxHeaderTypeDef tx_header = {0};
    uint32_t tx_mailbox = 0;
    uint8_t tag = 0;

    /* 电流限幅由各模式函数负责,此处只打包发送 */
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8U;
    tx_header.TransmitGlobalTime = DISABLE;

    if (motor->ID <= 4U)
    {
        tx_header.StdId = 0x200U;
        tag = (uint8_t)((motor->ID - 1U) * 2U);
    }
    else
    {
        tx_header.StdId = 0x1FFU;
        tag = (uint8_t)((motor->ID - 5U) * 2U);
    }

    EncodeS16Data(&motor->valSet.current_raw, &tx_data[tag]);
    ChangeDataByte(&tx_data[tag], &tx_data[tag + 1U]);

    if (motor->ID == 4U || motor->ID == 8U)
    {
        HAL_CAN_AddTxMessage(DJmotor_GetCanHandle(), &tx_header, tx_data, &tx_mailbox);
    }
}

void DJmotor_SpeedMode(DJMotorPointer motor)
{
    if (motor->limit.RPMLimitFlag)
    {
        motor->valSet.speed_rpm = ClampPeak(motor->velPID.SetVal, motor->limit.SpeedRPMLimit);
    }
    motor->velPID.SetVal = (float)motor->valSet.speed_rpm * Get_Total_Ratio(motor);
    motor->velPID.CurVal = (float)motor->valNow.speed_rpm * Get_Total_Ratio(motor);
    motor->valSet.current_raw += PID_Calculate(&motor->velPID);
    motor->valSet.current_raw = (int16_t)ClampPeak(motor->valSet.current_raw, motor->param.CurrentLimit_raw);
}

void DJmotor_PositionMode(DJMotorPointer motor)
{

    if (motor->limit.PosAngleLimitFlag)
    {
        motor->valSet.angle_deg = Clamp(motor->valSet.angle_deg, motor->limit.MinAngle_deg, motor->limit.MaxAngle_deg);
    }

    motor->valSet.PulseTotal = (int32_t)(motor->valSet.angle_deg * Get_Total_Ratio(motor) * (float)motor->param.PulsePerRound / 360.0f);
    motor->posPID.SetVal = (float)motor->valSet.PulseTotal;
    motor->posPID.CurVal = (float)motor->valNow.PulseTotal;

    motor->velPID.SetVal = PID_Calculate(&motor->posPID);
    if (motor->limit.PosRPMFlag)
    {
        motor->velPID.SetVal = ClampPeak(motor->velPID.SetVal, motor->limit.PosRPMLimit * Get_Total_Ratio(motor));
    }
    motor->velPID.CurVal = (float)motor->valNow.speed_rpm * Get_Total_Ratio(motor);

    motor->valSet.current_raw += PID_Calculate(&motor->velPID);
    motor->valSet.current_raw = (int16_t)ClampPeak(motor->valSet.current_raw, motor->param.CurrentLimit_raw);
}

void DJmotor_ZeroMode(DJMotorPointer motor)
{
    motor->velPID.SetVal = (float)motor->limit.ZeroRPMLimit * Get_Total_Ratio(motor);
    motor->velPID.CurVal = (float)motor->valNow.speed_rpm * Get_Total_Ratio(motor);
    motor->valSet.current_raw += PID_Calculate(&motor->velPID);
    motor->valSet.current_raw = (int16_t)ClampPeak(motor->valSet.current_raw, motor->limit.ZeroCurrentLimit_raw);

    if (ABS(motor->valNow.PulseGap) < Zero_Distance)
    {
        if (motor->argum.zeroCnt++ > 100U)
        {
            motor->argum.zeroCnt = 0;
            motor->statusFlag.ZeroFlag = true;
            motor->Begin = false;
            /* 寻零结束不走 SwitchMode,这里手动清 PID 历史,重新使能时从零起步 */
            PID_Reset(&motor->posPID);
            PID_Reset(&motor->velPID);
            DJmotor_SetZero(motor);
        }
    }
}

static void DJmotor_Monitor(DJMotorPointer motor)
{

    if (motor->valNow.PulseGap < 5 && motor->valNow.current_raw > 3000)
    {
        if (motor->error.stuckCount++ > 500U)
        {
            motor->error.stuckCount = 0;
            motor->statusFlag.StuckFlag = true;
            if (motor->limit.IsLooseStuck)
            {
                motor->MODE_Set = DJ_Disable;
            }
        }
    }
    else
    {
        motor->error.stuckCount = 0;
    }

    if (motor->error.lastRxTime++ > 50U)
    {
        if (motor->error.timeoutCount++ > 20U)
        {
            motor->error.timeoutCount = 0;
            motor->MODE_Set = DJ_Disable;
            motor->statusFlag.Overtimeflag = true;
        }
    }
}

static void DJmotor_SwitchMode(DJMotorPointer motor)
{
    if (motor->MODE_Set != motor->MODE_Cur)
    {
        motor->MODE_Cur = motor->MODE_Set;
        motor->valSet.current_raw = 0;
        motor->valSet.speed_rpm = 0;
        motor->valSet.angle_deg = motor->valNow.angle_deg;
        /* 清误差历史与位置环累加的目标速度(velPID.SetVal),避免残留值冲击新模式 */
        PID_Reset(&motor->posPID);
        PID_Reset(&motor->velPID);
        motor->statusFlag.ZeroFlag = false;
        motor->statusFlag.Overtimeflag = false;
        motor->statusFlag.StuckFlag = false;
    }
}

void DJmotor_Func(void)
{
    for (uint32_t i = 0; i < USE_DJNUM; i++)
    {

        if (DJmotor[i].Begin)
        {
            // DJmotor_Monitor(&DJmotor[i]);
            DJmotor_SwitchMode(&DJmotor[i]);

            switch (DJmotor[i].MODE_Cur)
            {
            case DJ_Disable:
                DJmotor[i].valSet.current_raw = 0;
                DJmotor_CurrentTransmit(&DJmotor[i]);
                continue;
                break;
            case DJ_RPM:
                DJmotor_SpeedMode(&DJmotor[i]);
                break;
            case DJ_Position:
                DJmotor_PositionMode(&DJmotor[i]);
                break;
            case DJ_Zero:
                DJmotor_ZeroMode(&DJmotor[i]);
                break;
            case DJ_Current:
                /* 直通电流:任务层每周期写 valSet.current_raw,这里补限幅 */
                ClampPeak(DJmotor[i].valSet.current_raw, DJmotor[i].param.CurrentLimit_raw);
                break;
            default:
                break;
            }
        }
        else
        {
            /* Begin=false(未初始化/寻零完成):强制 0 电流,防止残留累加电流持续输出 */
            DJmotor[i].valSet.current_raw = 0;
        }

        DJmotor_CurrentTransmit(&DJmotor[i]);
    }
}
#endif /* USE_DJ */
