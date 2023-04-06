/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */

#include <elog.h>
#include <stdlib.h>
#include <string.h>
#include "MiniCommon.h"
#include "elog_flash.h"

static osMutexId_t logMutex = NULL;
/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
  ElogErrCode result = ELOG_NO_ERR;

  /* add your code here */
  logMutex = osMutexNew(NULL);

  return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void) {

  /* add your code here */
  osMutexDelete(logMutex);
}

/**
 * driver log port interface
 *
 * @param log driver of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {

  /* add your code here */
//  MiniUart_writeData(MINI_CONSOLE_USART_IDX, log, size);
//  logWrite(LOG_ALL_OBJ, LOG_DEBUG, log);
  logWriteBuffer(LOG_ALL_OBJ, LOG_DEBUG, log, size);
  MiniFatfs_writeLog("mini.log", log, size);
}

void elog_port_output2(int level, const char *log, size_t size)
{
  /* add your code here */
//  MiniUart_writeData(MINI_CONSOLE_USART_IDX, log, size);
  logWrite(LOG_ALL_OBJ, level, log);
  MiniFatfs_writeLog("mini.log", log, size);
}

/**
 * driver lock
 */
void elog_port_output_lock(void) {

  /* add your code here */
  osMutexAcquire(logMutex, portMAX_DELAY);
}

/**
 * driver unlock
 */
void elog_port_output_unlock(void) {

  /* add your code here */
  osMutexRelease(logMutex);
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {

  /* add your code here */
  static char buf[32];

  memset(buf, 0, sizeof(buf));
  snprintf(buf, sizeof(buf), "%d", MiniCommon_millis());

  return buf;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {

  /* add your code here */
  static char info[configMAX_TASK_NAME_LEN] = {0};
  memset(info, 0, sizeof(info));

  osThreadId_t id = osThreadGetId();
  char *thName = osThreadGetName(id);
  if (thName) {
	memcpy(info, thName, configMAX_TASK_NAME_LEN);
  }

  return info;
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {

  /* add your code here */
  return "";
}