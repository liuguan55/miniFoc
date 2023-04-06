/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "MiniCommon.h"
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
#include <stdio.h>
#include <string.h>
uint8_t  readBuf[512];
uint8_t  writeBuf[512];
BYTE work[_MAX_SS];
char *fileName = "test.csv";
uint32_t writeLen;
uint32_t readLen;

static uint8_t sd_initalize = 0;
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

int MiniFatfs_init(void)
{
  retSD = f_mount(&SDFatFS, SDPath, 1);
  if (retSD != FR_OK){
    if (retSD == FR_NO_FILESYSTEM){
	  retSD = f_mkfs(SDPath, 0 ,_MAX_SS , work, sizeof(work));
	  if (retSD == FR_OK)	{
	    printf("fs format success\n");
		retSD = f_mount(NULL, SDPath, 1);
		retSD = f_mount(&SDFatFS, SDPath, 1);
	  }else {
		printf("format failed ,err:%d\n",retSD);
		return retSD;
	  }
    }else {
	  printf("f_mount failed ,err:%d\n",retSD);
	  return retSD;
    }
  }else {
	sd_initalize = 1;
    printf("mount fatfs success\n");
  }
}

int MiniFatfs_writeLog(const char *filename, const char *data, int len)
{
  if (!sd_initalize) {
    return 0;
  }

  FIL file ;
  int writeLen = 0;

  int ret = f_open(&file, filename, FA_OPEN_APPEND | FA_WRITE);
  if (ret != FR_OK)
  {
    printf("open %s failed,ret %d\n",filename, ret);
    return 0;
  }

  ret = f_write(&file, data, len, &writeLen);

  f_close(&file);

  return writeLen;
}

int fatfs_test(void)
{
  retSD = f_mount(&SDFatFS, SDPath, 1);
  if (retSD != FR_OK){
    printf("f_mount failed ,err:%d\n",retSD);
    return retSD;
  }

  retSD = f_open(&SDFile,fileName,FA_OPEN_APPEND|FA_WRITE);
  if (retSD != FR_OK){
	printf("f_open failed ,err:%d\n",retSD);
	return retSD;
  }

  sprintf((char *)writeBuf,"%s","11,44,33\n");
  retSD = f_write(&SDFile,writeBuf,strlen((const char *)writeBuf),&writeLen);
  retSD = f_close(&SDFile);

  retSD = f_open(&SDFile,fileName,FA_OPEN_EXISTING|FA_READ);
  retSD = f_read(&SDFile,readBuf,512,&readLen);
  f_close(&SDFile);
  printf("%s",readBuf);

  f_mount(NULL,SDPath,1);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
sdcardTest, fatfs_test, test sdcard );
/* USER CODE END Application */
