#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)

# specify cross compilers and tools
SET(CMAKE_C_COMPILER_WORKS 8)
SET(CMAKE_C_COMPILER arm-none-eabi-gcc)
SET(CMAKE_CXX_COMPILER_WORKS 8)
SET(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

set(CMAKE_VERBOSE_MAKEFILE ON)

SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F405RGTX_FLASH.ld)

#Uncomment for hardware floating point
SET(FPU_FLAGS "-mfloat-abi=hard -mfpu=fpv4-sp-d16")
add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING )
#add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING )
#Uncomment for software floating point
#SET(FPU_FLAGS "-mfloat-abi=soft")

SET(COMMON_FLAGS
    "-mcpu=cortex-m4 ${FPU_FLAGS} -mthumb -mthumb-interwork -ffunction-sections -fdata-sections \
    -g -fno-common -fmessage-length=0 -Wall -Werror ")

SET(COMMON_LDFLAGS "-Wl,--wrap,_malloc_r -Wl,--wrap,_realloc_r -Wl,--wrap,_free_r \
        -Wl,--wrap,gettimeofday -Wl,--wrap,settimeofday -Wl,--wrap,time -Wl,--wrap,main \
        -Wl,--wrap,exit，-Wl,--wrap,printf -Wl,--wrap,vprintf -Wl,--wrap,puts -Wl,--wrap,fprintf \
        -Wl,--wrap,vfprintf -Wl,--wrap,fputs -Wl,--wrap,putchar -Wl,--wrap,putc -Wl,--wrap,fputc \
        -Wl,--wrap,fflush")

SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99 ")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections,--print-memory-usage -T ${LINKER_SCRIPT}")

PROJECT(miniFoc_F4 C CXX ASM)
set(CMAKE_CXX_STANDARD 11 )

#add_definitions(-DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING -D__FPU_PRESENT=1)
add_definitions(-DDEBUG -DUSE_HAL_DRIVER -DSTM32F405xx)
add_definitions(-D__CONFIG_MALLOC_MODE=1 -D__CONFIG_MALLOC_TRACE -DHEAP_BACKTRACE)
#add_definitions(-D__CONFIG_MALLOC_MODE=1)

file(GLOB_RECURSE SOURCES
        "src/driver/*.*"
        "src/kernel/*.*"
        "src/libc/*.*"
        "src/fs/*.*"
        "src/sys/*.*"
#        "src/gui/*.*"
        "src/test/unity/*.*"
        "src/util/cjson/*.*"
        "src/log/*.*"
        "src/debug/*.*"
        "src/core/*.*"
        "src/console/*.*"
        "project/common/board/*.*"
        "project/common/board/miniFoc_v2/*.*"
        "project/common/cmd/*.*"
        "project/common/startup/*.*"
        "project/common/framework/*.*"
        "project/test/*.*"
        "project/apps/BldcController/*.*"
        "include/*.*"
        )

include_directories(
        include
        include/fs/FatFs/Target
        include/fs/FatFs/App
        include/drivers
        include/drivers/hal
        include/driver/chip/CMSIS
        include/driver/chip/CMSIS/Device/ST/STM32F4xx
        include/driver/chip/STM32_USB_Device_Library/Core
        include/driver/chip/STM32_USB_Device_Library/Class/CDC
        include/driver/chip/STM32_USB_Device_Library/Class/MSC
        include/driver/component/usb/USB_DEVICE/App
        include/driver/component/usb/USB_DEVICE/Target
        include/driver/chip/STM32F4xx_HAL_Driver
        include/driver/chip/STM32F4xx_HAL_Driver/Legacy
        include/kernel
        include/kernel/FreeRTOS/
        include/kernel/FreeRTOS/CMSIS_RTOS_V2
        include/kernel/FreeRTOS/portable/GCC/ARM_CM4F
        include/driver/component/flash
        include/driver/component/sfud
        project/
        project/common
        project/apps/modules
        src/gui/
        project/apps/BldcController/
        )

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_LDFLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
