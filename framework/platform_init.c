//
// Created by 邓志君 on 2023/4/16.
//
#include <stdio.h>
#include "MiniCommon.h"

#define PLATFORM_SHOW_DEBUG_INFO     1//0   /* for internal debug only */


static void test_env(void) {
  uint32_t i_boot_times = NULL;
  char *c_old_boot_times, c_new_boot_times[11] = {0};

  /* get the boot count number from Env */
  c_old_boot_times = ef_get_env("boot_times");
  assert_param(c_old_boot_times);
  i_boot_times = atol(c_old_boot_times);
  /* boot count +1 */
  i_boot_times++;
  printf("The system now boot %d times\n\r", i_boot_times);
  /* interger to string */
  sprintf(c_new_boot_times, "%ld", i_boot_times);
  /* set and store the boot count number to Env */
  ef_set_env("boot_times", c_new_boot_times);
  ef_save_env();
}

void RunTimeStatsTask(void) {
  __ccmram static char InfoBuffer[1024] = {0} ;

  memset(InfoBuffer, 0, sizeof(InfoBuffer));
  /*打印任务信息*/
  vTaskList(InfoBuffer);
  printf("%s", InfoBuffer);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN,
                 top, RunTimeStatsTask, run
                     times
                     stats
                     task);

static void platform_show_info(void)
{
  uint8_t dbg_en = 0;

#if PLATFORM_SHOW_DEBUG_INFO
  dbg_en = 1;
#endif /* PLATFORM_SHOW_INFO */

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

  printf("sram heap space [%p, %p), total size %u Bytes\n",
          __heap_start__, __heap_end__,
          __heap_end__ - __heap_start__);

  printf("cpu  clock %9u Hz\n", HAL_RCC_GetHCLKFreq());
  printf("sys  clock %9u Hz\n", HAL_RCC_GetSysClockFreq());
  printf("pclk  clock %9u Hz\n", HAL_RCC_GetPCLK1Freq());
  printf("pclk2  clock %9u Hz\n", HAL_RCC_GetPCLK2Freq());

  printf("====================================================================\n\n");
}

static platform_level1_init(void)
{
  cm_backtrace_init(FIRMWARE_NAME, HARDWARE_VERSION, SOFTWARE_VERSION);

  extern int debug_init(void);
  debug_init();

  MiniFatfs_init();
  MiniShell_Init();
  MiniShell_Run();

  easyflash_init();
  test_env();
//  /* initialize EasyLogger */
  elog_flash_init();
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

  MiniButton_init();
  MiniButton_run();

  MiniMotor_init();
}

void platform_init(void)
{
  platform_show_info();
  platform_level1_init();
}