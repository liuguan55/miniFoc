

#include "Sensor.h"
#include <string.h>
#include "elog.h"
#include <stdio.h>

#define LOG_TAG "FOC SENSOR"


static list g_sensorList = {0};

void MiniSensorManager_init(void)
{
  listInit(&g_sensorList);
}

int MiniSensorManager_registerSensor(FocSensor_t *sensor)
{
  if (MiniSensorManager_findSensorByName(sensor->sensorName))
  {
    printf("already register %s sensor",sensor->sensorName);
    return -1;
  }

  listInsert(&g_sensorList, sensor);

  return 0;
}


FocSensor_t* MiniSensorManager_findSensorByName(const char *sensorName)
{
  size_t sensorNum = listLength(&g_sensorList);

  FocSensor_t *sensor = NULL;
  for (int i = 0; i < sensorNum; ++i) {
	sensor = listFindByindex(&g_sensorList, i);
	if (!sensor){
	  break;
	}

	if (!strncmp(sensorName, sensor->sensorName, strlen(sensor->sensorName))){
	  return sensor;
	}
  }

  return NULL;
}

int MiniSensorManager_unregisterSensor(const char *sensorName)
{
  FocSensor_t *sensor = MiniSensorManager_findSensorByName(sensorName);
  listRemove(&g_sensorList, sensor);

  return 0;
}


