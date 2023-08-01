//
// Created by 邓志君 on 2023/3/2.
//

#include "Current.h"
#include "common/framework/util/MiniCommon.h"
#include "driver/hal/hal_adc.h"

typedef struct {
    int pin;
    float gain;
    float offset;
} PhaseAdc_t;

typedef struct {
    PhaseAdc_t a;
    PhaseAdc_t b;
    PhaseAdc_t c;

    float shuntResistor;
    float gain;
    float voltsToAmpsRatio;
} currentContext_t;

static currentContext_t currentCtx = {
        .shuntResistor = 0.001,
        .gain = 20.0f,
        .a = {ADC_CHANNEL_6, 0, 0},
        .b = {ADC_CHANNEL_7, 0, 0},
        .c = {NOT_SET, 0, 0},
};

/******************************************************************************/
// function reading an ADC value and returning the read voltage
static float _readADCVoltageInline(unsigned char ch) {
    unsigned short raw_adc = HAL_adcRead(HAL_ADC_1, ch, 10);
    return (float) raw_adc * 3.3 / 4096;
}

// Function finding zero offsets of the ADC
static void Current_calibrateOffsets(currentContext_t *ctx) {
    int i;

    int pinA = ctx->a.pin;
    int pinB = ctx->b.pin;
    int pinC = ctx->c.pin;
    ctx->a.offset = 0;
    ctx->b.offset = 0;
    ctx->c.offset = 0;
    // read the adc voltage 1000 times ( arbitrary number )
    for (i = 0; i < 1000; i++) {
        ctx->a.offset += _readADCVoltageInline(pinA);
        ctx->b.offset += _readADCVoltageInline(pinB);
        if (_isset(pinC)) ctx->c.offset += _readADCVoltageInline(pinC);
        MiniCommon_delayMs(1);
    }
    // calculate the mean offsets
    ctx->a.offset = ctx->a.offset / 1000;
    ctx->b.offset = ctx->b.offset / 1000;
    if (_isset(pinC)) ctx->c.offset = ctx->c.offset / 1000;

    elog_d("foc", "offset_ia:%.4f,offset_ib:%.4f,offset_ic:%.4f.\r\n", ctx->a.offset, ctx->b.offset, ctx->c.offset);
}

static void currentInit(void) {
    HAL_adcInit(HAL_ADC_1);
    currentCtx.voltsToAmpsRatio = 1.0 / currentCtx.shuntResistor / currentCtx.gain; // volts to amps

    currentCtx.a.gain = currentCtx.voltsToAmpsRatio;
    currentCtx.b.gain = -currentCtx.voltsToAmpsRatio;
    currentCtx.c.gain = currentCtx.voltsToAmpsRatio;

    elog_d("foc", "gain_a:%.2f,gain_b:%.2f,gain_c:%.2f.\r\n", currentCtx.a.gain, currentCtx.b.gain, currentCtx.c.gain);

    Current_calibrateOffsets(&currentCtx);
}

static PhaseCurrent_s getPhaseCurrents(void) {
    PhaseCurrent_s current;

    int pinA = currentCtx.a.pin;
    int pinB = currentCtx.b.pin;
    int pinC = currentCtx.c.pin;
    float offset_ia = currentCtx.a.offset;
    float offset_ib = currentCtx.b.offset;
    float offset_ic = currentCtx.c.offset;
    float gain_a = currentCtx.a.gain;
    float gain_b = currentCtx.b.gain;
    float gain_c = currentCtx.c.gain;

    current.a = (_readADCVoltageInline(pinA) - offset_ia) * gain_a;// amps
    current.b = (_readADCVoltageInline(pinB) - offset_ib) * gain_b;// amps
    current.c = (!_isset(pinC)) ? 0 : (_readADCVoltageInline(pinC) - offset_ic) * gain_c; // amps

    return current;
}

static FocCurrent_t currentDriver = {
        .name = "inlineCurrent",
        .data = &currentCtx,
        .init = currentInit,
        .getPhaseCurrents= getPhaseCurrents,
};

MODULE_INIT static void init(void) {
    MiniCurrentManager_registerCurrent(&currentDriver);
}