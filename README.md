Embedded development on CLion
=============================

I'm using an STM32 F7 Discovery (STM32F746G-DISCO), but I think this will help anyone doing cross platform compilation on CLion.

- Clion version: 2016.3 EAP (Win), 2016.2 (Linux)
- GNU ARM Toolchain: 5.4_2016q3
- OpenOCD: 0.9.0

**Note**: On windows, use MinGW.

Installation
------------

1. *[WIN]* Configure CLion to use MinGW.
	Cygwin can probably be used if you have a compiler compiled for it, but MinGW lets you use any compiler for normal Windows.
2. Download & Install your GCC compiler.
	- Ubuntu: 
		```
		sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded
		sudo apt-get update
		sudo apt-get install gcc-arm-none-eabi
		```
	- Arch linux:
		AUR package [gcc-arm-none-eabi-bin](https://aur.archlinux.org/packages/gcc-arm-none-eabi-bin/)
	- Windows (and other linux):
		[Launchpad](https://launchpad.net/gcc-arm-embedded)
3. Download & Install STM32CubeMX
	- [Website ST](http://www.st.com/en/development-tools/stm32cubemx.html)
	- Run it once and, via `Help -> Install New Libraries`, install the STM32F7 firmware package.
4. Download & Install OpenOCD
	- [Website OpenOCD](http://openocd.org/)
	- Make sure you have the configuration `board/stm32f3discovery.cfg`.
	  By default its at `<openocd install>/scripts/board/stm32f3discovery.cfg`
	- If you don't, download [this](https://github.com/openrisc/openOCD/tree/master/tcl) git repo and unpack `tcl` somewhere (`<openocd install>/tcl`).

Project Setup
-------------

1. Make new project in STM32CubeMX
	1. Select the right board. (Or if its not listed, select the raw MCU.)
	2. Leave the configuration default. (Or do pinout configuration, if raw MCU.)
	3. In the project settings, set `Toolchain/IDE` to `SW4STM32`.
		(You can change the stack size here too.)
	4. Save the project as a template, so you can easily make copies.
	5. Save project as, and set the name and project location.
    6. Finally, `Generate code`.
2. Import a new project 'from sources' in CLion
    1. Import the root project folder.
    2. Backup and replace `CMakeLists.txt` with [the attachement](#cmakelists) at the bottom.
    3. Create `toolchain.cmake` with [the attachement](#toolchain) at the bottom.
    4. Rename `STM32F746NGHx_FLASH.ld` (or equivalent) to `LinkerScript.ld`
    5. Mark as `Project Sources and Headers`:
        - `Drivers/CMSIS/Device/ST/STM32F7xx/Include`
        - `Drivers/CMSIS/Device/ST/STM32F7xx/Sources/Templates`
        - `Drivers/CMSIS/Include`
        - `Drivers/STM32F7xx_HAL_Driver/Inc`
        - `Drivers/STM32F7xx_HAL_Driver/Src`
        - `Inc`
        - `Src`
    6. Project settings:
        - Add the ARM toolchain bin folder to the `PATH` (environment variable, the box is hidden by default)
        - Add `-DCMAKE_TOOLCHAIN_FILE=toolchain.cmake` to the command (CMake option)
        - Set the generation path to `build`
    7. Rebuild the entire project:
        *You can ignore warnings about `CMAKE_FORCE_C_COMPILER` and `CMAKE_FORCE_CXX_COMPILER` being deprecated. The replacement for it doesn't seem to work in CLion.*
        - CMake Caches: `Tools -> CMake -> Reset Cache and Reload Project`
        - Build the actual code: `Run -> Build`
3. Fix imports
	For some reason, CLion won't recognise many of the imports and will turn the generated template red with errors.
	I fixed this by replacing the offending `#import main.h` statements with there full path versions `#import ../Inc/main.h`.
	You can see when CLion doesn't quite get it by looking for 'unused' imports.
	This is annoying and cumbersome, but it beats having to use Eclipse.
	

~Dries007 - nov 2016

Sourcess: 
- [Gold Coast Techspace: Getting to Blinky with the STM32 and Ubuntu Linux!](https://gctechspace.org/2014/09/getting-to-blinky-with-the-stm32-and-ubuntu-linux/)
- [Clion blog: CLion for embedded development](https://blog.jetbrains.com/clion/2016/06/clion-for-embedded-development/)

Attachements
------------

### Toolchain
```cmake
INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_CROSSCOMPILING "TRUE")

SET(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/LinkerScript.ld")

SET(CMAKE_C_COMPILER "arm-none-eabi-gcc")
SET(CMAKE_CXX_COMPILER "arm-none-eabi-g++")

CMAKE_FORCE_C_COMPILER("arm-none-eabi-gcc" GNU)
CMAKE_FORCE_CXX_COMPILER("arm-none-eabi-g++" GNU)
```

### CMakeLists

```cmake
cmake_minimum_required(VERSION 3.6)
# Project name
project(CLion_Embedded C ASM)
# Processor defenition **PROCESSOR / BOARD DEPENDANT**
add_definitions(-DSTM32F746xx)
add_definitions(-D__IO=volatile)

file(GLOB_RECURSE USER_SOURCES "Src/*.c")
file(GLOB_RECURSE HAL_SOURCES "Drivers/STM32F7xx_HAL_Driver/Src/*.c")

# Compiler options **PROCESSOR / BOARD DEPENDANT**
SET(COMMON_FLAGS "-mcpu=cortex-m7 -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv5-sp-d16 -ffunction-sections -fdata-sections -g -fno-common -fmessage-length=0 -Wall")

SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")

SET(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/LinkerScript.ld")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT}")

add_library(CMSIS
        Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c
        Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f746xx.s)

include_directories(Inc)
include_directories(Drivers/STM32F7xx_HAL_Driver/Inc)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32F7xx/Include)

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
```