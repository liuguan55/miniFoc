//
// Created by 邓志君 on 2023/3/2.
//

#include "driver.h"
#include <string.h>
#include "../core/list.h"
#define LOG_TAG "FOC OUTPUT"


list g_bldcDriverList = {0};
list g_stepperDriverList = {0};


void MiniBldcDriverManager_init(void)
{
  listInit(&g_bldcDriverList);
}

int MiniBldcDriverManager_register(BldcDriver_t *driver)
{
  listInsert(&g_bldcDriverList, driver);

  return 0;
}


BldcDriver_t* MiniBldcDriverManager_findByName(const char *name)
{
  size_t driverNum = listLength(&g_bldcDriverList);

  BldcDriver_t *driver = NULL;
  for (int i = 0; i < driverNum; ++i) {
	driver = listFindByindex(&g_bldcDriverList, i);
	if (!driver){
	  break;
	}

	if (!strncmp(name, driver->name, strlen(driver->name))){
	  break;
	}
  }

  return driver;
}

int MiniBldcDriverManager_unregister(const char *name)
{
  BldcDriver_t *driver =MiniBldcDriverManager_findByName(name);
  listRemove(&g_bldcDriverList, driver);
}

void MiniStepperDriverManager_init(void)
{
  listInit(&g_stepperDriverList);
}

int MiniStepperDriverManager_register(StepperDriver_t *driver)
{
  listInsert(&g_stepperDriverList, driver);
}

int MiniStepperDriverManager_unregister(const char *name)
{
  StepperDriver_t *driver =MiniStepperDriverManager_findByName(name);
  listRemove(&g_stepperDriverList, driver);
}

BldcDriver_t* MiniStepperDriverManager_findByName(const char *name)
{
  size_t driverNum = listLength(&g_stepperDriverList);

  StepperDriver_t  *driver = NULL;
  for (int i = 0; i < driverNum; ++i) {
	driver = listFindByindex(&g_stepperDriverList, i);
	if (!driver){
	  break;
	}

	if (!strncmp(name, driver->name, strlen(driver->name))){
	  return driver;
	}
  }

  return NULL;
}



