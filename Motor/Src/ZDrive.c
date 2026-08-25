/**
 * @file    ZDrive.c
 * @brief   ZDrive J60/Z-Smart motor driver, ported from R2 chassis.
 */
#include "ZDrive.h"

#if USE_ZMDR

Zdrive Zmotor[USE_ZDRIVE_NUM] = {0};

// 判断 ID(1-based)的电机是否挂在 bus(0=CAN1,1=CAN2)上
static inline bool Zdrive_IdOnBus(uint32_t id, uint8_t bus)
{
    uint8_t motor_index = (uint8_t)(id - 1U);

    uint8_t cfg_bus = (MOTOR_ZDRIVE_SPLIT_COUNT == 0U) ||
                              (motor_index < MOTOR_ZDRIVE_SPLIT_COUNT)
                          ? (uint8_t)MOTOR_ZDRIVE_CAN_BUS_1
                          : (uint8_t)MOTOR_ZDRIVE_CAN_BUS_2;
    return cfg_bus == bus;
}

// 获取电机的发送队列,根据电机 ID(0-based)判断挂在哪一路 CAN 上
static CAN_SendQueueType *Zdrive_GetTxQueue(uint32_t motor_index)
{
    uint8_t bus = (MOTOR_ZDRIVE_SPLIT_COUNT == 0U) ||
                          (motor_index < MOTOR_ZDRIVE_SPLIT_COUNT)
                      ? (uint8_t)MOTOR_ZDRIVE_CAN_BUS_1
                      : (uint8_t)MOTOR_ZDRIVE_CAN_BUS_2;
    if (bus == 0U)
    {
        return &CAN1_Txqueue;
    }
    return &CAN2_Txqueue;
}

// 拆分是否生效(第二路总线上有电机)
static inline bool Zdrive_SplitActive(void)
{
    return (MOTOR_ZDRIVE_SPLIT_COUNT > 0U) &&
           (MOTOR_ZDRIVE_SPLIT_COUNT < MOTOR_ZDRIVE_COUNT) &&
           (MOTOR_ZDRIVE_CAN_BUS_2 != MOTOR_ZDRIVE_CAN_BUS_1);
}

static const uint8_t s_empty_data[1] = {0};

// 入队:按帧 ID 解析所属总线队列(ID 1..8 各自解析,0xFU 广播两路都发);
// 满队列置 CanFullFlag 并丢弃,行为与直接写入一致
static void ZdriveEnqueue(uint32_t id, uint8_t dlc, const uint8_t *data)
{
    CAN_RxHeaderTypeDef header;
    CAN_SendQueueType *queues[2];
    uint8_t queue_cnt;

    header.StdId = id;
    header.IDE = CAN_ID_STD;
    header.DLC = dlc;

    if ((id & 0xFU) == 0xFU)
    {
        queues[0] = Zdrive_GetTxQueue(0);
        queue_cnt = 1U;
        if (Zdrive_SplitActive())
        {
            queues[1] = Zdrive_GetTxQueue(MOTOR_ZDRIVE_SPLIT_COUNT);
            queue_cnt = 2U;
        }
    }
    else
    {
        queues[0] = Zdrive_GetTxQueue((id & 0xFU) - 1U);
        queue_cnt = 1U;
    }

    for (uint8_t k = 0U; k < queue_cnt; k++)
    {
        if (CAN_Queue_IfFull(queues[k]))
        {
            continue;
        }
        CAN_Enqueue(queues[k], header, (uint8_t *)data);
    }
}

/* 1kHz 恒定出队:TIM2 中断里按总线调用,每条总线每 tick 发 2 帧。
   命名与 ZdriveEnqueue / ZdriveReceive 对齐,发送节奏归驱动所有。 */
void ZdriveDequeue(uint8_t bus)
{
    CAN_SendQueueType *queue = (bus == 0U) ? &CAN1_Txqueue : &CAN2_Txqueue;
    for (uint8_t i = 0; i < MOTOR_ZDRIVE_BUS_RETRANS_CNT; i++)
    {
        CAN_DequeueTx(queue);
    }
}

void ZdriveInit(void)
{
    for (uint32_t i = 0; i < USE_ZDRIVE_NUM; i++)
    {
        Zmotor[i].param.GearRatio = 1.0f;
        Zmotor[i].param.ReductionRatio = 1.0f;
        Zmotor[i].param.zdrive_id = (uint8_t)(i + 1U);
        Zmotor[i].valSetPre.pos_deg = 0.f;
        Zmotor[i].valSetNow.speed_rpm = 0.0f;
        Zmotor[i].valSetNow.pos_deg = 0.0f;
        Zmotor[i].valSetNow.current_A = 0.0f;
        Zmotor[i].valReal.pos_deg = 0.0f;
        Zmotor[i].param.kpPos = 1.2f;
        Zmotor[i].param.kdPos = 0.08f;
        Zmotor[i].param.kpVel = 1.8f;
        Zmotor[i].param.kiVel = 0.2f;
        Zmotor[i].pvtparam.deltaT = 0.002f; // pvt默认点控间隔
        Zmotor[i].pvtparam.answer_mode = 2U; // 默认响应模式: 2:无反馈队列执行
        Zmotor[i].mode = Zdrive_Disable;    /* 初始 setmode 为 disable,上电即失能 */
        Zmotor[i].modeRead = Zdrive_Disable;
        Zmotor[i].err = Zdrive_Well; /* 上电无错误 */
        Zmotor[i].Begin = false;     /* 初始化完成后由任务层置 true */
    }
    ZdriveAsk(0xFU, Mode); /* 读取所有电机的模式 */
    ZdriveAsk(0xFU, Pos_PID_P);
    ZdriveAsk(0xFU, Pos_PID_D);
    ZdriveAsk(0xFU, Vel_PID_P);
    ZdriveAsk(0xFU, Vel_PID_I);
}

/* 统一 set:按 set_code 完成单位换算、帧编码、读回确认后再入队。
   - 4 字节 float:绝大多数命令(PosIn/VelIn/Pur/Mode/Acc_Acu/Acc_Dec...)
   - 配置类(Vel_Limit/Acc_Acu/Acc_Dec)设置后自动 Ask 读回确认
   - PVT(速度+位置双值)是独立帧,见 ZdriveSetPVT */
void ZdriveSet(float data, uint8_t id, uint8_t set_code)
{
    uint8_t frame[8] = {0};
    uint8_t dlc = 4U;

    if (id == 0U)
    {
        id = 0xFU; /* broadcast address */
    }
    else if (id > USE_ZDRIVE_NUM)
    {
        return;
    }

    /* 单位换算:位置类 DEG→N,速度类 rpm→rps(输出端,除减速比) */
    if ((set_code == PosIn) || (set_code == Pur))
    {
        data = DEG2N(data) * Zmotor[id - 1U].param.ReductionRatio;
    }
    else if ((set_code == VelIn) || (set_code == Vel_Limit))
    {
        data /= (60.0f / Zmotor[id - 1U].param.ReductionRatio);
    }

    memcpy(frame, &data, sizeof(float));

    ZdriveEnqueue(id | ((uint32_t)set_code << 4U), dlc, frame);
}

void ZdriveAsk(uint8_t id, uint8_t ask_code)
{
    if (id == 0U)
    {
        id = 0xFU;
    }

    ZdriveEnqueue(id | ((uint32_t)ask_code << 4U), 0U, s_empty_data);
}

/* 帧过滤:标准帧 + 低位 ID 1..N + 该 ID 按配置挂在这条总线上。
   CAN 接收中断先用它区分 ZDrive 反馈帧与同总线其他设备帧
   (如 DJI 反馈 0x201..0x204 不在 ZDrive 总线则放行)。 */
bool Zdrive_IsOurs(const CAN_RxHeaderTypeDef *Rxheader, uint8_t bus)
{
    uint32_t control_id;

    if (Rxheader->IDE != CAN_ID_STD)
    {
        return false;
    }
    control_id = (uint32_t)(Rxheader->StdId & 0xFU);
    if ((control_id < 1U) || (control_id > USE_ZDRIVE_NUM))
    {
        return false;
    }
    return Zdrive_IdOnBus(control_id, bus);
}

void ZdriveReceive(CAN_RxHeaderTypeDef Rxheader, uint8_t *Rx_Data, uint8_t bus)
{
    uint32_t control_id = (uint32_t)(Rxheader.StdId & 0xFU);
    uint32_t operation_id = Rxheader.StdId >> 4U;
    float tmp_pos = 0.0f;
    int16_t tmp_vel = 0;
    int16_t tmp_cur = 0;

    /* 非本驱动的帧直接丢弃(过滤规则见 Zdrive_IsOurs) */
    if (!Zdrive_IsOurs(&Rxheader, bus))
    {
        return;
    }
    uint32_t motor_index = control_id - 1U;

    if (Rxheader.DLC == 4U)
    {
        switch (operation_id)
        {
        case Pur:
            Zmotor[motor_index].valPre.pos_deg = Zmotor[motor_index].valReal.pos_deg;
            memcpy(&Zmotor[motor_index].valReal.pos_deg, Rx_Data, sizeof(float));
            Zmotor[motor_index].valReal.pos_deg = N2DEG(Zmotor[motor_index].valReal.pos_deg) /
                                                  Zmotor[motor_index].param.ReductionRatio;
            break;
        case Cur_M:
            memcpy(&Zmotor[motor_index].valReal.current_A, Rx_Data, sizeof(float));
            break;
        case Vel:
            memcpy(&Zmotor[motor_index].valReal.speed_rpm, Rx_Data, sizeof(float));
            Zmotor[motor_index].valReal.speed_rpm *= (60.0f / Zmotor[motor_index].param.ReductionRatio);
            break;
        case Mode:
        {
            float temp_mode = 0.0f;
            memcpy(&temp_mode, Rx_Data, sizeof(float));
            Zmotor[motor_index].modeRead = (ZdriveMode)(int32_t)temp_mode;
            break;
        }
        case Err:
        {
            float temp_err = 0.0f;
            memcpy(&temp_err, Rx_Data, sizeof(float));
            Zmotor[motor_index].err = (ZdriveErr)(int32_t)temp_err;
            break;
        }
        case PosIn:
        {
            float temp_pos_in = 0.0f;
            memcpy(&temp_pos_in, Rx_Data, sizeof(float));
            Zmotor[motor_index].valReal.posIn_deg = N2DEG(temp_pos_in) /
                                                    Zmotor[motor_index].param.ReductionRatio;
            break;
        }
        case Vel_Limit:
            memcpy(&Zmotor[motor_index].limit.velLimit_rpm, Rx_Data, sizeof(float));
            break;

        case Acc_Acu:
            memcpy(&Zmotor[motor_index].valReal.accAcu_rps2, Rx_Data, sizeof(float));
            break;

        case Acc_Dec:
            memcpy(&Zmotor[motor_index].valReal.accDec_rps2, Rx_Data, sizeof(float));
            break;

        default:
            break;
        }
    }
    else if (Rxheader.DLC == 8U)
    {
        memcpy(&tmp_pos, Rx_Data, sizeof(float));

        Zmotor[motor_index].valPre.pos_deg = Zmotor[motor_index].valReal.pos_deg;
        Zmotor[motor_index].valReal.pos_deg = ((tmp_pos) / (float)0xffffffffU *
                                                   (POU - POD) +
                                               POD) /
                                              Zmotor[motor_index].param.ReductionRatio;

        memcpy(&tmp_vel, Rx_Data + 4U, sizeof(int16_t));
        Zmotor[motor_index].valReal.speed_rpm = (float)tmp_vel / (float)0xffffU *
                                                    (2.0f * Velocity_Limit) -
                                                Velocity_Limit;

        memcpy(&tmp_cur, Rx_Data + 6U, sizeof(int16_t));
        Zmotor[motor_index].valReal.current_A = (float)tmp_cur / (float)0xffffU *
                                                    (2.0f * Current_Limit) -
                                                Current_Limit;
    }
}

void ZdriveSetPVT(float speed, float angle, uint8_t id)
{
    uint8_t data[8] = {0};

    if ((id == 0U) || (id > USE_ZDRIVE_NUM))
    {
        return;
    }
    uint32_t vel_mod_u32 = (uint32_t)((speed + Velocity_Limit) /
                                      (2.0f * Velocity_Limit) * (float)0xffffffffU);
    uint32_t pos_mod_u32 = (uint32_t)(((angle - POD) / (POU - POD)) *
                                      (float)0xffffffffU);

    memcpy(data, &vel_mod_u32, sizeof(uint32_t));
    memcpy(data + 4U, &pos_mod_u32, sizeof(uint32_t));

    ZdriveEnqueue(id | ((uint32_t)PVT_Frame << 4U), 8U, data);
}

void ZdriveSetMIT(float speed, float angle, float current, float pos_kp, float vel_kp, uint8_t id)
{
    uint8_t data[8] = {0};

    if ((id == 0U) || (id > USE_ZDRIVE_NUM))
    {
        return;
    }

    int16_t pos_mod_i16 = (int32_t)(((angle - POD) / (POU - POD)) *
                                    (float)0xffffU);

    int16_t vel_mod_i12 = (int32_t)((speed + Velocity_Limit) /
                                    (2.0f * Velocity_Limit) * (float)0xfffU);
    int16_t cur_mod_i12 = (int32_t)((current + Current_Limit) /
                                    (2.0f * Current_Limit) * (float)0xfffU);
    int16_t pos_kp_mod_i12 = (int32_t)(pos_kp / 50.0f * (float)0xfffU);
    int16_t vel_kp_mod_i12 = (int32_t)(vel_kp / 50.0f * (float)0xfffU);

    data[0] = (uint8_t)(pos_mod_i16 & 0xFFU);
    data[1] = (uint8_t)((pos_mod_i16 >> 8U) & 0xFFU);
    data[2] = (uint8_t)(vel_mod_i12 & 0xFFU);
    data[3] = (uint8_t)((vel_mod_i12 >> 8U) & 0x0FU) | ((cur_mod_i12 & 0x0FU) << 4U);
    data[4] = (uint8_t)((cur_mod_i12 >> 4U) & 0xFFU);
    data[5] = (uint8_t)(pos_kp_mod_i12 & 0xFFU);
    data[6] = (uint8_t)((pos_kp_mod_i12 >> 8U) & 0x0FU) | ((vel_kp_mod_i12 & 0x0FU) << 4U);
    data[7] = (uint8_t)((vel_kp_mod_i12 >> 4U) & 0xFFU);

    ZdriveEnqueue(id | ((uint32_t)MIT_Frame << 4U), 8U, data);
}

void ZdriveParamConfig(uint8_t id, ZdriveParam param)
{
    ZdriveParam *p;

    if ((id == 0U) || (id > USE_ZDRIVE_NUM))
    {
        return;
    }

    p = &Zmotor[id - 1U].param;

    /* 哪一项 PID 有改动就下发哪一项 */
    if (param.kpPos != p->kpPos)
    {
        Clamp(param.kpPos, 0.0f, 20.0f);
        ZdriveSet(param.kpPos, id, Pos_PID_P);
    }
    if (param.kdPos != p->kdPos)
    {
        Clamp(param.kdPos, 0.0f, 20.0f);
        ZdriveSet(param.kdPos, id, Pos_PID_D);
    }
    if (param.kpVel != p->kpVel)
    {
        Clamp(param.kpVel, 0.0f, 100.0f);
        ZdriveSet(param.kpVel, id, Vel_PID_P);
    }
    if (param.kiVel != p->kiVel)
    {
        Clamp(param.kiVel, 0.0f, 100.0f);
        ZdriveSet(param.kiVel, id, Vel_PID_I);
    }

    *p = param;
}

/* ---- Switch 状态机:判断条件 modeRead != mode ----
   驱动尚未确认目标模式,持续下发 Mode 命令并查询,直至 read == set 进入运行。
   各模式可在切换期间预处理设定值(如 Speed 清零、Position 对齐当前 posIn),
   确认前不执行周期指令。 */
// 可以放小巧思
static void Zdrive_SwitchMachine(Zdrive *motor, uint8_t id)
{
    switch (motor->mode)
    {
    case Zdrive_Disable:
    case Zdrive_Current:
        break;
    case Zdrive_Speed:
        motor->valSetNow.speed_rpm = 0.f;
        motor->valSetPre.speed_rpm = 0.f;
        break;
    case Zdrive_Postion:
        motor->valSetNow.pos_deg = motor->valReal.pos_deg;
        motor->valSetPre.pos_deg = motor->valReal.pos_deg;
        break;
    case Zdrive_SaveSetting:
        motor->Begin = false; /* 保存配置后由任务层重新初始化 */
        break;
    case Zdrive_ClearErr:
        motor->err = Zdrive_Well; /* 清除错误后本地也清除 */
        break;
    default:
        break;
    }
    ZdriveSet((float)motor->mode, id, Mode);
    ZdriveAsk(id, Mode);
}

/* ---- 运行状态机:判断条件 modeRead == mode,按模式执行周期指令 ---- */
static void Zdrive_RunMachine(Zdrive *motor, uint8_t id)
{
    switch (motor->mode)
    {
    case Zdrive_Speed:
        if (fabs(motor->valSetNow.speed_rpm - motor->valSetPre.speed_rpm) > 0.1f)
        {
            ZdriveSet(motor->valSetNow.speed_rpm, id, VelIn);
            motor->valSetPre.speed_rpm = motor->valSetNow.speed_rpm;
        }
        break;

    case Zdrive_Current:
        // 建议自行把控调用频率,如果要使用Func任务层周期调用,建议 1kHz 或更低,否则可能出现电流环抖动
        // ZdriveSetMIT(motor->valSetNow.speed_rpm, motor->valSetNow.pos_deg, motor->valSetNow.current_A,
        //              motor->param.kpPos, motor->param.kpVel, id);
        break;

    case Zdrive_Postion:
        if (!motor->pvtparam.PVTflag)
        {
            if (fabs(motor->valSetNow.pos_deg - motor->valSetPre.pos_deg) > 0.01f)
            {
                motor->valSetPre.pos_deg = motor->valSetNow.pos_deg;
                ZdriveSet(motor->valSetNow.pos_deg, id, PosIn);
            }
            else if (motor->valSetNow.pos_deg == 0.0f &&
                     fabs(motor->valReal.posIn_deg) > 0.5f)
            {
                ZdriveSet(motor->valSetNow.pos_deg, id, PosIn);
            }
        }
        else
        {
            if (motor->pvtparam.PVTinitflag)
            {
                ZdriveSet(motor->pvtparam.deltaT, id, Pos_Vel_TimeGap); // 设置时间间隔
                ZdriveSet(motor->pvtparam.answer_mode, id, Answer_Mode); // 设置响应模式

                motor->pvtparam.PVTinitflag = false;
            }
            // 建议自行把控调用频率
            // ZdriveSetPVT(motor->valSetNow.speed_rpm, motor->valSetNow.pos_deg, id);
        }
        break;

    case Zdrive_Disable:
    default:
        motor->mode = Zdrive_Disable;
        break;
    }
}

// 错误处理:err != Well 时在此处理,内容暂空白(错误码定义见 ZdriveErr)
static void Zdrive_ErrHandle(Zdrive *motor)
{
    if (motor->err == Zdrive_Well)
    {
        return;
    }

    // TODO: 按错误码处理,如强制 mode = Zdrive_Disable 等
}

void ZdriveFunc(void)
{
    uint32_t i;

    for (i = 0; i < USE_ZDRIVE_NUM; i++)
    {

        // 错误处理
        Zdrive_ErrHandle(&Zmotor[i]);

        // Begin == false:初始化未完成,跳过该电机
        if (!Zmotor[i].Begin)
        {
            Zmotor[i].mode = Zdrive_Disable;
            if (Zmotor[i].modeRead != Zmotor[i].mode)
            {
                Zdrive_SwitchMachine(&Zmotor[i], Zmotor[i].param.zdrive_id);
            }
            continue;
        }
        if (Zmotor[i].modeRead != Zmotor[i].mode)
        {
            Zdrive_SwitchMachine(&Zmotor[i], Zmotor[i].param.zdrive_id);
            continue;
        }

        // 运行状态机
        Zdrive_RunMachine(&Zmotor[i], Zmotor[i].param.zdrive_id);
    }

    // 任何时候都执行的 ask:广播查询反馈,入队时自动覆盖两条总线
    // 可以在PVT和MIT这种有专门反馈帧的模式下不查询
    ZdriveAsk(0, Pur);
    // ZdriveAsk(0, PosIn);
    ZdriveAsk(0, Vel);
}
#endif /* USE_ZMDR */
