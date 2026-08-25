/**
 * @file    pid.h
 * @brief   Scalar incremental/positional PID and 2D vector PID.
 */
#ifndef PID_H
#define PID_H

#include <stdint.h>
#include "MathFunc.h"
#include "Vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Original names are kept because the ported motor drivers use them. */
typedef enum {
    PIDPOS = 0,     /* positional PID  */
    PIDINC = 1      /* incremental PID */
} PidMode_t;

#define PID_POS  PIDPOS
#define PID_INC  PIDINC

typedef struct {
    float KP;
    float KI;
    float KD;
    volatile float SetVal;   /* target   */
    volatile float CurVal;   /* feedback */
    volatile float err[3];
    volatile float output;
    uint8_t mode;
    float integral_limit;
} PIDType;

typedef struct {
    float kp;
    float ki;
    float kd;
    uint8_t mode;
    volatile vector2d input;
    volatile vector2d target;
    volatile vector2d output;
    volatile vector2d err[3];
} Vector2fPID;

/* 调用约定:先写 pid->CurVal(反馈)与 pid->SetVal(目标),再调 PID_Calculate(pid)。
   PID_Reset 清空动态状态(err/output/SetVal/CurVal),保留增益与模式。 */
void      PID_Init(PIDType *pid, float kp, float ki, float kd, uint8_t mode);
void      PID_Reset(PIDType *pid);
float     PID_Calculate(PIDType *pid);

void      vector2fPIDInit(Vector2fPID *pid, float *param, uint8_t mode);
void      vector2fPIDReset(Vector2fPID *pid);
vector2d  vector2fPIDOperation(Vector2fPID *pid);

#ifdef __cplusplus
}
#endif

#endif /* PID_H */
