//
// Created by 邓志君 on 2023/4/16.
//
#include <stdio.h>
#include "MiniCommon.h"
#include "platform_init.h"
#include "console/miniConsole.h"
#include "driver/hal/hal_wdg.h"
#include "driver/hal/hal_rnd.h"
#include "driver/hal/hal_pwm.h"
#include "driver/hal/hal_usb.h"
#include "driver/hal/hal_spi.h"
#include "driver/hal/hal_crc.h"
#include "driver/hal/hal_sdio.h"


#define PLATFORM_SHOW_DEBUG_INFO     1//0   /* for internal debug only */


static void platform_show_info(void) {
#if PLATFORM_SHOW_DEBUG_INFO
    extern uint8_t __text_start__[];
    extern uint8_t __text_end__[];
    extern uint8_t __etext[];
    extern uint8_t __data_start__[];
    extern uint8_t __data_end__[];
    extern uint8_t __bss_start__[];
    extern uint8_t __bss_end__[];
    extern uint8_t __heap_start__[];
    extern uint8_t __heap_end__[];
    extern uint8_t __end__[];
    extern uint8_t end[];
    extern uint8_t __HeapLimit[];
    extern uint8_t __StackLimit[];
    extern uint8_t __StackTop[];
    extern uint8_t __stack[];
    extern uint8_t _estack[];

    printf("\nplatform information ===============================================\n");
    printf("MIni SDK "SOFTWARE_VERSION" "__DATE__" "__TIME__"\n\n");

    printf("__text_start__ %p\n", __text_start__);
    printf("__text_end__   %p\n", __text_end__);
    printf("__etext        %p\n", __etext);
    printf("__data_start__ %p\n", __data_start__);
    printf("__data_end__   %p\n", __data_end__);
    printf("__bss_start__  %p\n", __bss_start__);
    printf("__bss_end__    %p\n", __bss_end__);
    printf("__end__        %p\n", __end__);
    printf("end            %p\n", end);
    printf("__HeapLimit    %p\n", __HeapLimit);
    printf("__StackLimit   %p\n", __StackLimit);
    printf("__StackTop     %p\n", __StackTop);
    printf("__stack        %p\n", __stack);
    printf("_estack        %p\n", _estack);

    printf("sram_heap heap space [%p, %p), total size %u Bytes\n",
           __heap_start__, __heap_end__,
           __heap_end__ - __heap_start__);

    printf("cpu  clock %9lu Hz\n", HAL_RCC_GetHCLKFreq());
    printf("sys  clock %9lu Hz\n", HAL_RCC_GetSysClockFreq());
    printf("pclk  clock %9lu Hz\n", HAL_RCC_GetPCLK1Freq());
    printf("pclk2  clock %9lu Hz\n", HAL_RCC_GetPCLK2Freq());

    printf("====================================================================\n\n");

#endif /* PLATFORM_SHOW_INFO */
}

#if PRJCONF_WATCHDOG_EN

void platform_wdgStart(void) {
    wdgConfig_t wdgConfig;
    wdgConfig.timeout = PRjCONF_WATCHDOG_TIMEOUT;
    hal_wdgInit(&wdgConfig);
}

#endif

#if PRJCONF_SPIFLASH_EN
void platform_flashInit(void) {
    board_spiFlash_init(BOARD_FLASH_SPI_ID);
    if (sfud_init() != SFUD_SUCCESS) {
        printf("SFUD init failed!\n");
        return;
    }
    printf("SFUD init success!\n");

    easyflash_init();
    /* initialize EasyLogger */
    elog_flash_init();
}
#endif

#if PRJCONF_LOG_EN

void platform_logInit(void) {
    elog_init();
//  /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME | ELOG_FMT_P_INFO);
    elog_set_text_color_enabled(true);
    elog_start();
}

#endif

#if PRJCONF_BTN_EN

void platform_btnInit(void) {
    MiniButton_init();
    MiniButton_run();
}

#endif

void platform_rngInit(void) {
    hal_rndInit();
    hal_crcInit();
}

#if PRJCONF_MOTOR_EN
void platform_motorInit(void){
    HAL_PWM_CFG cfg = {
            .period = 1000,
            .mode = HAL_PWM_COUNTER_MODE_CENTER,
            .duty = 0
    };
    HAL_pwmInit(HAL_PWM_ID_1, &cfg);
    HAL_pwmInit(HAL_PWM_ID_2, &cfg);

    MiniMotor_init();
}
#endif

#if PRJCONF_USB_EN
void platform_usbInit(void) {
    HAL_usbInit(HAL_USB_FS);
}

#endif

#if PRJCONF_SDCARD_EN

void platform_sdcardInit(void) {
    hal_sdcInit(HAL_SDC_1);
}

#endif

static void platform_level1_init(void) {
    cm_backtrace_init(FIRMWARE_NAME, HARDWARE_VERSION, SOFTWARE_VERSION);

    extern int debug_init(void);
    debug_init();

    platform_rngInit();

#if PRJCONF_WATCHDOG_EN
    platform_wdgStart();
#endif

#if PRJCONF_CONSOLE_EN
    MiniConsoleConfig_t cparam;
    cparam.uartId = BOARD_MAIN_UART_ID;
    cparam.stackSize = PRJCONF_CONSOLE_STACK_SIZE;
    miniConsoleInit(&cparam);
#endif
}

static void platform_level2_init(void) {
#if PRJCONF_SPIFLASH_EN
    platform_flashInit();
#endif

#if PRJCONF_LOG_EN
    platform_logInit();
#endif

#if PRJCONF_USB_EN
    platform_usbInit();
#endif

#if PRJCONF_SDCARD_EN
    platform_sdcardInit();
#endif
}

void platform_level3_init(void) {
#if PRJCONF_VFS_FILE_SYSTEM_EN
    vfsSystemInit();
#endif

#if PRJCONF_BTN_EN
    platform_btnInit();
#endif

#if PRJCONF_MOTOR_EN
    platform_motorInit();
#endif

#if PRJCONF_UNITY_EN
    UnityManager_run();
#endif
}

void platform_init(void) {
    platform_level1_init();
    platform_level2_init();
    platform_level3_init();
    platform_show_info();
}