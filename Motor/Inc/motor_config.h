/*
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-25 15:40:28
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-08-25 15:45:12
 * @FilePath: \f4_show\Motor\Inc\motor_config.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/**
 * @file    motor_config.h
 * @brief   电机清单与 CAN 总线分配,电机开关也在这里。
 *
 * 参考工程(H7)的总线约定,本工程 F4 仅 CAN1/CAN2(无 CAN3):
 *   - CAN2 总线:VESC + ZDrive
 *   - CAN1 总线:DJI M2006/M3508(或预留给用户通信)
 */
#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* ------------------------------------------------------------------ */
/* 电机驱动开关:1 = 编译并使用,0 = 不编译                               */
/* ------------------------------------------------------------------ */
#define USE_DJ 1
#define USE_VESC 0 // 未验证,不要启用
#define USE_ZMDR 1

/* ------------------------------------------------------------------ */
/* DJI M2006 / M3508                                                    */
/* ------------------------------------------------------------------ */
#define MOTOR_DJI_COUNT 4U  /* 必须为 4 或 8(DJI CAN 打包要求) */
#define MOTOR_DJI_CAN_BUS 0 /* 0=CAN1,1=CAN2 */

#define MOTOR_M2006_COUNT 4U
#define MOTOR_M3508_COUNT 0U
#define MOTOR_M2006_REDUCTION_RATIO 36U
#define MOTOR_M3508_REDUCTION_RATIO 19.20320855f

/* ------------------------------------------------------------------ */
/* VESC                                                                 */
/* ------------------------------------------------------------------ */
#define MOTOR_VESC_COUNT 4U
#define MOTOR_VESC_CAN_BUS 1 /* CAN2                 */
#define MOTOR_VESC_POLE_PAIRS 7U

/* ------------------------------------------------------------------ */
/* ZDrive                                                              */
/* 总线拆分:SPLIT_COUNT = 0 → 全部走第一路 CAN;= n → ID 1..n 走第一路,  */
/* ID n+1..COUNT 走第二路。                                              */
/* NOTE: ZDrive 的帧 ID = motor_id | (op_code<<4),低 4 位 ID 空间 1..N   */
/* 与 DJI 反馈 ID 0x201..0x204 的低 4 位重叠,Zdrive_IsOurs 无法区分,     */
/* 原则上两者不同总线。                                                */
/* ------------------------------------------------------------------ */
#define MOTOR_ZDRIVE_COUNT 6U       /* 最多控 8 个电机 */
#define MOTOR_ZDRIVE_SPLIT_COUNT 4U /* 0=不拆分;n=前 n 个 ID 走第一路 */
#define MOTOR_ZDRIVE_CAN_BUS_1 1U   /* 第一路:CAN2 */
#define MOTOR_ZDRIVE_CAN_BUS_2 1U   /* 第二路:CAN2(F4 无 CAN3) */
#define MOTOR_ZDRIVE_BUS_RETRANS_CNT 2 //调用出队函数时,单BUS连续发送的次数


#ifdef __cplusplus
}
#endif

#endif /* MOTOR_CONFIG_H */
