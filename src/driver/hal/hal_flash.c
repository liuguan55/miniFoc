// /* Copyright (C) 2023 dengzhijun. All rights reserved.
//  *
//  *  Redistribution and use in source and binary forms, with or without
//  *  modification, are permitted provided that the following conditions
//  *  are met:
//  *    1. Redistributions of source code must retain the above copyright
//  *       notice, this list of conditions and the following disclaimer.
//  *    2. Redistributions in binary form must reproduce the above copyright
//  *       notice, this list of conditions and the following disclaimer in the
//  *       documentation and/or other materials provided with the
//  *       distribution.
//  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//  *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  *

//
// Created by 86189 on 2023/5/30.
//

#include "driver/hal/hal_flash.h"
#include "driver/driver_chip.h"
#include "driver/hal/hal_def.h"
#include "driver/hal/hal_dev.h"
#include "driver/hal/hal_board.h"

#ifdef TARGET_MCU_STM32F4
#define ADD_FLASH_SECTOR_0      ((uint32_t)0x08000000) // 16K
#define ADD_FLASH_SECTOR_1      ((uint32_t)0x08004000) // 16K
#define ADD_FLASH_SECTOR_2      ((uint32_t)0x08008000) // 16K
#define ADD_FLASH_SECTOR_3      ((uint32_t)0x0800C000) // 16K
#define ADD_FLASH_SECTOR_4      ((uint32_t)0x08010000) // 64K
#define ADD_FLASH_SECTOR_5      ((uint32_t)0x08020000) // 128K
#define ADD_FLASH_SECTOR_6      ((uint32_t)0x08040000) // 128K
#define ADD_FLASH_SECTOR_7      ((uint32_t)0x08060000) // 128K
#define ADD_FLASH_SECTOR_8      ((uint32_t)0x08080000) // 128K
#define ADD_FLASH_SECTOR_9      ((uint32_t)0x080A0000) // 128K
#define ADD_FLASH_SECTOR_10     ((uint32_t)0x080C0000) // 128K
#define ADD_FLASH_SECTOR_11     ((uint32_t)0x080E0000) // 128K



/**
 * @brief Get the Sector from address object
 * @param address  flash address
 * @return sector number
 */
static uint32_t getSector(uint32_t address){
    uint32_t secotr = 0;

    if (address >= ADD_FLASH_SECTOR_0 && address < ADD_FLASH_SECTOR_1){
        secotr = FLASH_SECTOR_0;
    }else if (address >= ADD_FLASH_SECTOR_1 && address < ADD_FLASH_SECTOR_2){
        secotr = FLASH_SECTOR_1;
    }else if (address >= ADD_FLASH_SECTOR_2 && address < ADD_FLASH_SECTOR_3){
        secotr = FLASH_SECTOR_2;
    }else if (address >= ADD_FLASH_SECTOR_3 && address < ADD_FLASH_SECTOR_4){
        secotr = FLASH_SECTOR_3;
    }else if (address >= ADD_FLASH_SECTOR_4 && address < ADD_FLASH_SECTOR_5){
        secotr = FLASH_SECTOR_4;
    }else if (address >= ADD_FLASH_SECTOR_5 && address < ADD_FLASH_SECTOR_6){
        secotr = FLASH_SECTOR_5;
    }else if (address >= ADD_FLASH_SECTOR_6 && address < ADD_FLASH_SECTOR_7){
        secotr = FLASH_SECTOR_6;
    }else if (address >= ADD_FLASH_SECTOR_7 && address < ADD_FLASH_SECTOR_8){
        secotr = FLASH_SECTOR_7;
    }else if (address >= ADD_FLASH_SECTOR_8 && address < ADD_FLASH_SECTOR_9){
        secotr = FLASH_SECTOR_8;
    }else if (address >= ADD_FLASH_SECTOR_9 && address < ADD_FLASH_SECTOR_10){
        secotr = FLASH_SECTOR_9;
    }else if (address >= ADD_FLASH_SECTOR_10 && address < ADD_FLASH_SECTOR_11){
        secotr = FLASH_SECTOR_10;
    }else {
        secotr = FLASH_SECTOR_11;
    }

    return secotr;
}
#endif

HAL_Status HAL_flashErase(uint32_t address, uint32_t size){
    HAL_Status status = HAL_STATUS_OK;
    uint32_t sectorError = 0;
    FLASH_EraseInitTypeDef eraseInit;

#ifdef TARGET_MCU_STM32F4
    if (address < ADD_FLASH_SECTOR_0 || address >= ADD_FLASH_SECTOR_11){
        return HAL_STATUS_ERROR;
    }

    size = HAL_ALIGN_UP(size, 4);

    if (address % 4 != 0 || size % 4 != 0){
        return HAL_STATUS_ERROR;
    }

    eraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
    eraseInit.Sector = getSector(address);
    eraseInit.NbSectors = getSector(address + size - 1) - eraseInit.Sector + 1;
    eraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;
#elif defined(TARGET_MCU_STM32F1)
    if (address < 0x08000000 || address >= 0x08100000){
        return HAL_STATUS_ERROR;
    }

    size = HAL_ALIGN_UP(size, 1024);

    if (address % 1024 != 0 || size % 1024 != 0){
        return HAL_STATUS_ERROR;
    }

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = address;
    eraseInit.NbPages = size / 1024;
#endif
    HAL_FLASH_Unlock();
    if (HAL_FLASHEx_Erase(&eraseInit, &sectorError)){
        status = HAL_STATUS_ERROR;
    }
    HAL_FLASH_Lock();

    return status;
}

HAL_Status HAL_flashWrite(uint32_t address, uint32_t *data, uint32_t size){
    HAL_Status status = HAL_STATUS_OK;

    if (address < 0x08000000 || address >= 0x08100000){
        return HAL_STATUS_ERROR;
    }

    if (address % 4 != 0){
        return HAL_STATUS_ERROR;
    }

    if (HAL_flashErase(address, size) != HAL_STATUS_OK){
        return HAL_STATUS_ERROR;
    }

    HAL_FLASH_Unlock();
    for (uint32_t i = 0; i < size; i++){
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i * 4, data[i]) != HAL_OK){
            status = HAL_STATUS_ERROR;
            break;
        }
    }
    HAL_FLASH_Lock();

    return status;
}

HAL_Status HAL_flashRead(uint32_t address, uint8_t *data, uint32_t size){
    HAL_Status status = HAL_STATUS_OK;

    if (address < 0x08000000 || address >= 0x08100000){
        return HAL_STATUS_ERROR;
    }

    for (uint32_t i = 0; i < size; ++i) {
        data[i] = *(uint8_t *)(address + i);
    }

    return status;
}