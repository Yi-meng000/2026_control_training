/*
 * @Author: Frt001 2067314783@qq.com
 * @Date: 2026-08-11 08:59:46
 * @LastEditors: Frt001 2067314783@qq.com
 * @LastEditTime: 2026-08-24 16:10:18
 * @FilePath: \f4_show\HDL\Src\Led.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "Led.h"
#include "cmsis_os2.h"

void Led_Water(void)
{
    // todo
    LED_ON(1);
    osDelay(200);
    LED_OFF(1);
    osDelay(200);
    LED_ON(2);
    osDelay(200);
    LED_OFF(2);
    osDelay(200);
    LED_ON(3);
    osDelay(200);
    LED_OFF(3);
    osDelay(200);
    LED_ON(4);
    osDelay(200);
    LED_OFF(4);
    osDelay(200);

}
