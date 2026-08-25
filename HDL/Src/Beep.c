#include "Beep.h"
#include "myostasks.h"

void Beep_Init(void)
{
    // todo
    BEEP_ON();
    HAL_Delay(200);
    BEEP_OFF();
    HAL_Delay(200);
    BEEP_ON();
    HAL_Delay(50);
    BEEP_OFF();
    HAL_Delay(50);
    BEEP_ON();
    HAL_Delay(50);
    BEEP_OFF();
    HAL_Delay(50);
}


void Beep_Alarm(uint8_t times)
{
    BeepAlarmTimes = times;
}