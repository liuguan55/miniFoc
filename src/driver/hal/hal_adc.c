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
#include "sys/MiniDebug.h"
#include "driver/hal/hal_board.h"
#include "driver/driver_chip.h"


typedef struct {
    ADC_HandleTypeDef *adc;
    TIM_HandleTypeDef tim;
    DMA_HandleTypeDef dma;
    uint8_t dmaBuffer[HAL_ADC_CH_NR];
    uint8_t adcBuffer[HAL_ADC_CH_NR];
    HAL_Semaphore sem;
} AdcPrivate_t;

static AdcPrivate_t *adcPrivate[HAL_ADC_NR];

static ADC_TypeDef *const adcInstance[HAL_ADC_NR] = {
        ADC1,
        ADC2,
};
static ADC_TypeDef * ADCInstanceGet(HAL_ADC_ID id) {
    return adcInstance[id];
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

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    for (int i = 0; i < HAL_ADC_NR; ++i) {
        AdcPrivate_t *pAdcPrivate = adcPrivate[i];
        if (pAdcPrivate && pAdcPrivate->adc == hadc) {
            for (int j = 0; j < HAL_ADC_CH_NR; ++j) {
                pAdcPrivate->adcBuffer[j] = pAdcPrivate->dmaBuffer[j];
            }

            HAL_SemaphoreRelease(pAdcPrivate->sem);
            break;
        }
    }
}

static void ADCClockEnable(HAL_ADC_ID id) {
    if (id == HAL_ADC_1) {
        __HAL_RCC_ADC1_CLK_ENABLE();
    } else if (id == HAL_ADC_2) {
        __HAL_RCC_ADC2_CLK_ENABLE();
    }
}

static void  ADCClockDisable(HAL_ADC_ID id) {
    if (id == HAL_ADC_1) {
        __HAL_RCC_ADC1_CLK_DISABLE();
    } else if (id == HAL_ADC_2) {
        __HAL_RCC_ADC2_CLK_DISABLE();
    }
}
/**
 * @brief adc hardware init
 * @param id
 */
static void HAL_adcHwInit(HAL_ADC_ID id) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    ADC_HandleTypeDef *pAdc = pAdcPrivate->adc;

    ADCClockEnable(id);
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_ADC, id), 0);

    pAdc->Instance = ADCInstanceGet(id);
    pAdc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    pAdc->Init.Resolution = ADC_RESOLUTION_12B;
    pAdc->Init.ScanConvMode = ENABLE;
    pAdc->Init.ContinuousConvMode = DISABLE;
    pAdc->Init.DiscontinuousConvMode = DISABLE;
    pAdc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    pAdc->Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    pAdc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    pAdc->Init.NbrOfConversion = 1;
    pAdc->Init.DMAContinuousRequests = ENABLE;
    pAdc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(pAdc) != HAL_OK) {
        assert(NULL);
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    if (id == HAL_ADC_1) {
        ADC_ChannelConfTypeDef sConfig;
        sConfig.Channel = ADC_CHANNEL_4;
        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
        if (HAL_ADC_ConfigChannel(pAdc, &sConfig) != HAL_OK) {
            assert(NULL);
        }

        /* ADC1 DMA Init */
        DMA_HandleTypeDef *pDma = &pAdcPrivate->dma;
        pDma->Instance = DMA2_Stream0;
        pDma->Init.Channel = DMA_CHANNEL_0;
        pDma->Init.Direction = DMA_PERIPH_TO_MEMORY;
        pDma->Init.PeriphInc = DMA_PINC_DISABLE;
        pDma->Init.MemInc = DMA_MINC_ENABLE;
        pDma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        pDma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        pDma->Init.Mode = DMA_CIRCULAR;
        pDma->Init.Priority = DMA_PRIORITY_LOW;
        pDma->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(pDma) != HAL_OK) {
            assert(NULL);
        }
    }
    __HAL_LINKDMA(pAdc, DMA_Handle, pAdcPrivate->dma);
    pAdcPrivate->adc = pAdc;

    /*configuration tim for adc */
    __HAL_RCC_TIM1_CLK_ENABLE();
    TIM_HandleTypeDef *pTim = &pAdcPrivate->tim;
    memset(pTim, 0, sizeof(TIM_HandleTypeDef));
    /* USER CODE END TIM1_Init 1 */
    pTim->Instance = TIM1;
    pTim->Init.Prescaler = 83;
    pTim->Init.CounterMode = TIM_COUNTERMODE_UP;
    pTim->Init.Period = 999;
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
    sConfigOC.Pulse = 500;
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

    HAL_ADC_Start_DMA(pAdc, (uint32_t *) pAdcPrivate->dmaBuffer, HAL_ADC_CH_NR);
    HAL_TIM_PWM_Start(pTim, TIM_CHANNEL_1);
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

    HAL_adcHwInit(id);

    HAL_SemaphoreInit(pAdcPrivate->sem, 0, 1);
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


uint16_t HAL_adcRead(HAL_ADC_ID id, HAL_ADC_CH ch, uint32_t msec) {
    AdcPrivate_t *pAdcPrivate = adcPrivate[id];
    if (pAdcPrivate == NULL) {
        HAL_adcInit(id);
    }

    HAL_SemaphoreWait(pAdcPrivate->sem, msec);

    return pAdcPrivate->adcBuffer[ch];
}