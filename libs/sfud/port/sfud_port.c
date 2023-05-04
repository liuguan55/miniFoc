/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
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
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include "spi.h"
#include "../../../framework/MiniCommon.h"

extern SPI_HandleTypeDef hspi2;

typedef struct {
    SPI_HandleTypeDef *spix;
    GPIO_TypeDef *cs_gpiox;
    uint16_t cs_gpio_pin;
} spi_user_data, *spi_user_data_t;

static osMutexId_t flashMutex = NULL;
static const osMutexAttr_t flashMutexAtr = {
        .name = "nor flash mutex"
};

static spi_user_data spi2 = {.spix = &hspi2, .cs_gpiox = GPIOC, .cs_gpio_pin = GPIO_PIN_5};
static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

/* about 100 microsecond delay */
static void retry_delay_100us(void) {
    uint32_t delay = 120;
    while (delay--);
}

static void spi_lock(const sfud_spi *spi) {
    UNUSED(spi);
    osMutexAcquire(flashMutex, portMAX_DELAY);
}

static void spi_unlock(const sfud_spi *spi) {
    UNUSED(spi);
    osMutexRelease(flashMutex);
}

uint8_t spi_write_and_read(SPI_HandleTypeDef *hspi, uint8_t data) {
    uint8_t rx = 0;

    HAL_SPI_TransmitReceive(hspi, &data, &rx, 1, 1000);

    return rx;
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
//  uint8_t send_data, read_data;
    /**
     * add your spi write and read code
     */
    spi_user_data_t spi_dev = (spi_user_data_t) spi->user_data;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }
    if (spi_dev->cs_gpiox != NULL) {
        HAL_GPIO_WritePin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_RESET);
    }

    for (int kI = 0; kI < write_size; ++kI) {
        spi_write_and_read(spi_dev->spix, write_buf[kI]);
    }

    for (int kJ = 0; kJ < read_size; ++kJ) {
        read_buf[kJ] = spi_write_and_read(spi_dev->spix, SFUD_DUMMY_DATA);
    }

    if (spi_dev->cs_gpiox != NULL) {
        HAL_GPIO_WritePin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_SET);
    }

    return result;
}

#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
                          uint8_t *read_buf, size_t read_size) {
  sfud_err result = SFUD_SUCCESS;

  /**
   * add your qspi read flash data code
   */

  return result;
}
#endif /* SFUD_USING_QSPI */

sfud_err sfud_spi_port_init(sfud_flash *flash) {
    sfud_err result = SFUD_SUCCESS;

    flashMutex = osMutexNew(&flashMutexAtr);
    switch (flash->index) {
        case SFUD_W25Q16X_DEVICE_INDEX: {
            flash->spi.wr = spi_write_read;
            flash->spi.lock = spi_lock;
            flash->spi.unlock = spi_unlock;
            flash->spi.user_data = &spi2;
            /* about 100 microsecond delay */
            flash->retry.delay = retry_delay_100us;
            /* adout 60 seconds timeout */
            flash->retry.times = 10 * 10000;

            break;
        }
    }

    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format driver format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format driver format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}
