
#include "focConf.h"
#include "lowpass_filter.h"

/******************************************************************************/
/******************************************************************************/
void LPF_init(LowPassFilter *LPF, float freq) {
  LPF->Tf = freq;
  LPF->timestamp_prev = 0;
  LPF->y_prev = 0;
}
/******************************************************************************/
float LPFoperator(LowPassFilter *LPF, float x) {
  uint32_t now_us;
  float dt, alpha, y;

  now_us = MiniFoc_micros();
  if (now_us < LPF->timestamp_prev)dt = (float)(LPF->timestamp_prev - now_us) / 9 * 1e-6f;
  else
    dt = (float)(0xFFFFFF - now_us + LPF->timestamp_prev) / 9 * 1e-6f;
  LPF->timestamp_prev = now_us;
  if (dt > 0.3)   //时间过长，大概是程序刚启动初始化，直接返回
  {
    LPF->y_prev = x;
    return x;
  }

  alpha = LPF->Tf / (LPF->Tf + dt);
  y = alpha * LPF->y_prev + (1.0f - alpha) * x;
  LPF->y_prev = y;

  return y;
}
/******************************************************************************/


