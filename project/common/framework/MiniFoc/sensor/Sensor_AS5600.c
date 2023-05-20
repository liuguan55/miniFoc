//
// Created by 邓志君 on 2023/3/1.
//

#include "Sensor.h"
#include "common/framework/MiniCommon.h"

#undef LOG_TAG
#define LOG_TAG "AS5600"

#define  AS5600_Address  0x36
#define  RAW_Angle_Hi    0x0C
//#define  RAW_Angle_Lo    0x0D
#define  AS5600_CPR      4096

typedef struct {
  uint32_t cpr;
  uint32_t angle_data;
  int32_t d_angle;
  uint32_t raw_data_prev;
  float full_rotation_offset;
  float angle_prev;
  uint32_t velocity_calc_timestamp;
}AS5600Context_t;

static AS5600Context_t AS5600Ctx = {0};

static unsigned short readAs5600RawData(SensorInterface_t *interface)
{
  uint8_t reg = RAW_Angle_Hi ;
  uint8_t val[2] = {0};

  HAL_I2C_Master_Transmit(interface->handle, interface->address<<1 , &reg , 1, interface->timeout);
  HAL_I2C_Master_Receive(interface->handle, interface->address<<1, val, 2, interface->timeout);

  return ((val[0] << 8) + val[1]) ;
}

static void AS5600_init(struct FocSensor *sensor)
{
  log_i("AS5600 init");

  AS5600Context_t *ctx = (AS5600Context_t *)sensor->sensorData;
  if (!ctx){
    return ;
  }

  ctx->cpr = AS5600_CPR;
  ctx->raw_data_prev = readAs5600RawData(sensor->interface);
  ctx->angle_prev =  sensor->getAngle(sensor);
  ctx->full_rotation_offset = 0;
  ctx->velocity_calc_timestamp = 0;
}

static float AS5600_getAngle(struct FocSensor *senosr)
{
  AS5600Context_t *ctx = (AS5600Context_t *)senosr->sensorData;
  if (!ctx){
	return 0.0f;
  }

  ctx->angle_data = readAs5600RawData(senosr->interface);
//  printf("angle_data %d\n",ctx->angle_data);
  // tracking the number of rotations
  // in order to expand angle range form [0,2PI] to basically infinity
  ctx->d_angle = ctx->angle_data - ctx->raw_data_prev;
  // if overflow happened track it as full rotation
  if (fabs(ctx->d_angle) > (0.8 * ctx->cpr)){
	log_d("d_angle %d full_rotation_offset %4.2f angle_data %d",ctx->d_angle,ctx->full_rotation_offset,ctx->angle_data);
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

static float AS5600_getVelocity(struct FocSensor *sensor)
{
  AS5600Context_t *ctx = (AS5600Context_t *)sensor->sensorData;
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

static int AS5600_needSearch(struct FocSensor *sensor)
{
  return 0;
}

static SensorInterface_t sensorDriver_1 = {
	.interfaceName = "i2c",
	.handle = &hi2c1,
	.address = AS5600_Address,
	.timeout = 100,
	.retry = 1,
};

static SensorInterface_t sensorDriver_2 = {
	.interfaceName = "i2c",
	.handle = NULL,//&hi2c2,
	.address = AS5600_Address,
	.timeout = 100,
	.retry = 1,
};

static FocSensor_t AS5600Sensor_1 = {
	.sensorName = "AS5600@1",
	.sensorData = &AS5600Ctx,
	.interface = &sensorDriver_1,
	.init = AS5600_init,
	.getAngle = AS5600_getAngle,
	.getVelocity = AS5600_getVelocity,
	.needsSearch = AS5600_needSearch,
};

static FocSensor_t AS5600Sensor_2 = {
	.sensorName = "AS5600@2",
	.sensorData = &AS5600Ctx,
	.interface = &sensorDriver_2,
	.init = AS5600_init,
	.getAngle = AS5600_getAngle,
	.getVelocity = AS5600_getVelocity,
	.needsSearch = AS5600_needSearch,
};

MODULE_INIT static void init(void){
  MiniSensorManager_registerSensor(&AS5600Sensor_1);
  MiniSensorManager_registerSensor(&AS5600Sensor_2);
}