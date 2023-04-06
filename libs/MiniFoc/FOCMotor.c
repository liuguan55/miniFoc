
#include "focConf.h"
#include "MiniFoc.h"

/******************************************************************************/
// shaft angle calculation
float shaftAngle(float angle,float sensor_offset,  Direction sensor_direction)
{
  // if no sensor linked return previous value ( for open loop )

  return sensor_direction * angle - sensor_offset;
}

// shaft velocity calculation
float shaftVelocity(LowPassFilter *LPF_velocity, float velocity,  Direction sensor_direction)
{
  // if no sensor linked return previous value ( for open loop )
  return sensor_direction * LPFoperator(LPF_velocity,velocity);
}

/******************************************************************************/
float electricalAngle(float shaft_angle, int pole_pairs, float sensor_offset, float zero_electric_angle)
{
  return _normalizeAngle((shaft_angle + sensor_offset) * pole_pairs - zero_electric_angle);
}
/******************************************************************************/


