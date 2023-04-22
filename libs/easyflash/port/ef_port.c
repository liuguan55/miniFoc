/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
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
 * Created on: 2015-01-16
 */
#include <easyflash.h>
#include <stdarg.h>
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "usart.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include "../../../framework/MiniCommon.h"

/* base address of the flash sectors */
#define ADDR_FLASH_SECTOR_0      ((uint32_t)0x08000000) /* Base address of Sector 0, 16 K bytes   */
#define ADDR_FLASH_SECTOR_1      ((uint32_t)0x08004000) /* Base address of Sector 1, 16 K bytes   */
#define ADDR_FLASH_SECTOR_2      ((uint32_t)0x08008000) /* Base address of Sector 2, 16 K bytes   */
#define ADDR_FLASH_SECTOR_3      ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 K bytes   */
#define ADDR_FLASH_SECTOR_4      ((uint32_t)0x08010000) /* Base address of Sector 4, 64 K bytes   */
#define ADDR_FLASH_SECTOR_5      ((uint32_t)0x08020000) /* Base address of Sector 5, 128 K bytes  */
#define ADDR_FLASH_SECTOR_6      ((uint32_t)0x08040000) /* Base address of Sector 6, 128 K bytes  */
#define ADDR_FLASH_SECTOR_7      ((uint32_t)0x08060000) /* Base address of Sector 7, 128 K bytes  */
#define ADDR_FLASH_SECTOR_8      ((uint32_t)0x08080000) /* Base address of Sector 8, 128 K bytes  */
#define ADDR_FLASH_SECTOR_9      ((uint32_t)0x080A0000) /* Base address of Sector 9, 128 K bytes  */
#define ADDR_FLASH_SECTOR_10     ((uint32_t)0x080C0000) /* Base address of Sector 10, 128 K bytes */
#define ADDR_FLASH_SECTOR_11     ((uint32_t)0x080E0000) /* Base address of Sector 11, 128 K bytes */

static char logBuff[MINI_CONSOLEBUF_SIZE] = {0};
static osMutexId_t flashMutex = NULL;
static const osMutexAttr_t flashMutexAtr = {
	.name = "flash mutex"
};
/* default environment variables set for user */
static const ef_env default_env_set[] = {
	{"device_id","1", 1},
	{"boot_times", "1", 1}
};


/**
 * Get the sector of a given address
 *
 * @param address flash address
 *
 * @return The sector of a given address
 */
static uint32_t stm32_get_sector(uint32_t address) {
  uint32_t sector = 0;

  if ((address < ADDR_FLASH_SECTOR_1) && (address >= ADDR_FLASH_SECTOR_0)) {
	sector = FLASH_SECTOR_0;
  } else if ((address < ADDR_FLASH_SECTOR_2) && (address >= ADDR_FLASH_SECTOR_1)) {
	sector = FLASH_SECTOR_1;
  } else if ((address < ADDR_FLASH_SECTOR_3) && (address >= ADDR_FLASH_SECTOR_2)) {
	sector = FLASH_SECTOR_2;
  } else if ((address < ADDR_FLASH_SECTOR_4) && (address >= ADDR_FLASH_SECTOR_3)) {
	sector = FLASH_SECTOR_3;
  } else if ((address < ADDR_FLASH_SECTOR_5) && (address >= ADDR_FLASH_SECTOR_4)) {
	sector = FLASH_SECTOR_4;
  } else if ((address < ADDR_FLASH_SECTOR_6) && (address >= ADDR_FLASH_SECTOR_5)) {
	sector = FLASH_SECTOR_5;
  } else if ((address < ADDR_FLASH_SECTOR_7) && (address >= ADDR_FLASH_SECTOR_6)) {
	sector = FLASH_SECTOR_6;
  } else if ((address < ADDR_FLASH_SECTOR_8) && (address >= ADDR_FLASH_SECTOR_7)) {
	sector = FLASH_SECTOR_7;
  } else if ((address < ADDR_FLASH_SECTOR_9) && (address >= ADDR_FLASH_SECTOR_8)) {
	sector = FLASH_SECTOR_8;
  } else if ((address < ADDR_FLASH_SECTOR_10) && (address >= ADDR_FLASH_SECTOR_9)) {
	sector = FLASH_SECTOR_9;
  } else if ((address < ADDR_FLASH_SECTOR_11) && (address >= ADDR_FLASH_SECTOR_10)) {
	sector = FLASH_SECTOR_10;
  } else {
	sector = FLASH_SECTOR_11;
  }

  return sector;
}

/**
 * Get the sector size
 *
 * @param sector sector
 *
 * @return sector size
 */
static uint32_t stm32_get_sector_size(uint32_t sector) {
  EF_ASSERT(IS_FLASH_SECTOR(sector));

  switch (sector) {
	case FLASH_SECTOR_0: return 16 * 1024;
	case FLASH_SECTOR_1: return 16 * 1024;
	case FLASH_SECTOR_2: return 16 * 1024;
	case FLASH_SECTOR_3: return 16 * 1024;
	case FLASH_SECTOR_4: return 64 * 1024;
	case FLASH_SECTOR_5: return 128 * 1024;
	case FLASH_SECTOR_6: return 128 * 1024;
	case FLASH_SECTOR_7: return 128 * 1024;
	case FLASH_SECTOR_8: return 128 * 1024;
	case FLASH_SECTOR_9: return 128 * 1024;
	case FLASH_SECTOR_10: return 128 * 1024;
	case FLASH_SECTOR_11: return 128 * 1024;
	default : return 128 * 1024;
  }
}
/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
  EfErrCode result = EF_NO_ERR;

  *default_env = default_env_set;
  *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

  flashMutex = osMutexNew(&flashMutexAtr);

  sfud_init();

  return result;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
  EfErrCode result = EF_NO_ERR;

//  printf("ef_port_read addr %x size %d\n",addr,size);
//  uint8_t *buf_8 = (uint8_t *)buf;
//  size_t i;
//  for (i = 0; i < size; i++, addr ++, buf_8++) {
//	*buf_8 = *(uint8_t *) addr;
//  }
//获取SFUD Flash设备对象
  const sfud_flash *flash = sfud_get_device_table() + SFUD_W25Q16X_DEVICE_INDEX;

  //使用SFUD开源库提供的API实现Flash读取
  sfud_read(flash, addr, size, (uint8_t *)buf);

  return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
  EfErrCode result = EF_NO_ERR;
//  size_t erased_size = 0;
//  uint32_t start_erase_sector, cur_erase_sector, erase_sector_num = 0;
//
//  ef_print("addr %x size %d\n",addr,size);
//  /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
//  EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);
//
//  /* You can add your code under here. */
//  HAL_FLASH_Unlock();
//  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP |FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);
//
//  start_erase_sector = stm32_get_sector(addr + erased_size);
//  while(erased_size < size) {
//	cur_erase_sector = stm32_get_sector(addr + erased_size);
//	erased_size += stm32_get_sector_size(cur_erase_sector);
//	erase_sector_num++;
//  }
//  FLASH_EraseInitTypeDef f;                        //初始化FLASH_EraseInitTypeDef
//  f.TypeErase = FLASH_TYPEERASE_SECTORS;//标明Flash执行页面只做擦除操作
//  f.Sector = start_erase_sector;          //声明要擦除的地址
//  f.NbSectors = erase_sector_num  	;//说明要擦除的页数，此参数必须是Min_Data = 1和Max_Data =(最大页数-初始页的值)之间的值
//  f.VoltageRange = FLASH_VOLTAGE_RANGE_3;
//  //一些MCU一页是1KB，一些MCU一页是2KB
//  uint32_t PageError = 0;                        //设置PageError,如果出现错误这个变量会被设置为出错的FLASH地址
//
//  //调用擦除函数
//  if (HAL_FLASHEx_Erase(&f, &PageError) != HAL_OK){
//    printf("HAL_FLASHEx_Erase failed\n");
//	result = EF_ERASE_ERR;
//  }
//
//  HAL_FLASH_Lock();

  sfud_err sfud_result = SFUD_SUCCESS;

  //获取SFUD Flash设备对象
  const sfud_flash *flash = sfud_get_device_table() + SFUD_W25Q16X_DEVICE_INDEX;

  /* make sure the start address is a multiple of FLASH_ERASE_MIN_SIZE */
  EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

  //使用SFUD提供的API实现Flash擦除
  sfud_result = sfud_erase(flash, addr, size);

  if(sfud_result != SFUD_SUCCESS) {
	result = EF_ERASE_ERR;
  }

  return result;
}
/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
  EfErrCode result = EF_NO_ERR;
//  size_t i;
//  uint32_t read_data;
//  uint8_t *buf_8 = (uint8_t *)buf;
////  printf("addr %x size %d\n",addr,size);
//  HAL_FLASH_Unlock();
//  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP |FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR | FLASH_FLAG_WRPERR);
//
//  for (i = 0; i < size; i++, buf_8++, addr++)
//  {
//	/* write data */
//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, addr, *buf_8);
//	read_data = *(uint8_t *) addr;
//	/* check data */
//	if (read_data != *buf_8) {
//	  result = EF_WRITE_ERR;
//	  break;
//	}
//  }
//
//  HAL_FLASH_Lock();
  sfud_err sfud_result = SFUD_SUCCESS;

  //获取SFUD Flash设备对象
  const sfud_flash *flash = sfud_get_device_table() + SFUD_W25Q16X_DEVICE_INDEX;

  //使用SFUD开源库提供的API实现
  sfud_result = sfud_write(flash, addr, size, (const uint8_t *)buf);

  if(sfud_result != SFUD_SUCCESS) {
	result = EF_WRITE_ERR;
  }

  return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
  /* You can add your code under here. */
  osMutexAcquire(flashMutex, portMAX_DELAY);
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
  /* You can add your code under here. */
  osMutexRelease(flashMutex);
}

/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format driver format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {

#ifdef PRINT_DEBUG

  va_list args;

  /* args point to the first variable parameter */
  va_start(args, format);
  ef_print("[Flash](%s:%ld) ", file, line);
  /* must use vprintf to print */
  vsnprintf(logBuff,sizeof(logBuff), format, args);
  ef_print("%s", logBuff);
  va_end(args);
#endif

}

/**
 * This function is print flash routine info.
 *
 * @param format driver format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
  va_list args;

  /* args point to the first variable parameter */
  va_start(args, format);
  ef_print("[Flash]");
  /* must use vprintf to print */
  vsnprintf(logBuff,sizeof(logBuff), format, args);
  ef_print("%s", logBuff);
  va_end(args);
}
/**
 * This function is print flash non-package info.
 *
 * @param format driver format
 * @param ... args
 */
void ef_print(const char *format, ...) {

  va_list args;

  /* args point to the first variable parameter */
  va_start(args, format);
  /* must use vprintf to print */
  vsnprintf(logBuff,sizeof(logBuff), format, args);
  va_end(args);

  MiniUart_writeData(MINI_CONSOLE_USART_IDX, logBuff, strlen(logBuff));

  usb_write_data(logBuff, strlen(logBuff));
}
