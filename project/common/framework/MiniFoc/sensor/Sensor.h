#ifndef SENSOR_LIB_H
#define SENSOR_LIB_H
#include <stdint.h>
#include "common/framework/MiniFoc/core/list.h"

typedef struct {
  char *interfaceName;
  uint32_t handle ;
  uint32_t address ;
  uint32_t timeout;
  uint32_t retry;
}SensorInterface_t;

typedef struct FocSensor{
  char *sensorName ;
  void *sensorData;
  SensorInterface_t *interface;
  void (*init)(struct FocSensor *);
  float (*getAngle)(struct FocSensor *);
  float (*getVelocity)(struct FocSensor *);
  int  (*needsSearch)(struct FocSensor *);
}FocSensor_t;

void MiniSensorManager_init(void);
int MiniSensorManager_registerSensor(FocSensor_t *sensor);
int MiniSensorManager_unregisterSensor(const char *sensorName);
FocSensor_t* MiniSensorManager_findSensorByName(const char *sensorName);
#endif
