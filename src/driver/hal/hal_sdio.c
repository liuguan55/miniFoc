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
// Created by 86189 on 2023/6/4.
//

#include "driver/hal/hal_sdio.h"
#include "stm32f4xx_hal.h"
#include "driver/hal/hal_board.h"
#include "driver/hal/hal_def.h"
#include "driver/hal/hal_dev.h"
#include "driver/hal/hal_os.h"
#include "sys/MiniDebug.h"

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;
typedef struct {
    SD_HandleTypeDef *hsd;
    DMA_HandleTypeDef *hdma_sdio_rx;
    DMA_HandleTypeDef *hdma_sdio_tx;
}SdcDev_t;

static SdcDev_t sdcDev[HAL_SDC_NR] = {
        {&hsd, &hdma_sdio_rx, &hdma_sdio_tx},
};

/**
  * @brief This function handles SDIO global interrupt.
  */
void SDIO_IRQHandler(void) {
    /* USER CODE BEGIN SDIO_IRQn 0 */

    /* USER CODE END SDIO_IRQn 0 */
    for (int i = 0; i < HAL_SDC_NR; ++i) {
        HAL_SD_IRQHandler(sdcDev[i].hsd);
    }
    /* USER CODE BEGIN SDIO_IRQn 1 */

    /* USER CODE END SDIO_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void) {
    /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

    /* USER CODE END DMA2_Stream3_IRQn 0 */
    HAL_DMA_IRQHandler(sdcDev[HAL_SDC_1].hdma_sdio_rx);
    /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

    /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream6 global interrupt.
  */
void DMA2_Stream6_IRQHandler(void) {
    /* USER CODE BEGIN DMA2_Stream6_IRQn 0 */

    /* USER CODE END DMA2_Stream6_IRQn 0 */
    HAL_DMA_IRQHandler(sdcDev[HAL_SDC_1].hdma_sdio_tx);
    /* USER CODE BEGIN DMA2_Stream6_IRQn 1 */

    /* USER CODE END DMA2_Stream6_IRQn 1 */
}

static void sdioClkEnable(HAL_SDC_ID sdioId){
    if (sdioId == HAL_SDC_1){
        __HAL_RCC_SDIO_CLK_ENABLE();
    }
}


static void sdioClkDisable(HAL_SDC_ID sdioId){
    if (sdioId == HAL_SDC_1){
        __HAL_RCC_SDIO_CLK_DISABLE();
    }
}

static HAL_Status sdioHwInit(HAL_SDC_ID sdioId){
    SdcDev_t *dev = &sdcDev[sdioId];
    sdioClkEnable(sdioId);
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_SDC, sdioId), 0);

    dev->hsd->Instance = SDIO;
    dev->hsd->Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    dev->hsd->Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    dev->hsd->Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    dev->hsd->Init.BusWide = SDIO_BUS_WIDE_1B;
    dev->hsd->Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    dev->hsd->Init.ClockDiv = 0;

    /* SDIO DMA Init */
    /* SDIO_RX Init */
    dev->hdma_sdio_rx->Instance = DMA2_Stream3;
    dev->hdma_sdio_rx->Init.Channel = DMA_CHANNEL_4;
    dev->hdma_sdio_rx->Init.Direction = DMA_PERIPH_TO_MEMORY;
    dev->hdma_sdio_rx->Init.PeriphInc = DMA_PINC_DISABLE;
    dev->hdma_sdio_rx->Init.MemInc = DMA_MINC_ENABLE;
    dev->hdma_sdio_rx->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dev->hdma_sdio_rx->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dev->hdma_sdio_rx->Init.Mode = DMA_PFCTRL;
    dev->hdma_sdio_rx->Init.Priority = DMA_PRIORITY_LOW;
    dev->hdma_sdio_rx->Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    dev->hdma_sdio_rx->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    dev->hdma_sdio_rx->Init.MemBurst = DMA_MBURST_INC4;
    dev->hdma_sdio_rx->Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(dev->hdma_sdio_rx) != HAL_OK) {
        assert(NULL);
    }

    __HAL_LINKDMA(dev->hsd, hdmarx, hdma_sdio_rx);

    /* SDIO_TX Init */
    dev->hdma_sdio_tx->Instance = DMA2_Stream6;
    dev->hdma_sdio_tx->Init.Channel = DMA_CHANNEL_4;
    dev->hdma_sdio_tx->Init.Direction = DMA_MEMORY_TO_PERIPH;
    dev->hdma_sdio_tx->Init.PeriphInc = DMA_PINC_DISABLE;
    dev->hdma_sdio_tx->Init.MemInc = DMA_MINC_ENABLE;
    dev->hdma_sdio_tx->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dev->hdma_sdio_tx->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dev->hdma_sdio_tx->Init.Mode = DMA_PFCTRL;
    dev->hdma_sdio_tx->Init.Priority = DMA_PRIORITY_LOW;
    dev->hdma_sdio_tx->Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    dev->hdma_sdio_tx->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    dev->hdma_sdio_tx->Init.MemBurst = DMA_MBURST_INC4;
    dev->hdma_sdio_tx->Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(dev->hdma_sdio_tx) != HAL_OK) {
        assert(NULL);
    }

    __HAL_LINKDMA(dev->hsd, hdmatx, hdma_sdio_tx);

    /* SDIO interrupt Init */

    HAL_NVIC_SetPriority(SDIO_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);

    return HAL_STATUS_OK;
}

static HAL_Status sdcHwDeinit(HAL_SDC_ID sdioId){
    HAL_Status ret = HAL_STATUS_OK;
    SdcDev_t *dev = &sdcDev[sdioId];
    sdioClkDisable(sdioId);
    HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_SDC, sdioId), 0);


    /* SDIO DMA DeInit */
    HAL_DMA_DeInit(dev->hsd->hdmarx);
    HAL_DMA_DeInit(dev->hsd->hdmatx);

    /* SDIO interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDIO_IRQn);

    return ret;
}

HAL_Status hal_sdcInit(HAL_SDC_ID sdcId){
    HAL_Status ret = HAL_STATUS_OK;
    ret = sdioHwInit(sdcId);
    return ret;
}

HAL_Status hal_sdcDeinit(HAL_SDC_ID sdcId){
    HAL_Status ret = HAL_STATUS_OK;
    ret = sdcHwDeinit(sdcId);
    return ret;
}