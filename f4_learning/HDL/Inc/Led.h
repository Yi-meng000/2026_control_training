/*
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-07 20:21:55
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-08-08 11:32:46
 * @FilePath: \f4\f4_learning\HDL\Inc\LED.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef LED_H
#define LED_H

/* -------------------------Include------------------------- */
#include "gpio.h"
#include "main.h"

/* -------------------------Define------------------------- */

#define LED_ON(x) HAL_GPIO_WritePin(LED##x##_GPIO_Port, LED##x##_Pin, GPIO_PIN_SET)
#define LED_OFF(x) HAL_GPIO_WritePin(LED##x##_GPIO_Port, LED##x##_Pin, GPIO_PIN_RESET)
#define LED_TOGGLE(x) HAL_GPIO_TogglePin(LED##x##_GPIO_Port, LED##x##_Pin)

/* -------------------------Function------------------------- */

void Led_Water(void);

#endif