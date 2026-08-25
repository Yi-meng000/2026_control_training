/**
 * @file    pid.c
 * @brief   Scalar and 2D vector PID implementation.
 */
#include "pid.h"

void PID_Init(PIDType *pid, float kp, float ki, float kd, uint8_t mode)
{
    pid->KP = kp;
    pid->KI = ki;
    pid->KD = kd;
    pid->mode = mode;
    PID_Reset(pid);
}

/* 清空动态状态,保留增益与模式;切模式/重新使能时调用 */
void PID_Reset(PIDType *pid)
{
    pid->SetVal = 0.0f;
    pid->CurVal = 0.0f;
    pid->err[0] = 0.0f;
    pid->err[1] = 0.0f;
    pid->err[2] = 0.0f;
    pid->output = 0.0f;
}

float PID_Calculate(PIDType *pid)
{
    pid->err[0] = pid->SetVal - pid->CurVal;

    switch (pid->mode) {
    case PIDINC:
        pid->output = pid->KP * (pid->err[0] - pid->err[1]) +
                      pid->KI * pid->err[0] +
                      pid->KD * (pid->err[0] - 2.0f * pid->err[1] + pid->err[2]);
        pid->err[2] = pid->err[1];
        pid->err[1] = pid->err[0];
        break;

    case PIDPOS:
        pid->err[2] = 0.5f * pid->err[0] + 0.5f * pid->err[2];
        pid->output = pid->KP * pid->err[0] +
                      pid->KI * pid->err[2] +
                      pid->KD * (pid->err[0] - pid->err[1]);
        pid->err[1] = pid->err[0];
        break;

    default:
        break;
    }

    return pid->output;
}

void vector2fPIDInit(Vector2fPID *pid, float *param, uint8_t mode)
{
    pid->kp = param[0];
    pid->ki = param[1];
    pid->kd = param[2];
    pid->mode = mode;
    vector2fPIDReset(pid);
}

/* 清空动态状态(含 target/input),保留增益与模式 */
void vector2fPIDReset(Vector2fPID *pid)
{
    pid->target.x = 0.0f;
    pid->target.y = 0.0f;
    pid->input.x = 0.0f;
    pid->input.y = 0.0f;
    pid->output.x = 0.0f;
    pid->output.y = 0.0f;

    for (int i = 0; i < 3; i++) {
        pid->err[i].x = 0.0f;
        pid->err[i].y = 0.0f;
    }
}

vector2d vector2fPIDOperation(Vector2fPID *pid)
{
    pid->err[0] = Vector_Minus(pid->target, pid->input);

    switch (pid->mode) {
    case PIDPOS:
        pid->err[2] = Vector_Add(Vector_MultiplyNum(pid->err[2], 0.5f),
                                 Vector_MultiplyNum(pid->err[0], 0.5f));
        if (ABS(pid->err[2].x) > 1000.0f) {
            pid->err[2].x = GetSign(pid->err[2].x) * 1000.0f;
        }
        if (ABS(pid->err[2].y) > 1000.0f) {
            pid->err[2].y = GetSign(pid->err[2].y) * 1000.0f;
        }
        pid->output = Vector_MultiplyNum(pid->err[0], pid->kp);
        pid->output = Vector_Add(pid->output, Vector_MultiplyNum(pid->err[2], pid->ki));
        pid->output = Vector_Add(pid->output, Vector_MultiplyNum(Vector_Minus(pid->err[0], pid->err[1]), pid->kd));
        pid->err[1] = pid->err[0];
        break;

    case PIDINC:
        pid->output = Vector_MultiplyNum(Vector_Minus(pid->err[0], pid->err[1]), pid->kp);
        pid->output = Vector_Add(pid->output, Vector_MultiplyNum(pid->err[0], pid->ki));
        pid->output = Vector_Add(pid->output,
                                 Vector_MultiplyNum(Vector_Minus(Vector_Add(pid->err[0], pid->err[2]),
                                                                 Vector_MultiplyNum(pid->err[1], 2.0f)), pid->kd));
        pid->err[2] = pid->err[1];
        pid->err[1] = pid->err[0];
        break;

    default:
        break;
    }

    return pid->output;
}
