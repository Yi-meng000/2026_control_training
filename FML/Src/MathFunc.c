#include "MathFunc.h"

/**
 * @brief 将p1和p2进行互换，在地址上操作
 *
 * @param p1
 * @param p2
 */
void ChangeDataByte(uint8_t *p1, uint8_t *p2) {
    uint8_t t;
    t = *p1;
    *p1 = *p2;
    *p2 = t;
}

/**
 * @brief 将32位转为float 同时改变index 除以scale
 *
 * @param buffer
 * @param scale
 * @param index
 * @return float
 */
float buffer_32_to_float(const uint8_t *buffer, float scale, int32_t *index) {
    return (float)get_s32_from_buffer(buffer, index) / scale;
}

/**
 * @brief 将16位转为float
 *
 * @param buffer
 * @param scale
 * @param index
 * @return float
 */
float buffer_16_to_float(const uint8_t *buffer, float scale, int32_t *index) {
    return (float)get_s16_from_buffer(buffer, index) / scale;
}

void buffer_s16_to_u8(u8 *buffer, s16 source) {
    *buffer = (source >> 8) & 0xff;
    *(buffer + 1) = source & 0xff;
}

s16 buffer_u8_to_s16(u8 *source, u8 *index) {
    *index += 2;
    return (s16)((*source << 8) | (*(source + 1)));
}
void buffer_float_to_u8(u8 *buffer, float source) {
    *(buffer) = ((s32)source >> 24) & 0xff;
    *(buffer + 1) = ((s32)source >> 16) & 0xff;
    *(buffer + 2) = ((s32)source >> 8) & 0xff;
    *(buffer + 3) = ((s32)source) & 0xff;
}
float buffer_u8_to_float(u8 *source, u8 *index) {
    *index += 4;
    return (float)((s32)((*source << 24) | (*(source + 1) << 16) | (*(source + 2) << 8)
                         | (*(source + 3))));
}
void buffer_int_to_u8(u8 *buffer, int source) {
    *(buffer) = (source >> 24) & 0xff;
    *(buffer + 1) = (source >> 16) & 0xff;
    *(buffer + 2) = (source >> 8) & 0xff;
    *(buffer + 3) = (source) & 0xff;
}
int buffer_u8_to_int(u8 *source, u8 *index) {
    *index += 4;
    return (int)((s32)((*source << 24) | (*(source + 1) << 16) | (*(source + 2) << 8)
                       | (*(source + 3))));
}

void buffer_s32_to_u8(u8 *buffer, s32 source) {
    *(buffer) = (source >> 24) & 0xff;
    *(buffer + 1) = (source >> 16) & 0xff;
    *(buffer + 2) = (source >> 8) & 0xff;
    *(buffer + 3) = (source) & 0xff;
}
s32 buffer_u8_to_s32(u8 *source, u8 *index) {
    *index += 4;
    return (s32)((u32)((*source << 24) | (*(source + 1) << 16) | (*(source + 2) << 8)
                       | (*(source + 3))));
}
/**
 * @brief Get the s32 from buffer object 将4个8位合成32位 同时改变index
 *
 * @param buffer
 * @param index
 * @return int32_t
 */
int32_t get_s32_from_buffer(const uint8_t *buffer, int32_t *index) {
    int32_t res = (((uint32_t)buffer[*index]) << 24) | (((uint32_t)buffer[*index + 1]) << 16)
                  | (((uint32_t)buffer[*index + 2]) << 8) | (((uint32_t)buffer[*index + 3]));
    *index += 4;
    return res;
}

/**
 * @brief Get the s16 from buffer object 将两个8位合成16位 同时改变index
 *
 * @param buffer
 * @param index
 * @return int16_t
 */
int16_t get_s16_from_buffer(const uint8_t *buffer, int32_t *index) {
    int16_t res = (((uint32_t)buffer[*index]) << 8) | (((uint32_t)buffer[*index + 1]));
    *index += 2;
    return res;
}

void int16_to_bytes(int16_t data, uint8_t *buff, int index) {
    buff[index] = (uint8_t)(data & 0xff);
    buff[index + 1] = (uint8_t)((data >> 8) & 0xff);
}
void int32_to_bytes(int16_t data, uint8_t *buff, int index) {
    buff[index] = (uint8_t)(data & 0xff);
    buff[index + 1] = (uint8_t)((data >> 8) & 0xff);
    buff[index + 2] = (uint8_t)((data >> 16) & 0xff);
    buff[index + 3] = (uint8_t)((data >> 24) & 0xff);
}
int16_t bytes_to_int16(uint8_t *buff, int index) {
    int16_t data = ((uint32_t)buff[index]) | ((uint32_t)buff[index + 1] << 8);
    return data;
}
int32_t bytes_to_int32(uint8_t *buff, int index) {
    int32_t data = (int32_t)(((buff[index + 3]) << 24) | ((buff[index + 2]) << 16)
                             | ((buff[index + 1]) << 8) | ((buff[index])));
    return data;
}

float bytes_to_float(const uint8_t *data) {
    float value;
    memcpy(&value, &data[4], sizeof(float));
    return value;
}

/**
 * @brief 将一个32位转为4个8位
 *
 * @param buffer
 * @param source
 * @param index
 */
void buffer_append_int32(uint8_t *buffer, int32_t source, int32_t *index) {
    buffer[(*index)++] = source >> 24;
    buffer[(*index)++] = source >> 16;
    buffer[(*index)++] = source >> 8;
    buffer[(*index)++] = source;
}

/**
 * @brief 将两个float转化为double
 *
 * @param n1
 * @param n2
 * @return double
 */
double cvtFloat2Double(float n1, float n2) {
    struct {
        float n1;
        float n2;
    } s;
    s.n1 = n1;
    s.n2 = n2;
    return *((double *)&s);
}

/**
 * @brief 用于关节电机的数据转化
 *
 * @param x_int
 * @param x_min
 * @param x_max
 * @param bits
 * @return float
 */
float uint2float(int x_int, float x_min, float x_max, int bits) {
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}

u16 float2uint(float x, float x_min, float x_max, uint8_t bits) {
    float span = x_max - x_min;
    float offset = x_min;

    return (u16)((x - offset) * ((float)((1 << bits) - 1)) / span);
}
float Lerp(float start, float end, float t) {
    if (t > 1)
        t = 1;
    else if (t < 0)
        t = 0;
    return (end - start) * t + start;
}

float N2DEG(float N) {
    return N * 360;
}
static inline float DEG2RAD(float angle) {
    return angle / 180.f * PI;
}

static inline float RAD2DEG(float angle) {
    return angle / PI * 180.f;
}

/**
 * @brief 旋转度 将诸如贝塞尔曲线上取点进行旋转变换
 *
 * @param x 结果 x
 * @param y 结果 y
 * @param x0
 * @param y0
 * @param a 旋转度 度为单位
 */
void Rotate(float *x, float *y, float x0, float y0, float a) {
    float x1 = *x;
    float y1 = *y;
    float rad = a * PI / 180.f;
    *x = (x1 - x0) * cosf(rad) - (y1 - y0) * sinf(rad) + x0;
    *y = (y1 - y0) * cosf(rad) + (x1 - x0) * sinf(rad) + y0;
}
/**
 * @brief 将字节数组转换为16位整数
 * @param buff 字节数组
 * @param i 读取的起始索引
 * @return 转换后的16位整数
 */
s16 MSG_Byte2Int16(uint8_t *buff, uint8_t i) {
    s16 data = (s16)(buff[i + 1] << 8 | buff[i]);
    return data;
}
int MSG_Byte2Int32(uint8_t *buff, uint8_t i) {
    int data = (int)(buff[i + 3] << 24 | buff[i + 2] << 16 | buff[i + 1] << 8 | buff[i]);
    return data;
}
/**
 * @brief 将16位整数转换为字节数组
 * @param data 待转换的16位整数
 * @param buff 字节数组
 * @param i 写入的起始索引
 */
void MSG_Int162Byte(s16 data, uint8_t *buff, uint8_t i) {
    buff[i] = (uint8_t)(data & 0xff);
    buff[i + 1] = (uint8_t)(data >> 8);
}
void MSG_Int322Byte(int data, uint8_t *buff, uint8_t i) {
    buff[i] = (uint8_t)(data & 0xff);
    buff[i + 1] = (uint8_t)((int)data >> 8 & 0xff);
    buff[i + 2] = (uint8_t)((int)data >> 16 & 0xff);
    buff[i + 3] = (uint8_t)((int)data >> 24);
}
/**
 * @brief 将浮点数转换为字节数组
 * @param data 待转换的浮点数
 * @param buff 字节数组
 * @param i 写入的起始索引
 */
void MSG_Float2Byte(float data, uint8_t *buff, uint8_t i) {
    union {
        float fltdata;
        uint32_t u32data;
    } F2B;

    F2B.fltdata = data;

    buff[i] = (uint8_t)(F2B.u32data & 0xFF);
    buff[i + 1] = (uint8_t)((F2B.u32data >> 8) & 0xFF);
    buff[i + 2] = (uint8_t)((F2B.u32data >> 16) & 0xFF);
    buff[i + 3] = (uint8_t)((F2B.u32data >> 24) & 0xFF);
}

/**
 * @brief  计算梯形速度规划（Trapezoidal Velocity Profile）的时间参数
 * @note   该函数不依赖特定硬件，可通用于任何电机的位置规划。
 *         它根据目标距离计算出加速、匀速和减速三个阶段所需的时间。
 *         自动判断是生成完整的梯形曲线，还是退化为三角形曲线（距离太短无法达到最大速度）。
 *
 * @param  distance  目标位移距离的绝对值或相对值（内部会取绝对值运算）
 *                   (单位: 用户自定义单位，如 rad, deg, meter，需与速度加速度单位统一)
 * @param  max_vel   允许的最大运行速度 (单位: distance/s，必须 > 0)
 * @param  max_acc   允许的最大加速度 (单位: distance/s^2，必须 > 0)
 * @param  t_accel   [输出] 加速阶段所需时间 (s)
 * @param  t_const   [输出] 匀速阶段所需时间 (s)
 * @param  t_decel   [输出] 减速阶段所需时间 (s)，当前算法设定加减速对称，故等于 t_accel
 * @param  peak_vel  [输出] 实际规划能达到的峰值速度 (s)
 */
void PVT_Calculate_Timing(float distance, float max_vel, float max_acc, float *t_accel,
                          float *t_const, float *t_decel, float *peak_vel) {
    // 参数有效性校验（防止除零错误）
    if (max_acc <= 0.0f || max_vel <= 0.0f) {
        *t_accel = 0.0f;
        *t_const = 0.0f;
        *t_decel = 0.0f;
        *peak_vel = 0.0f;
        return;
    }

    float abs_distance = fabsf(distance);
    float t_ramp = max_vel / max_acc;
    float dist_ramp = 0.5f * max_acc * t_ramp * t_ramp;

    if (abs_distance < 2.0f * dist_ramp) // 三角形轨迹 (距离太短，达不到最大速度)
    {
        *t_accel = sqrtf(abs_distance / max_acc);
        *t_decel = *t_accel;
        *t_const = 0.0f;
        *peak_vel = max_acc * (*t_accel);
    } else if (abs_distance >= 2.0f * dist_ramp) // 梯形轨迹
    {
        *t_accel = t_ramp;
        *t_decel = t_ramp;
        *peak_vel = max_vel;
        float dist_const = abs_distance - 2.0f * dist_ramp;
        *t_const = dist_const / max_vel;
    }
}

/**
 * @brief  根据当前时间 t 计算梯形规划曲线上的理论位置
 * @note   该函数为无状态函数，输入时间 t 返回对应的位置。
 *         需要在外部循环中维护一个从 0 开始累计的时间变量 t。
 *
 * @param  t            当前时间 (从运动开始起算的相对时间, s)
 * @param  t_accel      加速总时间 (由 Calculate_Timing 计算得出)
 * @param  t_const      匀速总时间 (由 Calculate_Timing 计算得出)
 * @param  initial_pos  起始位置
 * @param  direction    运动方向符号 (1.0f 或 -1.0f)
 * @param  max_acc      最大加速度
 * @param  peak_vel     实际峰值速度
 * @param  dist_accel   加速阶段走过的距离 (通常为 0.5 * max_acc * t_accel * t_accel)
 * @return float        当前时刻 t 的理论目标位置
 */
float PVT_Calculate_Position(float t, float t_accel, float t_const, float initial_pos,
                             float direction, float max_acc, float peak_vel, float dist_accel) {
    float t_total = t_accel + t_const + t_accel; // t_decel = t_accel

    if (t <= 0.0f) {
        return initial_pos;
    } else if (t >= t_total) // 添加终点保护，防止时间溢出导致位置超调
    {
        float total_distance = 2.0f * dist_accel + peak_vel * t_const;
        return initial_pos + direction * total_distance;
    } else if (t <= t_accel) // 加速阶段: x = x0 + 0.5*a*t^2
    {
        return initial_pos + direction * (0.5f * max_acc * t * t);
    } else if (t <= t_accel + t_const) // 匀速阶段: x = x_acc_end + v*(t-t_acc)
    {
        float t_in_const = t - t_accel;
        return initial_pos + direction * (dist_accel + peak_vel * t_in_const);
    } else // 减速阶段: x = x_const_end + v*t' - 0.5*a*t'^2
    {
        float t_in_decel = t - (t_accel + t_const);
        float pos_at_decel_start = initial_pos + direction * (dist_accel + peak_vel * t_const);
        return pos_at_decel_start
               + direction * (peak_vel * t_in_decel - 0.5f * max_acc * t_in_decel * t_in_decel);
    }
}

/**
 * @brief  计算五次贝塞尔曲线速度规划的参数
 * @note   五次贝塞尔曲线提供了更平滑的速度和加速度过渡，消除了加加速度(jerk)的突变
 *         曲线在起点和终点的速度和加速度都为0，中间平滑过渡
 *
 * @param  distance     目标位移距离（可正可负，符号表示方向）
 * @param  total_time   期望的总运动时间 (s)，必须 > 0
 * @param  t_total      [输出] 实际规划的总时间 (s)
 * @param  direction    [输出] 运动方向 (1.0f 或 -1.0f)
 * @param  abs_distance [输出] 距离的绝对值
 */
void Bezier5_Calculate_Timing(float distance, float total_time, float *t_total, float *direction,
                              float *abs_distance) {
    // 参数有效性校验
    if (total_time <= 0.0f) {
        *t_total = 0.0f;
        *direction = 0.0f;
        *abs_distance = 0.0f;
        return;
    }

    *abs_distance = fabsf(distance);
    *direction = (distance >= 0.0f) ? 1.0f : -1.0f;
    *t_total = total_time;
}

/**
 * @brief  根据当前时间 t 计算五次贝塞尔曲线上的理论位置
 * @note   使用五次贝塞尔曲线: B(t) = (1-t)^5*P0 + 5*(1-t)^4*t*P1 + 10*(1-t)^3*t^2*P2
 *                                   + 10*(1-t)^2*t^3*P3 + 5*(1-t)*t^4*P4 + t^5*P5
 *         其中 P0=0, P1=P2=0, P3=P4=1, P5=1，简化后得到平滑的S曲线
 *
 * @param  t            当前时间 (从运动开始起算的相对时间, s)
 * @param  t_total      总运动时间 (s)
 * @param  initial_pos  起始位置
 * @param  direction    运动方向符号 (1.0f 或 -1.0f)
 * @param  abs_distance 移动距离的绝对值
 * @return float        当前时刻 t 的理论目标位置
 */
float Bezier5_Calculate_Position(float t, float t_total, float initial_pos, float direction,
                                 float abs_distance) {
    if (t <= 0.0f) {
        return initial_pos;
    } else if (t >= t_total) {
        return initial_pos + direction * abs_distance;
    } else {
        // 归一化时间参数 [0, 1]
        float tau = t / t_total;

        // 五次贝塞尔曲线，控制点设置为 P0=0, P5=1
        // 使用优化的计算公式：B(tau) = tau^3 * (10 - 15*tau + 6*tau^2)
        float tau2 = tau * tau;
        float tau3 = tau2 * tau;
        float blend = tau3 * (10.0f - 15.0f * tau + 6.0f * tau2);

        return initial_pos + direction * abs_distance * blend;
    }
}

/**
 * @brief  根据当前时间 t 计算五次贝塞尔曲线上的理论速度
 * @note   速度是位置对时间的一阶导数
 *
 * @param  t            当前时间 (s)
 * @param  t_total      总运动时间 (s)
 * @param  direction    运动方向符号
 * @param  abs_distance 移动距离的绝对值
 * @return float        当前时刻 t 的理论速度
 */
float Bezier5_Calculate_Velocity(float t, float t_total, float direction, float abs_distance) {
    if (t <= 0.0f || t >= t_total) {
        return 0.0f;
    }

    float tau = t / t_total;
    float tau2 = tau * tau;

    // 速度：dB/dt = (dB/dtau) * (dtau/dt) = (dB/dtau) / t_total
    // dB/dtau = 30*tau^2 - 60*tau^3 + 30*tau^4
    float d_blend = 30.0f * tau2 * (1.0f - 2.0f * tau + tau2);

    return direction * abs_distance * d_blend / t_total;
}

/**
 * @brief  根据当前时间 t 计算五次贝塞尔曲线上的理论加速度
 * @note   加速度是位置对时间的二阶导数
 *
 * @param  t            当前时间 (s)
 * @param  t_total      总运动时间 (s)
 * @param  direction    运动方向符号
 * @param  abs_distance 移动距离的绝对值
 * @return float        当前时刻 t 的理论加速度
 */
float Bezier5_Calculate_Acceleration(float t, float t_total, float direction, float abs_distance) {
    if (t <= 0.0f || t >= t_total) {
        return 0.0f;
    }

    float tau = t / t_total;

    // 加速度：d²B/dt² = (d²B/dtau²) / t_total²
    // d²B/dtau² = 60*tau - 180*tau^2 + 120*tau^3
    float dd_blend = 60.0f * tau * (1.0f - 3.0f * tau + 2.0f * tau * tau);

    return direction * abs_distance * dd_blend / (t_total * t_total);
}

float normalize_yaw_f(float angle) {
    // 将角度模 360°，得到 (-360°, 360°) 范围内的值
    float result = fmodf(angle, 360.0f);

    // 调整到 [-180°, 180°)
    if (result >= 180.0f) {
        result -= 360.0f; // [180°, 360°) -> [-180°, 0°)
    } else if (result < -180.0f) {
        result += 360.0f; // (-360°, -180°) -> (0°, 180°)
    }

    return result;
}
