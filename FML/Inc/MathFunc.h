#pragma once
#ifndef __MATHFUNC_H__
#define __MATHFUNC_H__

#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include <math.h>

/**
 ******************************************************************************
 * ├─ stdbool.h
 * ├─ stdint.h
 * ├─ string.h
 * ├─ stdlib.h
 * ├─ math.h
 ******************************************************************************
 */

#define __I volatile const /*!< Defines 'read only' permissions */

#define __O volatile  /*!< Defines 'write only' permissions */
#define __IO volatile /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define __IM volatile const /*! Defines 'read only' structure member permissions */
#define __OM volatile       /*! Defines 'write only' structure member permissions */
#define __IOM volatile      /*! Defines 'read / write' structure member permissions */

typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef const int32_t sc32;
typedef const int16_t sc16;
typedef const int8_t sc8;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef const uint32_t uc32;
typedef const uint16_t uc16;
typedef const uint8_t uc8;

typedef __IO int32_t vs32;
typedef __IO int16_t vs16;
typedef __IO int8_t vs8;

typedef __I int32_t vsc32;
typedef __I int16_t vsc16;
typedef __I int8_t vsc8;

typedef __IO uint32_t vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t vu8;

typedef __I uint32_t vuc32;
typedef __I uint16_t vuc16;
typedef __I uint8_t vuc8;

#define Pi 3.14159265358979f
#define PI 3.14159265358979f

#define __RAM_D1_ __attribute__((section(".RAM_D1")))
#define __RAM_D2_ __attribute__((section(".RAM_D2")))
#define __RAM_D3_ __attribute__((section(".RAM_D3")))

#define ALIGN_32B __attribute__((aligned(32)))

#define ABS(x) ((x) > 0 ? (x) : (-(x)))
#define SIG(x) ((x < 0) ? -1 : 1)
#define GetSign(x) ((x > 0) ? 1 : -1)

#define PEAK(A, B)                                                                                 \
    if (ABS(A) > B)                                                                                \
        A = SIG(A) * B; // 此处默认B为正值
#define PeakLimit(a, b)                                                                            \
    if (ABS(a) > ABS(b))                                                                           \
    a = GetSign(a) * b

#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define square(x) ((x) * (x))

#define EncodeS32Data(f, buff)                                                                     \
    {                                                                                              \
        *(int32_t *)buff = *f;                                                                     \
    }
#define DecodeS32Data(f, buff)                                                                     \
    {                                                                                              \
        *f = *(int32_t *)buff;                                                                     \
    }
#define EncodeS16Data(f, buff)                                                                     \
    {                                                                                              \
        *(s16 *)buff = *f;                                                                         \
    }
#define DecodeS16Data(f, buff)                                                                     \
    {                                                                                              \
        *f = *(s16 *)buff;                                                                         \
    }
#define EncodeU16Data(f, buff)                                                                     \
    {                                                                                              \
        *(u16 *)buff = *f;                                                                         \
    }
#define DecodeU16Data(f, buff)                                                                     \
    {                                                                                              \
        *f = *(u16 *)buff;                                                                         \
    }

typedef struct {
    int Trajectory_Begin; // 轨迹开始标志位

    float UPDATE_PERIOD_S;   // 更新周期 s
    float Target_Total_Time; // 总运动时间 s

    float True_Total_Time; // 实际规划的总时间 s
    float Direction;       // 运动方向 1或-1
    float Abs_Distance;
    float Initial_Position; // 初始位置
    float Current_Time;     // 当前时间
} Trajectory_Planning;

void ChangeDataByte(uint8_t *p1, uint8_t *p2);

float buffer_32_to_float(const uint8_t *buffer, float scale, int32_t *index);
float buffer_16_to_float(const uint8_t *buffer, float scale, int32_t *index);
void buffer_s16_to_u8(u8 *buffer, s16 source);
s16 buffer_u8_to_s16(u8 *source, u8 *index);
void buffer_float_to_u8(u8 *buffer, float source);
float buffer_u8_to_float(u8 *source, u8 *index);
void buffer_int_to_u8(u8 *buffer, int source);
int buffer_u8_to_int(u8 *source, u8 *index);
void buffer_s32_to_u8(u8 *buffer, s32 source);
s32 buffer_u8_to_s32(u8 *source, u8 *index);

void buffer_append_int32(uint8_t *buffer, int32_t source, int32_t *index);

int32_t get_s32_from_buffer(const uint8_t *buffer, int32_t *index);
int16_t get_s16_from_buffer(const uint8_t *buffer, int32_t *index);

void int16_to_bytes(int16_t data, uint8_t *buff, int index);
void int32_to_bytes(int16_t data, uint8_t *buff, int index);
int16_t bytes_to_int16(uint8_t *buff, int index);
int32_t bytes_to_int32(uint8_t *buff, int index);
float bytes_to_float(const uint8_t *data);

double cvtFloat2Double(float n1, float n2);

float uint2float(int x_int, float x_min, float x_max, int bits);
u16 float2uint(float x, float x_min, float x_max, uint8_t bits);

float Lerp(float start, float end, float t);
float N2DEG(float N);

static inline float DEG2RAD(float angle);
static inline float RAD2DEG(float angle);
// #define DEG2RAD (PI / 180)
// #define RAD2DEG (180 / PI)

void Rotate(float *x, float *y, float x0, float y0, float a);

s16 MSG_Byte2Int16(uint8_t *buff, uint8_t i);
int MSG_Byte2Int32(uint8_t *buff, uint8_t i);

void MSG_Int162Byte(s16 data, uint8_t *buff, uint8_t i);
void MSG_Int322Byte(int data, uint8_t *buff, uint8_t i);
void MSG_Float2Byte(float data, uint8_t *buff, uint8_t i);

void PVT_Calculate_Timing(float distance, float max_vel, float max_acc, float *t_accel,
                          float *t_const, float *t_decel, float *peak_vel);
float PVT_Calculate_Position(float t, float t_accel, float t_const, float initial_pos,
                             float direction, float max_acc, float peak_vel, float dist_accel);

void Bezier5_Calculate_Timing(float distance, float total_time, float *t_total, float *direction,
                              float *abs_distance);
float Bezier5_Calculate_Position(float t, float t_total, float initial_pos, float direction,
                                 float abs_distance);
float Bezier5_Calculate_Velocity(float t, float t_total, float direction, float abs_distance);
float Bezier5_Calculate_Acceleration(float t, float t_total, float direction, float abs_distance);

float normalize_yaw_f(float angle);

static inline float Clamp(float value, float min, float max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}
static inline float ClampPeak(float value, float abs_max)
{
    if (value > abs_max)
        return abs_max;
    else if (value < -abs_max)
        return -abs_max;
    else
        return value;
}

static inline float DEG2N(float deg)
{
    return deg / 360.0f;
}

#endif /* __MATHFUNC_H__ */
