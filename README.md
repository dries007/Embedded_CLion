Embedded development on CLion
=============================

I'm using an STM32 F7 Discovery (STM32F746G-DISCO), but I think this will help anyone doing cross platform compilation on CLion.

- Clion version: 2016.3 (Linux & Windows)
- GNU ARM Toolchain: 5.4_2016q3 (Windows) 6.2.0-1 (Arch Linux)
- OpenOCD: 0.9.0

**Note**: On windows, use MinGW.

~Dries007 - nov 2016

Sources: 
- [Gold Coast Techspace: Getting to Blinky with the STM32 and Ubuntu Linux!](https://gctechspace.org/2014/09/getting-to-blinky-with-the-stm32-and-ubuntu-linux/)
- [Clion blog: CLion for embedded development](https://blog.jetbrains.com/clion/2016/06/clion-for-embedded-development/)

Installation
------------

1. *[WIN]* Configure CLion to use MinGW.
	Cygwin can probably be used if you have a compiler compiled for it, but MinGW lets you use any compiler for normal Windows.
2. Download & Install your GCC compiler.
	- Ubuntu (untested):
        Add the right repo and install gcc-arm-none-eabi
	    `sudo add-apt-repository ppa:terry.guo/gcc-arm-embedded; sudo apt-get update; sudo apt-get install gcc-arm-none-eabi` 
	- Arch linux packages:
	    1. Pacman: `arm-none-eabi-newlib arm-none-eabi-gcc arm-none-eabi-gdb`
	    2. Optional Pacman: `openocd stlink`
	    3. Optional AUR: `stm32cubemx stm32flash`
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
    2. Backup and replace `CMakeLists.txt` with [CMakeLists.txt](CMakeLists.txt) from git.
    3. Create `toolchain.cmake` with [toolchain.cmake](toolchain.cmake) from git.
    4. Rename `STM32F746NGHx_FLASH.ld` (or equivalent) to `LinkerScript.ld`
    5. Mark as `Project Sources and Headers` (or don't yet, see step 3):
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
3. Fix imports.
    - For some reason, CLion won't recognise many of the imports and will turn the generated template red with errors.
    - The fix: Flatten the file structure to just 1 'lib/' folder with sources and headers:
    - **Move all files in include with a file manager, not CLion.**
4. Move the pre-generated init code out of the `main` src-header pair.
    - If you want cleaner code, move it to `lib/main_gen.c|h`.
    - Don't forget to export the functions and call them in the same order in your new `main.c`
5. Disable (strict) code inspection on the generated source.
    - Make a new scope called "Lib" via `File -> Settings -> Appearance & Behaviour -> Scopes` and include `lib/` recursively.
    - Now, via `Editor -> Inspections` in the settings window, click on `C/C++`.
    - Via the dropdown "In All Scopes", select "Lib". The box should change to "Severity by scope"
    - Set the level of "Lib" to "Weak Warning"
    - You can do the same to other inspections like Spelling (Just uncheck the box to completely disable it in that scope)
    - Run a code inspection (`Code -> Inspect Code...`). It will be a lot less crazy than without the scope limitation. (This also makes commit time inspections usefull agian.)
    - You can also make a scope just for your code. It can come in handy in many situation, including code inspection.


todo: Write debugger part
