/*
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-11 10:06:00
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-08-25 16:53:33
 * @FilePath: \f4_show\IRQ\Src\TIM_IRQHandler.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "TIM_IRQHandler.h"
#include "motor_config.h"


void TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2){
        #if USE_ZMDR
            ZdriveDequeue((uint8_t)MOTOR_ZDRIVE_CAN_BUS_1);
            ZdriveDequeue((uint8_t)MOTOR_ZDRIVE_CAN_BUS_2);
        #endif        

        #if USE_DJ
            DJmotor_Func();
        #endif
        static uint8_t Func_cnt = 0;
        if (++Func_cnt >= 5)
        {
            Func_cnt = 0;
            #if USE_VESC
                VescFunc();
            #endif
            #if USE_ZMDR
                ZdriveFunc();
            #endif
        }
    }
    if(htim->Instance == TIM3){

    }

}
