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
// Created by 86189 on 2023/11/24.
//

#include "driver/hal/hal_can.h"
#include "driver/hal/hal_os.h"
#include "driver/hal/hal_gpio.h"
#include "sys/MiniDebug.h"
#include "driver/hal/hal_board.h"
#include "driver/driver_chip.h"

typedef struct {
    CAN_HandleTypeDef hcan;
    HAL_Semaphore semaphore;
    HAL_Mutex mutex;
    CAN_RX_CALLBACK rxCallback;
}CANPrivate_t;

static CANPrivate_t *gCanPrivate[HAL_CAN_NR] = {0};



__STATIC_INLINE  CANPrivate_t *CANPrivateGet(HAL_CAN_ID id)
{
    if (id >= HAL_CAN_NR)
        return NULL;

    return gCanPrivate[id];
}

__STATIC_INLINE   CANPrivate_t *CANPrivateSet(HAL_CAN_ID id, CANPrivate_t *pCanPrivate)
{
    if (id >= HAL_CAN_NR)
        return NULL;

    gCanPrivate[id] = pCanPrivate;
    return pCanPrivate;
}

__STATIC_INLINE  void CANClockEnable(HAL_CAN_ID id)
{
    switch (id){
        case HAL_CAN_0:
            __HAL_RCC_CAN1_CLK_ENABLE();
            break;
        case HAL_CAN_1:
            break;
        default:
            break;
    }
}

__STATIC_INLINE  void CANClockDisable(HAL_CAN_ID id)
{
    switch (id){
        case HAL_CAN_0:
            __HAL_RCC_CAN1_CLK_DISABLE();
            break;
        case HAL_CAN_1:
            break;
        default:
            break;
    }
}

__STATIC_INLINE  CAN_TypeDef* CANInstance(HAL_CAN_ID id)
{
    switch (id){
        case HAL_CAN_0:
            return CAN1;
        case HAL_CAN_1:
        default:
            return NULL;
    }
}

__STATIC_INLINE  uint32_t  CANMode(HAL_CAN_MODE mode)
{
    switch (mode) {
        case HAL_CAN_MODE_NORMAL:
            return CAN_MODE_NORMAL;
        case HAL_CAN_MODE_LOOPBACK:
            return CAN_MODE_LOOPBACK;
        case HAL_CAN_MODE_SILENT:
            return CAN_MODE_SILENT;
        case HAL_CAN_MODE_SILENT_LOOPBACK:
            return CAN_MODE_SILENT_LOOPBACK;
        default:
            return CAN_MODE_NORMAL;
    }
}



/**
  * @brief This function handles USB high priority or CAN TX interrupts.
  */
void USB_HP_CAN1_TX_IRQHandler(void)
{
    /* USER CODE BEGIN USB_HP_CAN1_TX_IRQn 0 */
    CANPrivate_t *p = CANPrivateGet(HAL_CAN_0);
    if (p){
        HAL_CAN_IRQHandler(&p->hcan);
    }
}

/**
  * @brief This function handles USB low priority or CAN RX0 interrupts.
  */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 0 */

    /* USER CODE END USB_LP_CAN1_RX0_IRQn 0 */
    CANPrivate_t *p = CANPrivateGet(HAL_CAN_0);
    if (p){
        HAL_CAN_IRQHandler(&p->hcan);
    }
    /* USER CODE BEGIN USB_LP_CAN1_RX0_IRQn 1 */

    /* USER CODE END USB_LP_CAN1_RX0_IRQn 1 */
}

/**
  * @brief This function handles CAN RX1 interrupt.
  */
void CAN1_RX1_IRQHandler(void)
{
    /* USER CODE BEGIN CAN1_RX1_IRQn 0 */

    /* USER CODE END CAN1_RX1_IRQn 0 */
    CANPrivate_t *p = CANPrivateGet(HAL_CAN_0);
    if (p){
        HAL_CAN_IRQHandler(&p->hcan);
    }
    /* USER CODE BEGIN CAN1_RX1_IRQn 1 */

    /* USER CODE END CAN1_RX1_IRQn 1 */
}

/**
  * @brief This function handles CAN SCE interrupt.
  */
void CAN1_SCE_IRQHandler(void)
{
    /* USER CODE BEGIN CAN1_SCE_IRQn 0 */

    /* USER CODE END CAN1_SCE_IRQn 0 */
    CANPrivate_t *p = CANPrivateGet(HAL_CAN_0);
    if (p){
        HAL_CAN_IRQHandler(&p->hcan);
    }
    /* USER CODE BEGIN CAN1_SCE_IRQn 1 */

    /* USER CODE END CAN1_SCE_IRQn 1 */
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t Rx_data[8];
    HAL_StatusTypeDef ret = HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, Rx_data);
    if (ret != HAL_OK){
        HAL_ERR("HAL_CAN_GetRxMessage error %d\n", ret);
        return;
    }

    if (RxHeader.IDE == CAN_ID_STD){
        HAL_CAN_ID id = HAL_CAN_0;
        if (hcan->Instance == CAN1){
            id = HAL_CAN_0;
        }
//        else if (hcan->Instance == CAN2){
//            id = HAL_CAN_ID_1;
//        }

        CANPrivate_t *pCanPrivate = CANPrivateGet(id);
        if (pCanPrivate == NULL){
            HAL_ERR("CANPrivateGet error\n");
            return;
        }

        if (pCanPrivate->rxCallback != NULL){
            CAN_MSG msg = {
                    .id = RxHeader.StdId,
                    .len = RxHeader.DLC,
                    .data = Rx_data,
                    .timestamp = RxHeader.Timestamp,
            };
            pCanPrivate->rxCallback(id, &msg);
        }
    }
}


static HAL_Status HAL_CANHwInit(HAL_CAN_ID id, CAN_CONFIG *pconfig)
{
    if (id >= HAL_CAN_NR)
        return HAL_STATUS_ERROR;

    CANPrivate_t *pCanPrivate = CANPrivateGet(id);
    if (pCanPrivate == NULL)
        return HAL_STATUS_ERROR;

    CANClockEnable(id);
    HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_CAN, id), 0);

    uint32_t prescaler = 4;
    uint32_t syncJumpWidth = CAN_SJW_1TQ;
    uint32_t timeSeg1 = CAN_BS1_4TQ;
    uint32_t timeSeg2 = CAN_BS2_4TQ;

    if (pconfig->baudrate == HAL_CAN_1000K){
        prescaler = 4;
        syncJumpWidth = CAN_SJW_1TQ;
        timeSeg1 = CAN_BS1_4TQ;
        timeSeg2 = CAN_BS2_4TQ;
    }


    CAN_HandleTypeDef *hcan = &pCanPrivate->hcan;
    hcan->Instance = CANInstance(id);
    hcan->Init.Prescaler = prescaler;
    hcan->Init.Mode = CANMode(pconfig->mode);
    hcan->Init.SyncJumpWidth = syncJumpWidth;
    hcan->Init.TimeSeg1 = timeSeg1;
    hcan->Init.TimeSeg2 = timeSeg2;
    hcan->Init.TimeTriggeredMode = DISABLE;
    hcan->Init.AutoBusOff = DISABLE;
    hcan->Init.AutoWakeUp = DISABLE;
    hcan->Init.AutoRetransmission = ENABLE;
    hcan->Init.ReceiveFifoLocked = DISABLE;
    hcan->Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(hcan) != HAL_OK)
        return HAL_STATUS_ERROR;

    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = pconfig->filter_id >> 16;
    sFilterConfig.FilterIdLow = pconfig->filter_id & 0xFFFF;
    sFilterConfig.FilterMaskIdHigh = pconfig->filter_mask >> 16;
    sFilterConfig.FilterMaskIdLow = pconfig->filter_mask & 0xFFFF;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;
    if (HAL_CAN_ConfigFilter(hcan, &sFilterConfig) != HAL_OK)
        return HAL_STATUS_ERROR;

    if (HAL_CAN_Start(hcan) != HAL_OK)
        return HAL_STATUS_ERROR;


    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);

    if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
        return HAL_STATUS_ERROR;

    return HAL_STATUS_OK;
}

static void HAL_CANHwDeInit(HAL_CAN_ID id)
{
    if (id >= HAL_CAN_NR)
        return;

    CANPrivate_t *pCanPrivate = CANPrivateGet(id);
    if (pCanPrivate == NULL)
        return;

    CAN_HandleTypeDef *hcan = &pCanPrivate->hcan;
    HAL_CAN_DeInit(hcan);

    CANClockDisable(id);
    HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_CAN, id), 0);
    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
}

HAL_Status HAL_CANinit(HAL_CAN_ID id, CAN_CONFIG *pconfig)
{
    if (id >= HAL_CAN_NR)
        return HAL_STATUS_ERROR;

    CANPrivate_t *pCanPrivate = CANPrivateGet(id);
    if (pCanPrivate == NULL){
        pCanPrivate = HAL_Malloc(sizeof(CANPrivate_t));
        if (pCanPrivate == NULL)
            return HAL_STATUS_ERROR;

        memset(pCanPrivate, 0, sizeof(CANPrivate_t));
        CANPrivateSet(id, pCanPrivate);
    }

    if (HAL_CANHwInit(id, pconfig) != HAL_STATUS_OK)
        return HAL_STATUS_ERROR;

    HAL_SemaphoreInit(&pCanPrivate->semaphore,1,1);
    HAL_MutexInit(&pCanPrivate->mutex);

    return HAL_STATUS_OK;
}

HAL_Status HAL_CANDeInit(HAL_CAN_ID id)
{
    if (id >= HAL_CAN_NR)
        return HAL_STATUS_ERROR;

    CANPrivate_t *pCanPrivate = CANPrivateGet(id);
    if (pCanPrivate == NULL)
        return HAL_STATUS_ERROR;

    HAL_CANHwDeInit(id);

    HAL_SemaphoreDeinit(&pCanPrivate->semaphore);
    HAL_MutexDeinit(&pCanPrivate->mutex);

    HAL_Free(pCanPrivate);
    CANPrivateSet(id, NULL);

    return HAL_STATUS_OK;
}

HAL_Status HAL_CANSetRxCallback(HAL_CAN_ID id, CAN_RX_CALLBACK callback)
{
    if (id >= HAL_CAN_NR)
        return HAL_STATUS_ERROR;

    CANPrivate_t *pCanPrivate = CANPrivateGet(id);
    if (pCanPrivate == NULL)
        return HAL_STATUS_ERROR;

    pCanPrivate->rxCallback = callback;

    return HAL_STATUS_OK;
}

int32_t HAL_CANSendMsg(HAL_CAN_ID id, CAN_MSG *msg)
{
    if (id >= HAL_CAN_NR)
        return -HAL_STATUS_ERROR;

    if (msg == NULL)
        return -HAL_STATUS_ERROR;

    CANPrivate_t *pCanPrivate = CANPrivateGet(id);
    if (pCanPrivate == NULL)
        return -HAL_STATUS_ERROR;

    CAN_HandleTypeDef *hcan = &pCanPrivate->hcan;
    CAN_TxHeaderTypeDef TxHeader;
    uint8_t Tx_data[8] = {0};

    int32_t sendLen = 0;
    int32_t totalLen = msg->len;

    while (sendLen < totalLen) {
        int32_t len = totalLen - sendLen;
        if (len > 8)
            len = 8;

        memcpy(Tx_data, msg->data + sendLen, len);
        sendLen += len;
        if (HAL_CAN_GetTxMailboxesFreeLevel(hcan) <= 0){
            return sendLen;
        }

        uint32_t txMailbox = CAN_TX_MAILBOX0;
        if ((hcan->Instance->TSR & CAN_TSR_TME0) != RESET) {
            //Check the status of sender mailbox 0. If mailbox 0 is idle, put the data to be sent into FIFO0
            txMailbox = CAN_TX_MAILBOX0;
        } else if ((hcan->Instance->TSR & CAN_TSR_TME1) != RESET) {
            txMailbox = CAN_TX_MAILBOX1;
        } else if ((hcan->Instance->TSR & CAN_TSR_TME2) != RESET) {
            txMailbox = CAN_TX_MAILBOX2;
        } else {
            return sendLen;
        }

        TxHeader.StdId = msg->id;
        TxHeader.ExtId = 0;
        TxHeader.RTR = CAN_RTR_DATA;
        TxHeader.IDE = CAN_ID_STD;
        TxHeader.DLC = len;
        memcpy(Tx_data, msg->data, msg->len);
        if (HAL_CAN_AddTxMessage(hcan, &TxHeader, Tx_data, &txMailbox) != HAL_OK)
            return sendLen;
    }


    return HAL_STATUS_OK;
}