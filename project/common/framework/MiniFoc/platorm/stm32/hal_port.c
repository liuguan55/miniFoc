//
// Created by 邓志君 on 2023/3/5.
//

#include "hal_port.h"

float MiniFoc_getVbus(void)
{
  unsigned short raw_adc = analogRead(ADC_CHANNEL_4, ADC_SAMPLETIME_84CYCLES, 100);
  return (float)raw_adc * 69.3 / 4096;
}
