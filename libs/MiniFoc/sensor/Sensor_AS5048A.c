//
// Created by 邓志君 on 2023/3/3.
//

#include "Sensor.h"

#include "../../../framework/MiniCommon.h"


#undef LOG_TAG
#define LOG_TAG "AS5048a"

#define  AS5048a_CPR      1023      //12bit

typedef struct {
  uint32_t cpr;
  uint32_t angle_data;
  int32_t d_angle;
  uint32_t raw_data_prev;
  float full_rotation_offset;
  float angle_prev;
  uint32_t velocity_calc_timestamp;
}AS5048aContext_t;

static AS5048aContext_t AS5048aCtx = {0};

static unsigned short readRawData(SensorInterface_t *interface)
{
  unsigned short  u16Data = 0;

  return u16Data;
}

static void AS5048a_init(struct FocSensor *sensor)
{
  log_i("AS5048a init");

  AS5048aContext_t *ctx = (AS5048aContext_t *)sensor->sensorData;
  if (!ctx){
	return ;
  }

  ctx->cpr = AS5048a_CPR;
  ctx->raw_data_prev = readRawData(sensor->interface);
  ctx->angle_prev =  sensor->getAngle(sensor);
  ctx->full_rotation_offset = 0;
  ctx->velocity_calc_timestamp = 0;
}

static float AS5048a_getAngle(struct FocSensor *senosr)
{
  AS5048aContext_t *ctx = (AS5048aContext_t *)senosr->sensorData;
  if (!ctx){
	return 0.0f;
  }

  ctx->angle_data = readRawData(senosr->interface);
  // tracking the number of rotations
  // in order to expand angle range form [0,2PI] to basically infinity
  ctx->d_angle = ctx->angle_data - ctx->raw_data_prev;
  // if overflow happened track it as full rotation
  if (fabs(ctx->d_angle) > (0.8 * ctx->cpr)){
//	log_d("d_angle %d full_rotation_offset %4.2f angle_data %d",d_angle,full_rotation_offset,angle_data);
	ctx->full_rotation_offset += (ctx->d_angle > 0) ? -_2PI : _2PI;
  }
  // save the current angle value for the next steps
  // in order to know if overflow happened
  ctx->raw_data_prev = ctx->angle_data;
  // return the full angle
  // (number of full rotations)*2PI + current sensor angle
//	if(full_rotation_offset >= ( _2PI*1000))   //转动圈数过多后浮点数精度下降，并导致堵转，每隔一千圈归零一次
//	{
//		full_rotation_offset=0;
//		angle_prev = angle_prev - _2PI*1000;
//	}
//	if(full_rotation_offset <= (-_2PI*1000))
//	{
//		full_rotation_offset=0;
//		angle_prev = angle_prev + _2PI*1000;
//	}

  return (ctx->full_rotation_offset + ((float)ctx->angle_data / ctx->cpr) * _2PI);
}

static float AS5048a_getVelocity(struct FocSensor *sensor)
{
  AS5048aContext_t *ctx = (AS5048aContext_t *)sensor->sensorData;
  if (!ctx){
	return 0.0f;
  }

  uint32_t now_us;
  float Ts, angle_now, vel;

  // calculate sample time
  now_us = MiniFoc_micros(); //_micros();
  if (now_us < ctx->velocity_calc_timestamp){
	Ts = (float)(ctx->velocity_calc_timestamp - now_us) / 9 * 1e-6;
  }else{
	Ts = (float)(0xFFFFFF - now_us + ctx->velocity_calc_timestamp) / 9 * 1e-6;
  }
  // quick fix for strange cases (micros overflow)
  if (Ts == 0 || Ts > 0.5) Ts = 1e-3;

  // current angle
  angle_now = sensor->getAngle(sensor);
  // velocity calculation
  vel = (angle_now - ctx->angle_prev) / Ts;

  // save variables for future pass
  ctx->angle_prev = angle_now;
  ctx->velocity_calc_timestamp = now_us;

  return vel;
}

static int AS5048A_needSearch(struct FocSensor *sensor)
{
  return 0;
}

static SensorInterface_t sensorDriver = {
	.interfaceName = "pwm",
	.handle = &htim2,
	.address = 0,
	.timeout = 100,
	.retry = 1,
};

static FocSensor_t AS5048aSensor = {
	.sensorName = "AS5048a_pwm",
	.sensorData = &AS5048aCtx,
	.interface = &sensorDriver,
	.init = AS5048a_init,
	.getAngle = AS5048a_getAngle,
	.getVelocity = AS5048a_getVelocity,
	.needsSearch = AS5048A_needSearch,
};

MODULE_INIT static void init(void){
  MiniSensorManager_registerSensor(&AS5048aSensor);
}