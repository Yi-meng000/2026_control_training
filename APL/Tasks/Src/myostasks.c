#include "myostasks.h"

uint8_t BeepAlarmTimes = 0;

void LedWaterTask(void *argument)
{
  for(;;)
  {
    Led_Water();
  }
}


void BeepAlarmTask(void *argument)
{
  for(;;)
  {
    uint8_t i;
    for(i = 0; i < BeepAlarmTimes; i++)
    {
        BEEP_ON();
        osDelay(40);
        BEEP_OFF();
        osDelay(40);
    }
    osDelay(1);
  }
}
