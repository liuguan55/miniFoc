//
// Created by 邓志君 on 2023/3/3.
//

#include "Sensor.h"

#include "../../../framework/MiniCommon.h"

#undef LOG_TAG
#define LOG_TAG "ENCODER"

#define  PPR      2048      //12bit

typedef enum {
    Quadrature_ON, //!<  Enable quadrature mode CPR = 4xPPR
    Quadrature_OFF //!<  Disable quadrature mode / CPR = PPR
} Quadrature;

typedef struct {
    GPIO_TypeDef *port;
    uint32_t Pin;
} GpioPort_t;
typedef struct {
    GpioPort_t gpioA;
    GpioPort_t gpioB;
    GpioPort_t gpioC;
} HallGpio_t;

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
} Context_t;

static Context_t gCtx = {0};
static HallGpio_t hallGpio = {
        {.port = GPIOA, GPIO_PIN_0},
        {.port = GPIOA, GPIO_PIN_1},
        {.port = GPIOA, GPIO_PIN_2},
};

static void __unused HandleA(void) {
    uint8_t A = HAL_GPIO_ReadPin(hallGpio.gpioA.port, hallGpio.gpioA.Pin);
    switch (gCtx.quadrature) {
        case Quadrature_ON:
            // CPR = 4xPPR
            if (A != gCtx.A_active) {
                gCtx.pulse_counter += (gCtx.A_active == gCtx.B_active) ? 1 : -1;
                gCtx.pulse_timestamp = MiniFoc_micros();
                gCtx.A_active = A;
            }
            break;
        case Quadrature_OFF:
            // CPR = PPR
            if (A && !HAL_GPIO_ReadPin(hallGpio.gpioB.port, hallGpio.gpioB.Pin)) {
                gCtx.pulse_counter++;
                gCtx.pulse_timestamp = MiniFoc_micros();
            }
            break;
    }
}

static void __unused HandleB(void) {
    uint8_t B = HAL_GPIO_ReadPin(hallGpio.gpioB.port, hallGpio.gpioB.Pin);
    switch (gCtx.quadrature) {
        case Quadrature_ON:
            //     // CPR = 4xPPR
            if (B != gCtx.B_active) {
                gCtx.pulse_counter += (gCtx.A_active != gCtx.B_active) ? 1 : -1;
                gCtx.pulse_timestamp = MiniFoc_micros();
                gCtx.B_active = B;
            }
            break;
        case Quadrature_OFF:
            // CPR = PPR
            if (B && !HAL_GPIO_ReadPin(hallGpio.gpioA.port, hallGpio.gpioA.Pin)) {
                gCtx.pulse_counter--;
                gCtx.pulse_timestamp = MiniFoc_micros();
            }
            break;
    }
}

// private function used to determine if encoder has index
static int hasIndex() {
    return hallGpio.gpioC.Pin != -1;
}

// getter for index pin
// return -1 if no index
static int needsSearch() {
    return hasIndex() && !gCtx.index_found;
}

// Index channel
static __unused void handleIndex() {
    if (hasIndex()) {
        uint8_t I = HAL_GPIO_ReadPin(hallGpio.gpioB.port, hallGpio.gpioB.Pin);
        if (I && !gCtx.I_active) {
            gCtx.index_found = true;
            // align encoder on each index
            long tmp = gCtx.pulse_counter;
            // corrent the counter value
            gCtx.pulse_counter = round((double) gCtx.pulse_counter / (double) gCtx.cpr) * gCtx.cpr;
            // preserve relative speed
            gCtx.prev_pulse_counter += gCtx.pulse_counter - tmp;
        }
        gCtx.I_active = I;
    }
}

static void sensor_init(struct FocSensor *sensor) {
    log_i("hall init");

    Context_t *context = (Context_t *) sensor->sensorData;
    if (!context) {
        return;
    }

    // counter setup
    context->pulse_counter = 0;
    context->pulse_timestamp = 0;

    context->cpr = PPR;
    context->A_active = 0;
    context->B_active = 0;
    context->I_active = 0;

    // velocity calculation variables
    context->prev_Th = 0;
    context->pulse_per_second = 0;
    context->prev_pulse_counter = 0;
    context->prev_timestamp_us = MiniFoc_micros();

    // enable quadrature encoder by default
    context->quadrature = Quadrature_ON;

    // counter setup
    context->pulse_counter = 0;
    context->pulse_timestamp = MiniFoc_micros();
    // velocity calculation variables
    context->prev_Th = 0;
    context->pulse_per_second = 0;
    context->prev_pulse_counter = 0;
    context->prev_timestamp_us = MiniFoc_micros();

    // initial cpr = PPR
    // change it if the mode is quadrature
    if (context->quadrature == Quadrature_ON) context->cpr = 4 * context->cpr;
}

static float getAngle(struct FocSensor *sensor) {
    Context_t *ctx = (Context_t *) sensor->sensorData;
    if (!ctx) {
        return 0;
    }

    return _2PI * (ctx->pulse_counter) / ((float) ctx->cpr);
}

static float getVelocity(struct FocSensor *sensor) {
    Context_t *ctx = (Context_t *) sensor->sensorData;
    if (!ctx) {
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
        .sensorData = &gCtx,
        .interface = &sensorDriver,
        .init = sensor_init,
        .getAngle = getAngle,
        .getVelocity = getVelocity,
        .needsSearch = needsSearch,
};

MODULE_INIT static void init(void) {
    MiniSensorManager_registerSensor(&EncoderSensor);
}