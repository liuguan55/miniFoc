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
// Created by 86189 on 2023/5/28.
//

#include "driver/hal/hal_adc.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_gpio.h"
#include "sys/MiniDebug.h"
#include "driver/hal/hal_board.h"
#include "driver/driver_chip.h"

typedef struct {
    ADC_HandleTypeDef *adc;
    TIM_HandleTypeDef tim;
    DMA_HandleTypeDef dma;
    uint16_t *dmaBuffer;
    uint32_t dmaBufferSize;
    uint16_t *adcBuffer;
    uint32_t adcBufferSize;
    HAL_Semaphore sem;
    HAL_ADC_Callback_t callback;

    ADC_Config_t *config;
} AdcPrivate_t;

static AdcPrivate_t *adcPrivate[HAL_ADC_NR];

static ADC_TypeDef *const adcInstance[HAL_ADC_NR] = {
        ADC1,
#if defined(TARGET_MCU_STM32F1) || defined(TARGET_MCU_STM32F4)
        ADC2,
#endif
};

static ADC_TypeDef * ADCInstanceGet(HAL_ADC_ID id) {
    return adcInstance[id];
}

static void timerClockEnable(TIM_TypeDef *timer){
    if (timer == TIM1){
        __HAL_RCC_TIM1_CLK_ENABLE();
    }
#if defined(TARGET_MCU_STM32F1) || defined(TARGET_MCU_STM32F4)
    else if (timer == TIM2){
        __HAL_RCC_TIM2_CLK_ENABLE();
    }else if (timer == TIM3) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }else if (timer == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();
    }
#endif
}

/**
 * @brief This function handles DMA2 stream0 global interrupt.
 * @note  This function is redefined in HAL_Driver/stm32f4xx_hal_dma.c
 */
void DMA2_Stream0_IRQHandler(void) {
    /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

    /* USER CODE END DMA2_Stream0_IRQn 0 */

    AdcPrivate_t *pAdcPrivate = adcPrivate[HAL_ADC_1];
    if (pAdcPrivate) {
        DMA_HandleTypeDef *hdma_adc1 = &pAdcPrivate->dma;
        HAL_DMA_IRQHandler(hdma_adc1);
    }
    /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

    /* USER CODE END DMA2_Stream0_IRQn 1 */
}

void DMA1_Channel1_IRQHandler(void) {
    /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

    /* USER CODE END DMA2_Stream0_IRQn 0 */

    AdcPrivate_t *pAdcPrivate = adcPrivate[HAL_ADC_1];
    if (pAdcPrivate) {
        DMA_HandleTypeDef *hdma_adc1 = &pAdcPrivate->dma;
        HAL_DMA_IRQHandler(hdma_adc1);
    }
    /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

    /* USER CODE END DMA2_Stream0_IRQn 1 */
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    for (int i = 0; i < HAL_ADC_NR; ++i) {
        AdcPrivate_t *pAdcPrivate = adcPrivate[i];
        if (pAdcPrivate && pAdcPrivate->adc == hadc) {
            memcpy(pAdcPrivate->adcBuffer, pAdcPrivate->dmaBuffer, pAdcPrivate->dmaBufferSize * sizeof(uint16_t));
            pAdcPrivate->callback(pAdcPrivate->adcBuffer, pAdcPrivate->adcBufferSize);

            HAL_SemaphoreRelease(pAdcPrivate->sem);
            break;
        }
    }
}

static void ADCClockEnable(HAL_ADC_ID id) {
#if defined(TARGET_MCU_STM32F1) || defined(TARGET_MCU_STM32F4)
    if (id == HAL_ADC_1) {
        __HAL_RCC_ADC1_CLK_ENABLE();
    } else if (id == HAL_ADC_2) {
        __HAL_RCC_ADC2_CLK_ENABLE();
    }
#elif defined(TARGET_MCU_STM32G0)
    if (id == HAL_ADC_1) {
        __HAL_RCC_ADC_CLK_ENABLE();
    }
#endif
}

static void  ADCClockDisable(HAL_ADC_ID id) {
#if defined(TARGET_MCU_STM32F1) || defined(TARGET_MCU_STM32F4)
    if (id == HAL_ADC_1) {
        __HAL_RCC_ADC1_CLK_DISABLE();
    } else if (id == HAL_ADC_2) {
        __HAL_RCC_ADC2_CLK_DISABLE();
    }
#elif defined(TARGET_MCU_STM32G0)
    if (id == HAL_ADC_1) {
        __HAL_RCC_ADC_CLK_DISABLE();
    }
#endif
}

static void ADCDmaEnable(DMA_Channel_TypeDef *dmaChannelTypeDef) {
    if (dmaChannelTypeDef == DMA1_Channel1) {
        __HAL_RCC_DMA1_CLK_ENABLE();
        /* DMA interrupt init */
        /* DMA2_Stream0_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    }
#if defined(TARGET_MCU_STM32F1) || defined(TARGET_MCU_STM32F4)
    else if (dmaChannelTypeDef == DMA1_Channel2) {
        __HAL_RCC_DMA1_CLK_ENABLE();
        /* DMA interrupt init */
        /* DMA2_Stream0_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    }
#endif
}
/**
 * @brief adc hardware init
 * @param id
 */
static void HAL_adcHwInit(HAL_ADC_ID id) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    ADC_HandleTypeDef *pAdc = pAdcPrivate->adc;
    if (pAdc == NULL) {
        HAL_ERR("adc%d not init", id);
        return;
    }

    board_adc_info_t boardAdcInfo;
    HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_ADC, id), (uint32_t)&boardAdcInfo);

    if(boardAdcInfo.config->channelCount <= 0){
        return;
    }

    ADCClockEnable(id);
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_ADC, id), 0);

    pAdcPrivate->config = boardAdcInfo.config;

    pAdc->Instance = ADCInstanceGet(id);
#ifdef TARGET_MCU_STM32F4
    pAdc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    pAdc->Init.Resolution = ADC_RESOLUTION_12B;
    pAdc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    pAdc->Init.DMAContinuousRequests = ENABLE;
    pAdc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
#endif
    pAdc->Init.ScanConvMode = ENABLE;
    pAdc->Init.ContinuousConvMode = ENABLE;
    pAdc->Init.DiscontinuousConvMode = DISABLE;
    pAdc->Init.ExternalTrigConv = boardAdcInfo.config->externalTrigConv;
    pAdc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    pAdc->Init.NbrOfConversion = boardAdcInfo.config->channelCount;
    if (HAL_ADC_Init(pAdc) != HAL_OK) {
        assert(NULL);
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    for (uint32_t i = 0; i < boardAdcInfo.config->channelCount; ++i) {
        ADC_ChannelConfTypeDef sConfig;
        ADC_Channel_t *adcChannel = &boardAdcInfo.config->channels[i];
        sConfig.Channel = adcChannel->channel;
        sConfig.Rank = adcChannel->rank;
        sConfig.SamplingTime = adcChannel->sampleTime;
        if (HAL_ADC_ConfigChannel(pAdc, &sConfig) != HAL_OK) {
            assert(NULL);
        }
    }

    /* ADC1 DMA Init */
    DMA_HandleTypeDef *pDma = &pAdcPrivate->dma;
    pDma->Instance = boardAdcInfo.config->dmaChannel;
    pDma->Init.Direction = DMA_PERIPH_TO_MEMORY;
    pDma->Init.PeriphInc = DMA_PINC_DISABLE;
    pDma->Init.MemInc = DMA_MINC_ENABLE;
    pDma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    pDma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    pDma->Init.Mode = DMA_CIRCULAR;
    pDma->Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(pDma) != HAL_OK) {
        assert(NULL);
    }
    __HAL_LINKDMA(pAdc, DMA_Handle, pAdcPrivate->dma);
    ADCDmaEnable(pDma->Instance);
    pAdcPrivate->dmaBufferSize = boardAdcInfo.config->channelCount;
    pAdcPrivate->dmaBuffer = (uint16_t *)HAL_Malloc(boardAdcInfo.config->channelCount * sizeof(uint16_t));
    if (pAdcPrivate->dmaBuffer == NULL) {
        assert(NULL);
    }

    pAdcPrivate->adcBufferSize = boardAdcInfo.config->channelCount;
    pAdcPrivate->adcBuffer = (uint16_t *)HAL_Malloc(boardAdcInfo.config->channelCount * sizeof(uint16_t));
    if (pAdcPrivate->adcBuffer == NULL) {
        assert(NULL);
    }
    pAdcPrivate->adc = pAdc;

    if (boardAdcInfo.config->timer){
        /*configuration tim for adc */
        timerClockEnable(boardAdcInfo.config->timer);

        TIM_HandleTypeDef *pTim = &pAdcPrivate->tim;
        memset(pTim, 0, sizeof(TIM_HandleTypeDef));
        /* USER CODE END TIM1_Init 1 */
        pTim->Instance = boardAdcInfo.config->timer;
        pTim->Init.Prescaler = boardAdcInfo.config->prescaler;
        pTim->Init.CounterMode = TIM_COUNTERMODE_UP;
        pTim->Init.Period = boardAdcInfo.config->period;
        pTim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        pTim->Init.RepetitionCounter = 0;
        pTim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_PWM_Init(pTim) != HAL_OK) {
            assert(NULL);
        }

        TIM_MasterConfigTypeDef sMasterConfig = {0};
        TIM_OC_InitTypeDef sConfigOC = {0};
        TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(pTim, &sMasterConfig) != HAL_OK) {
            assert(NULL);
        }
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = 0;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
        sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
        if (HAL_TIM_PWM_ConfigChannel(pTim, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
            assert(NULL);
        }
        sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
        sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
        sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
        sBreakDeadTimeConfig.DeadTime = 0;
        sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
        sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
        sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
        if (HAL_TIMEx_ConfigBreakDeadTime(pTim, &sBreakDeadTimeConfig) != HAL_OK) {
            assert(NULL);
        }
    }
}

/**
 * @brief adc hardware deinit
 * @param id
 */
static void HAL_adcHwDeinit(HAL_ADC_ID id) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        return;
    }

    ADCClockDisable(id);
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_ADC, id), 0);

    HAL_ADC_Stop_DMA(pAdcPrivate->adc);
    HAL_TIM_PWM_Stop(&pAdcPrivate->tim, TIM_CHANNEL_1);
    HAL_TIM_PWM_DeInit(&pAdcPrivate->tim);
    HAL_ADC_DeInit(pAdcPrivate->adc);

    if (pAdcPrivate->dmaBuffer){
        HAL_Free(pAdcPrivate->dmaBuffer);
        pAdcPrivate->dmaBuffer = NULL;
        pAdcPrivate->dmaBufferSize = 0;
    }

    if (pAdcPrivate->adcBuffer){
        HAL_Free(pAdcPrivate->adcBuffer);
        pAdcPrivate->adcBuffer = NULL;
        pAdcPrivate->adcBufferSize = 0;
    }

    HAL_DMA_DeInit(&pAdcPrivate->dma);
}


void HAL_adcInit(HAL_ADC_ID id) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        pAdcPrivate = (AdcPrivate_t *) HAL_Malloc(sizeof(AdcPrivate_t));
        if (pAdcPrivate == NULL) {
            return;
        }
        adcPrivate[id] = pAdcPrivate;
    }

    memset(pAdcPrivate, 0, sizeof(AdcPrivate_t));

    ADC_HandleTypeDef *pAdc = (ADC_HandleTypeDef *) HAL_Malloc(sizeof(ADC_HandleTypeDef));
    if (pAdc == NULL) {
        return;
    }
    memset(pAdc, 0, sizeof(ADC_HandleTypeDef));
    pAdcPrivate->adc = pAdc;

    HAL_adcHwInit(id);

    HAL_SemaphoreInit(&pAdcPrivate->sem, 1, 1);
}

void HAL_adcDeinit(HAL_ADC_ID id) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        return;
    }

    HAL_adcHwDeinit(id);

    HAL_SemaphoreDeinit(pAdcPrivate->sem);

    HAL_Free(pAdcPrivate->adc);
    HAL_Free(pAdcPrivate);
    adcPrivate[id] = NULL;
}

int8_t HAL_adcStart(HAL_ADC_ID id) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        HAL_adcInit(id);
    }

    HAL_ADCEx_Calibration_Start(pAdcPrivate->adc);
    HAL_ADC_Start_DMA(pAdcPrivate->adc, (uint32_t *) pAdcPrivate->dmaBuffer, pAdcPrivate->dmaBufferSize);
    HAL_TIM_PWM_Start(&pAdcPrivate->tim, pAdcPrivate->config->timerChannel);

    return 0;
}

int8_t HAL_adcStop(HAL_ADC_ID id) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        HAL_adcInit(id);
    }

    HAL_ADC_Stop_DMA(pAdcPrivate->adc);
    HAL_TIM_PWM_Stop(&pAdcPrivate->tim, TIM_CHANNEL_1);

    return 0;
}

uint16_t HAL_adcRead(HAL_ADC_ID id, HAL_ADC_CH ch, uint32_t msec) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        HAL_adcInit(id);
    }

    HAL_SemaphoreWait(pAdcPrivate->sem, msec);

    return pAdcPrivate->adcBuffer[ch];
}

uint32_t HAL_adcGetChannelCount(HAL_ADC_ID id) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        HAL_adcInit(id);
    }

    return pAdcPrivate->adcBufferSize;
}

int32_t HAL_adcReadMulti(HAL_ADC_ID id, uint16_t *buffer, uint16_t size, uint32_t msec) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        HAL_adcInit(id);
    }

    HAL_SemaphoreWait(pAdcPrivate->sem, msec);


    memcpy(buffer, pAdcPrivate->adcBuffer, pAdcPrivate->adcBufferSize * sizeof(uint16_t));

    return size;
}

void HAL_adcSetCallback(HAL_ADC_ID id, HAL_ADC_Callback_t callback) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        HAL_adcInit(id);
    }

    pAdcPrivate->callback = callback;
}
