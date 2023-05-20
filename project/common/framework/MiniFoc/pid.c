

#include "focConf.h"
#include "pid.h"
#include "foc_utils.h"

/******************************************************************************/
//所有PID参数，对于云台电机适当大一点，对于航模电机适当小一点
void PID_init(PIDController *pid, float p, float i, float d, float outputLimit, float outputRamp) {
  pid->P = p;
  pid->I = i;
  pid->D = d;
  pid->output_ramp = outputRamp;
  pid->limit = outputLimit;
  pid->error_prev = 0;
  pid->output_prev = 0;
  pid->integral_prev = 0;
  pid->timestamp_prev = 0;
}
/******************************************************************************/
// PID controller function
float PIDoperator(PIDController *PID, float error) {
  uint32_t now_us;
  float Ts;
  float proportional, integral, derivative, output;
  float output_rate;

  now_us = MiniFoc_micros();
  if (now_us < PID->timestamp_prev)Ts = (float)(PID->timestamp_prev - now_us) / 9 * 1e-6f;
  else
    Ts = (float)(0xFFFFFF - now_us + PID->timestamp_prev) / 9 * 1e-6f;
  PID->timestamp_prev = now_us;
  if (Ts == 0 || Ts > 0.5) Ts = 1e-3f;

  // u(s) = (P + I/s + Ds)e(s)
  // Discrete implementations
  // proportional part
  // u_p  = P *e(k)
  proportional = PID->P * error;
  // Tustin transform of the integral part
  // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
  integral = PID->integral_prev + PID->I * Ts * 0.5f * (error + PID->error_prev);
  // antiwindup - limit the driver
  integral = _constrain(integral, -PID->limit, PID->limit);
  // Discrete derivation
  // u_dk = D(ek - ek_1)/Ts
  derivative = PID->D * (error - PID->error_prev) / Ts;

  // sum all the components
  output = proportional + integral + derivative;
  // antiwindup - limit the driver variable
  output = _constrain(output, -PID->limit, PID->limit);

  // if driver ramp defined
  if (PID->output_ramp > 0) {
    // limit the acceleration by ramping the driver
    output_rate = (output - PID->output_prev) / Ts;
    if (output_rate > PID->output_ramp)output = PID->output_prev + PID->output_ramp * Ts;
    else if (output_rate < -PID->output_ramp)output = PID->output_prev - PID->output_ramp * Ts;
  }

  // saving for the next pass
  PID->integral_prev = integral;
  PID->output_prev = output;
  PID->error_prev = error;

  return output;
}
/******************************************************************************/

