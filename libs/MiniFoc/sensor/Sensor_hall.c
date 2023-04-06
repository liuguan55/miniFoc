//
// Created by 邓志君 on 2023/3/3.
//

#include "Sensor.h"

#include "MiniCommon.h"
#define LOG_TAG "HALL"

#define  POLE_PAIR      8      //12bit

typedef struct {
  GPIO_TypeDef port;
  uint32_t Pin;
}GpioPort_t;
typedef struct {
  GpioPort_t gpioA;
  GpioPort_t gpioB;
  GpioPort_t gpioC;
}HallGpio_t;

typedef struct {
  uint32_t cpr;
  uint32_t polePair;
  volatile long pulse_timestamp;//!< last impulse timestamp in us
  volatile int A_active; //!< current active states of A channel
  volatile int B_active; //!< current active states of B channel
  volatile int C_active; //!< current active states of C channel
  volatile long pulse_diff;
  // the current 3bit state of the hall sensors
  volatile int8_t hall_state;
  // the current sector of the sensor. Each sector is 60deg electrical
  volatile int8_t electric_sector;
  // the number of electric rotations
  volatile long electric_rotations;
  // this is sometimes useful to identify interrupt issues (e.g. weak or no pullup resulting in 1000s of interrupts)
  volatile long total_interrupts;
  // whether last step was CW (+1) or CCW (-1).
  Direction direction;
}Context_t;

static Context_t ctx = {0};
static HallGpio_t hallGpio = {
	{.port = GPIOA, GPIO_PIN_0},
	{.port = GPIOA, GPIO_PIN_1},
	{.port = GPIOA, GPIO_PIN_2},
};

// seq 1 > 5 > 4 > 6 > 2 > 3 > 1     000 001 010 011 100 101 110 111
const int8_t ELECTRIC_SECTORS[8] = {-1, 0, 4, 5, 2, 1, 3, -1};

static void updateState(void)
{
  long new_pulse_timestamp = MiniFoc_micros();

  int8_t new_hall_state = ctx.C_active + (ctx.B_active << 1) + (ctx.A_active << 2);

  // glitch avoidance #1 - sometimes we get an interrupt but pins haven't changed
  if (new_hall_state == ctx.hall_state)
  {
	return;
  }
  ctx.hall_state = new_hall_state;

  int8_t new_electric_sector = ELECTRIC_SECTORS[ctx.hall_state];
  static Direction old_direction;
  if (new_electric_sector - ctx.electric_sector > 3)
  {
	//underflow
	ctx.direction = CCW;
	ctx.electric_rotations += ctx.direction;
  } else if (new_electric_sector - ctx.electric_sector < (-3))
  {
	//overflow
	ctx.direction = CW;
	ctx.electric_rotations += ctx.direction;
  } else
  {
	ctx.direction = (new_electric_sector > ctx.electric_sector) ? CW : CCW;
  }
  ctx.electric_sector = new_electric_sector;

  // glitch avoidance #2 changes in direction can cause velocity spikes.  Possible improvements needed in this area
  if (ctx.direction == old_direction)
  {
	// not oscilating or just changed direction
	ctx.pulse_diff = new_pulse_timestamp - ctx.pulse_timestamp;
  } else
  {
	ctx.pulse_diff = 0;
  }

  ctx.pulse_timestamp = new_pulse_timestamp;
  ctx.total_interrupts++;
  old_direction = ctx.direction;
}

static void HandleA(void)
{
  ctx.A_active = HAL_GPIO_ReadPin(&hallGpio.gpioA.port,hallGpio.gpioA.Pin);
  updateState();
}
static void HandleB(void)
{
  ctx.B_active = HAL_GPIO_ReadPin(&hallGpio.gpioB.port,hallGpio.gpioB.Pin);
  updateState();
}
static void HandleC(void)
{
  ctx.C_active = HAL_GPIO_ReadPin(&hallGpio.gpioC.port,hallGpio.gpioC.Pin);
  updateState();
}

static unsigned short readRawData(SensorInterface_t *interface)
{
  unsigned short  u16Data;

  return u16Data;
}

static void sensor_init(struct FocSensor *sensor)
{
  log_i("hall init");

  Context_t *ctx = (Context_t *)sensor->sensorData;
  if (!ctx){
	return ;
  }

  ctx->polePair = POLE_PAIR ;
  ctx->cpr = 6 * ctx->polePair ;
  ctx->electric_rotations = 0;
  ctx->A_active = HAL_GPIO_ReadPin(&hallGpio.gpioA.port,hallGpio.gpioA.Pin);
  ctx->B_active = HAL_GPIO_ReadPin(&hallGpio.gpioB.port,hallGpio.gpioB.Pin);
  ctx->C_active = HAL_GPIO_ReadPin(&hallGpio.gpioC.port,hallGpio.gpioC.Pin);
  updateState();

  ctx->pulse_timestamp = MiniFoc_micros();
}

static float getAngle(struct FocSensor *sensor)
{
  Context_t *ctx = (Context_t *)sensor->sensorData;
  if (!ctx){
	return 0;
  }

  return ((ctx->electric_rotations * 6 + ctx->electric_sector) / ctx->cpr) * _2PI;
}

static float getVelocity(struct FocSensor *sensor)
{
  Context_t *ctx = (Context_t *)sensor->sensorData;
  if (!ctx){
	return 0;
  }

  if (ctx->pulse_diff == 0 || ((MiniFoc_micros() - ctx->pulse_timestamp) > ctx->pulse_diff))
  { // last velocity isn't accurate if too old
	return 0;
  } else
  {
	return ctx->direction * (_2PI / ctx->cpr) / (ctx->pulse_diff / 1000000.0);
  }
}

static int needSearch(void)
{
  return 0;
}

static SensorInterface_t sensorDriver = {
	.interfaceName = "gpio",
	.handle = &hallGpio,
	.address = 0,
	.timeout = 100,
	.retry = 1,
};

static FocSensor_t HALLSensor = {
	.sensorName = "HALL",
	.sensorData = &ctx,
	.interface = &sensorDriver,
	.init = sensor_init,
	.getAngle = getAngle,
	.getVelocity = getVelocity,
	.needsSearch = needSearch,
};

MODULE_INIT static void init(void){
  MiniSensorManager_registerSensor(&HALLSensor);
}