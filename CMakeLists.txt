cmake_minimum_required(VERSION 3.6)
# Project name
project(CLion_Embedded C ASM)
# Processor defenition **PROCESSOR / BOARD DEPENDANT**
add_definitions(-DSTM32F746xx)
add_definitions(-DARM_MATH_CM7)

file(GLOB_RECURSE USER_SOURCES "src/*.c")
file(GLOB_RECURSE HAL_SOURCES "lib/*.c")

# Compiler options **PROCESSOR / BOARD DEPENDANT**
SET(COMMON_FLAGS "-mcpu=cortex-m7 -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv5-sp-d16 -ffunction-sections -fdata-sections -g -fno-common -fmessage-length=0 -Wall")

SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")

SET(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/LinkerScript.ld")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT}")

add_library(CMSIS
        lib/system_stm32f7xx.c
        lib/gcc/startup_stm32f746xx.s)

include_directories(lib)
include_directories(src)

add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${HAL_SOURCES} ${LINKER_SCRIPT})

target_link_libraries(${PROJECT_NAME}.elf CMSIS)

# Make map, hex and bin
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map")
set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
