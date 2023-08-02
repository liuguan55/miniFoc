#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.7)

include(${CMAKE_SOURCE_DIR}/build/CMakeLists.gcc.txt)

file(GLOB_RECURSE SOURCES )
file(GLOB_RECURSE HEADERS )
include(${CMAKE_SOURCE_DIR}/build/CMakeLists.app.txt)
include(${CMAKE_SOURCE_DIR}/build/CMakeLists.platform.txt)
include(${CMAKE_SOURCE_DIR}/build/CMakeLists.common.txt)

include_directories(
        ${HEADERS}
        include
        include/driver
        include/driver/hal
        include/driver/chip/CMSIS
        project/
        project/common/
        project/modules
)

file(GLOB_RECURSE BASE_SOURCES
        "src/sys/*.* "
        "src/core/*.*"
        "src/driver/hal/*.*"
        "project/common/board/Board.c"
        "project/common/board/BoardCommon.c"
        "project/common/startup/common/retarget.c"
        "project/common/startup/common/retarget_main.c"
        "project/common/startup/common/retarget_stdout.c"
        "project/common/startup/common/syscalls.c"
        "project/common/startup/common/sysmem.c"
        "project/common/framework/multiButton/*.*"
        "project/common/framework/multiTimer/*.*"
        "project/common/framework/platform/*.*"
        "project/common/framework/util/*.*"
        "include/*.*"
        )

list(APPEND SOURCES ${BASE_SOURCES})

message(STATUS "HEADERS: ${HEADERS}")
message(STATUS "SOURCES: ${SOURCES}")

PROJECT(${APP_NAME} C CXX ASM)
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_LDFLAGS} -Wl,--wrap,main -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
