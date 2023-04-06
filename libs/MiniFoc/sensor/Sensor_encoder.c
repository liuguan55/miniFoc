//
// Created by 邓志君 on 2023/3/3.
//

#include "Sensor.h"

#include "MiniCommon.h"

#define LOG_TAG "ENCODER"

#define  PPR      2048      //12bit

typedef enum
{
  Quadrature_ON, //!<  Enable quadrature mode CPR = 4xPPR
  Quadrature_OFF //!<  Disable quadrature mode / CPR = PPR
}Quadrature;

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
  float cpr;
  Quadrature quadrature;//!< Configuration parameter enable or disable quadrature mode

  volatile long pulse_counter;//!< current pulse counter
  volatile long pulse_timestamp;//!< last impulse timestamp in us
  volatile int A_active; //!< current active states of A channel
  volatile int B_active; //!< current active states of B channel
  volatile int I_active; //!< current active states of Index channel
  volatile char index_found; //!< flag stating that the index has been found

  // velocity calculation variables
  float prev_Th, pulse_per_second;
  volatile long prev_pulse_counter, prev_timestamp_us;
}Context_t;

static Context_t ctx = {0};
static HallGpio_t hallGpio = {
	{.port = GPIOA, GPIO_PIN_0},
	{.port = GPIOA, GPIO_PIN_1},
	{.port = GPIOA, GPIO_PIN_2},
};

static void HandleA(void)
{
  uint8_t A = HAL_GPIO_ReadPin(&hallGpio.gpioA.port,hallGpio.gpioA.Pin);
  switch (ctx.quadrature)
  {
	case Quadrature_ON:
	  // CPR = 4xPPR
	  if (A != ctx.A_active)
	  {
		ctx.pulse_counter += (ctx.A_active == ctx.B_active) ? 1 : -1;
		ctx.pulse_timestamp = MiniFoc_micros();
		ctx.A_active = A;
	  }
	  break;
	case Quadrature_OFF:
	  // CPR = PPR
	  if (A && ! HAL_GPIO_ReadPin(&hallGpio.gpioB.port,hallGpio.gpioB.Pin))
	  {
		ctx.pulse_counter++;
		ctx.pulse_timestamp = MiniFoc_micros();
	  }
	  break;
  }
}
static void HandleB(void)
{
  uint8_t B = HAL_GPIO_ReadPin(&hallGpio.gpioB.port,hallGpio.gpioB.Pin);
  switch (ctx.quadrature)
  {
	case Quadrature_ON:
	  //     // CPR = 4xPPR
	  if (B != ctx.B_active)
	  {
		ctx.pulse_counter += (ctx.A_active != ctx.B_active) ? 1 : -1;
		ctx.pulse_timestamp = MiniFoc_micros();
		ctx.B_active = B;
	  }
	  break;
	case Quadrature_OFF:
	  // CPR = PPR
	  if (B && !HAL_GPIO_ReadPin(&hallGpio.gpioA.port,hallGpio.gpioA.Pin))
	  {
		ctx.pulse_counter--;
		ctx.pulse_timestamp = MiniFoc_micros();
	  }
	  break;
  }
}

// private function used to determine if encoder has index
static int hasIndex()
{
  return hallGpio.gpioC.Pin != -1;
}

// getter for index pin
// return -1 if no index
static int needsSearch()
{
  return hasIndex() && !ctx.index_found;
}

// Index channel
static void handleIndex()
{
  if (hasIndex())
  {
	uint8_t I = HAL_GPIO_ReadPin(&hallGpio.gpioB.port,hallGpio.gpioB.Pin);
	if (I && !ctx.I_active)
	{
	  ctx.index_found = true;
	  // align encoder on each index
	  long tmp = ctx.pulse_counter;
	  // corrent the counter value
	  ctx.pulse_counter = round((double) ctx.pulse_counter / (double) ctx.cpr) * ctx.cpr;
	  // preserve relative speed
	  ctx.prev_pulse_counter += ctx.pulse_counter - tmp;
	}
	ctx.I_active = I;
  }
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

  // counter setup
  ctx->pulse_counter = 0;
  ctx->pulse_timestamp = 0;

  ctx->cpr = PPR;
  ctx->A_active = 0;
  ctx->B_active = 0;
  ctx->I_active = 0;

  // velocity calculation variables
  ctx->prev_Th = 0;
  ctx->pulse_per_second = 0;
  ctx->prev_pulse_counter = 0;
  ctx->prev_timestamp_us = MiniFoc_micros();

  // enable quadrature encoder by default
  ctx->quadrature = Quadrature_ON;

  // counter setup
  ctx->pulse_counter = 0;
  ctx->pulse_timestamp = MiniFoc_micros();
  // velocity calculation variables
  ctx->prev_Th = 0;
  ctx->pulse_per_second = 0;
  ctx->prev_pulse_counter = 0;
  ctx->prev_timestamp_us = MiniFoc_micros();

  // initial cpr = PPR
  // change it if the mode is quadrature
  if (ctx->quadrature == Quadrature_ON) ctx->cpr = 4 * ctx->cpr;
}

static float getAngle(struct FocSensor *sensor)
{
  Context_t *ctx = (Context_t *)sensor->sensorData;
  if (!ctx){
	return 0;
  }

  return _2PI * (ctx->pulse_counter) / ((float) ctx->cpr);
}

static float getVelocity(struct FocSensor *sensor)
{
  Context_t *ctx = (Context_t *)sensor->sensorData;
  if (!ctx){
	return 0;
  }

  // timestamp
  long timestamp_us = MiniFoc_micros();
  // sampling time calculation
  float Ts = (timestamp_us - ctx->prev_timestamp_us) * 1e-6;
  // quick fix for strange cases (micros overflow)
  if (Ts <= 0 || Ts > 0.5) Ts = 1e-3;

  // time from last impulse
  float Th = (timestamp_us - ctx->pulse_timestamp) * 1e-6;
  long dN = ctx->pulse_counter - ctx->prev_pulse_counter;

  // Pulse per second calculation (Eq.3.)
  // dN - impulses received
  // Ts - sampling time - time in between function calls
  // Th - time from last impulse
  // Th_1 - time form last impulse of the previous call
  // only increment if some impulses received
  float dt = Ts + ctx->prev_Th - Th;
  ctx->pulse_per_second = (dN != 0 && dt > Ts / 2) ? dN / dt : ctx->pulse_per_second;

  // if more than 0.05 passed in between impulses
  if (Th > 0.1) ctx->pulse_per_second = 0;

  // velocity calculation
  float velocity = ctx->pulse_per_second / ((float) ctx->cpr) * (_2PI);

  // save variables for next pass
  ctx->prev_timestamp_us = timestamp_us;
  // save velocity calculation variables
  ctx->prev_Th = Th;
  ctx->prev_pulse_counter = ctx->pulse_counter;

  return velocity;
}


static SensorInterface_t sensorDriver = {
	.interfaceName = "gpio",
	.handle = &hallGpio,
	.address = 0,
	.timeout = 100,
	.retry = 1,
};

static FocSensor_t EncoderSensor = {
	.sensorName = "ENCODER",
	.sensorData = &ctx,
	.interface = &sensorDriver,
	.init = sensor_init,
	.getAngle = getAngle,
	.getVelocity = getVelocity,
	.needsSearch = needsSearch,
};

MODULE_INIT static void init(void){
  MiniSensorManager_registerSensor(&EncoderSensor);
}