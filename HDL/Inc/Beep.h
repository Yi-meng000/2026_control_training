/*
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-11 08:59:52
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-09-10 15:35:39
 * @FilePath: \f4_show\HDL\Inc\Beep.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef BEEP_H
#define BEEP_H

/*----------------------- include ---------------------*/
#include "gpio.h"
#include "main.h"

/*----------------------- Define ---------------------*/
#define BEEP_ON() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET)
#define BEEP_OFF() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET)

/*----------------------- Function ---------------------*/
void Beep_Init(void);
void Beep_Alarm(uint8_t times);

#endif