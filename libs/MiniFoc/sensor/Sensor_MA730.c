//
// Created by 邓志君 on 2023/3/4.
//


#include "Sensor.h"
#include "../../../framework/MiniCommon.h"

#undef LOG_TAG
#define LOG_TAG "MA730"

#define  MA730_CPR        65536      //14bit, 但高位在前,低位补0
#define  MA730_Angle      0x0000     //

typedef struct {
    uint32_t cpr;
    uint32_t angle_data;
    int32_t d_angle;
    uint32_t raw_data_prev;
    float full_rotation_offset;
    float angle_prev;
    uint32_t velocity_calc_timestamp;
} MA730Context_t;

static MA730Context_t MA730Ctx = {0};


#define SPI2_CS1_L  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,GPIO_PIN_RESET)      //CS1_L
#define SPI2_CS1_H  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,GPIO_PIN_SET)        //CS1_H
#define SPI2_TX_OFF {GPIOC->CRH&=0x0FFFFFFF;GPIOC->CRH|=0x40000000;}  //把PB15(MOSI)配置为浮空模式
#define SPI2_TX_ON  {GPIOC->CRH&=0x0FFFFFFF;GPIOC->CRH|=0xB0000000;}  //把PB15(MOSI)配置为复用推挽输出(50MHz)

static unsigned short readRawData(SensorInterface_t *interface) {
    unsigned short u16Data;

    uint16_t sendData = MA730_Angle;
    SPI2_CS1_L;
    HAL_SPI_TransmitReceive(interface->handle, (uint8_t *) &sendData, (uint8_t *) &u16Data, 2, interface->timeout);
    SPI2_CS1_H;

    return u16Data;
}

static void MA730_init(struct FocSensor *sensor) {
    log_i("MA730 init");

    MA730Context_t *ctx = (MA730Context_t *) sensor->sensorData;
    if (!ctx) {
        return;
    }

    ctx->cpr = MA730_CPR;
    ctx->raw_data_prev = readRawData(sensor->interface);
    ctx->angle_prev = sensor->getAngle(sensor);
    ctx->full_rotation_offset = 0;
    ctx->velocity_calc_timestamp = 0;
}

static float MA730_getAngle(struct FocSensor *senosr) {
    MA730Context_t *ctx = (MA730Context_t *) senosr->sensorData;
    if (!ctx) {
        return 0.0f;
    }

    ctx->angle_data = readRawData(senosr->interface);
    // tracking the number of rotations
    // in order to expand angle range form [0,2PI] to basically infinity
    ctx->d_angle = ctx->angle_data - ctx->raw_data_prev;
    // if overflow happened track it as full rotation
    if (fabs(ctx->d_angle) > (0.8 * ctx->cpr)) {
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

    return (ctx->full_rotation_offset + ((float) ctx->angle_data / ctx->cpr) * _2PI);
}

static float MA730_getVelocity(struct FocSensor *sensor) {
    MA730Context_t *ctx = (MA730Context_t *) sensor->sensorData;
    if (!ctx) {
        return 0.0f;
    }

    uint32_t now_us;
    float Ts, angle_now, vel;

    // calculate sample time
    now_us = MiniFoc_micros(); //_micros();
    if (now_us < ctx->velocity_calc_timestamp) {
        Ts = (float) (ctx->velocity_calc_timestamp - now_us) / 9 * 1e-6;
    } else {
        Ts = (float) (0xFFFFFF - now_us + ctx->velocity_calc_timestamp) / 9 * 1e-6;
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

static int needSearch(struct FocSensor *sensor) {
    UNUSED(sensor);
    return 0;
}

static SensorInterface_t sensorDriver = {
        .interfaceName = "spi",
        .handle = &hspi2,
        .address = 0,
        .timeout = 100,
        .retry = 1,
};

static FocSensor_t MA730Sensor = {
        .sensorName = "MA730",
        .sensorData = &MA730Ctx,
        .interface = &sensorDriver,
        .init = MA730_init,
        .getAngle = MA730_getAngle,
        .getVelocity = MA730_getVelocity,
        .needsSearch = needSearch,
};

MODULE_INIT static void init(void) {
    MiniSensorManager_registerSensor(&MA730Sensor);
}