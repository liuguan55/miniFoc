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

#include "common/board/board.h"
#include <driver/component/flash/sfud/sfud.h>
#include <stdarg.h>
#include "driver/hal/hal_spi.h"
#include "common/framework/util/MiniCommon.h"
#include "driver/hal/hal_uart.h"

static HAL_Mutex flashMutex = NULL;

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

/* about 100 microsecond delay */
static void retry_delay_100us(void) {
    uint32_t delay = 120;
    while (delay--);
}

static void spi_lock(const sfud_spi *spi) {
    UNUSED(spi);
    HAL_MutexLock(&flashMutex, HAL_OS_WAIT_FOREVER);
}

static void spi_unlock(const sfud_spi *spi) {
    UNUSED(spi);
    HAL_MutexUnlock(&flashMutex);
}

uint8_t spi_write_and_read(uint8_t data) {
    uint8_t rx = 0;
    HAL_spiTransmitReceive(BOARD_FLASH_SPI_ID, &data, &rx, 1, 1000);

    return rx;
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size) {
    UNUSED(spi);
    sfud_err result = SFUD_SUCCESS;
    /**
     * add your spi write and read code
     */
    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }

    if (HAL_spiOpen(BOARD_FLASH_SPI_ID, BOARD_FLASH_SPI_SS_SEL, 100) != HAL_STATUS_OK){
        return SFUD_ERR_TIMEOUT;
    }
    HAL_spiCsEnable(BOARD_FLASH_SPI_ID);

    for (size_t kI = 0; kI < write_size; ++kI) {
        spi_write_and_read(write_buf[kI]);
    }

    for (size_t kJ = 0; kJ < read_size; ++kJ) {
        read_buf[kJ] = spi_write_and_read(SFUD_DUMMY_DATA);
    }

    HAL_spiCsDisable(BOARD_FLASH_SPI_ID);
    HAL_spiClose(BOARD_FLASH_SPI_ID);

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

    HAL_MutexInit(&flashMutex);
    switch (flash->index) {
        case SFUD_W25Q16X_DEVICE_INDEX: {
            flash->spi.wr = spi_write_read;
            flash->spi.lock = spi_lock;
            flash->spi.unlock = spi_unlock;
            flash->spi.user_data = NULL;
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
    memset(log_buf, 0, sizeof(log_buf));

    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    snprintf(log_buf, sizeof(log_buf), "[SFUD](%s:%ld)" , file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf + strlen(log_buf), sizeof(log_buf) - strlen(log_buf), format, args);
    va_end(args);

    log_buf[strlen(log_buf)] = '\n';
    log_buf[strlen(log_buf)] = '\0';

    HAL_UartSend(BOARD_MAIN_UART_ID, (uint8_t *)log_buf, strlen(log_buf));
}

/**
 * This function is print routine info.
 *
 * @param format driver format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    memset(log_buf, 0, sizeof(log_buf));
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    snprintf(log_buf, sizeof(log_buf), "[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf+ strlen(log_buf), sizeof(log_buf) - strlen(log_buf), format, args);
    va_end(args);

    log_buf[strlen(log_buf)] = '\n';
    log_buf[strlen(log_buf)] = '\0';

    HAL_UartSend(BOARD_MAIN_UART_ID, (uint8_t *)log_buf, strlen(log_buf));
}
