# ---------------------------------------------------------------------------
# Copyright (c) 2011 Matthias Meier 31.1.2017
# ---------------------------------------------------------------------------

# define where to create the elf and map output files
TARGET = build/main

# ARM Toolchain -  if toolchain is not in system search path, prefix the path to the toolchain binaries!
TOOLCHAIN=arm-none-eabi-
CC=$(TOOLCHAIN)gcc
OBJCOPY=$(TOOLCHAIN)objcopy
SIZE=$(TOOLCHAIN)size

# ---------------------------------------------------------------------------



SYSTEM_LIB_BASEDIR = system-libs
MAIN_DIR = .
FREERTOS_DIR             = $(SYSTEM_LIB_BASEDIR)/freertos
CMSIS_DIR                = $(SYSTEM_LIB_BASEDIR)/cmsis
DIAG_DIR                 = $(SYSTEM_LIB_BASEDIR)/diag
STM32F4_HAL_DIR          = $(SYSTEM_LIB_BASEDIR)/stm32f4-hal
STM32F429I_DISCOVERY_DIR = $(SYSTEM_LIB_BASEDIR)/stm32f429i-discovery
CORTEXM_DIR              = $(SYSTEM_LIB_BASEDIR)/cortexm
NEWLIB_DIR               = $(SYSTEM_LIB_BASEDIR)/newlib

# build gcc options for include search paths (without CORTEX_DIR and NEWLIB_DIR)
INC_DIRS = $(MAIN_DIR) $(FREERTOS_DIR) $(CMSIS_DIR) $(DIAG_DIR) $(STM32F4_HAL_DIR) $(STM32F429I_DISCOVERY_DIR) 
#INC_DIRS += CORTEXM_DIR) $(NEWLIB_DIR)
INCLUDES=$(INC_DIRS:%=-I %/inc)
# which is same as: INCLUDES = $(foreach d, $(INC_DIRS), -I $d/inc)


# build SRC list for each system lib subdir

MAIN_SRCS = \
main.c \
_initialize_hardware.c \
BlinkLed.c \
Lcd.c \
stm32f4xx_hal_msp.c


STM32F429I_DISCOVERY_SRCS = \
ili9341.c \
l3gd20.c \
stm32f429i_discovery.c \
stm32f429i_discovery_eeprom.c \
stm32f429i_discovery_gyroscope.c \
stm32f429i_discovery_io.c \
stm32f429i_discovery_lcd.c \
stm32f429i_discovery_sdram.c \
stm32f429i_discovery_ts.c \
stm32f4xx_hal_timebase_tim.c \
stm32f4xx_it.c \
stmpe811.c \
system_stm32f4xx.c \
ts_calibration.c 

CORTEXM_SRCS = \
_initialize_hardware.c \
_reset_hardware.c \
exception_handlers.c \
vectors_stm32f429xx.c 

DIAG_SRCS = \
Trace.c \
trace_impl.c 

NEWLIB_SRCS = \
_exit.c \
_sbrk.c \
_startup.c \
_syscalls.c \
assert.c 

NEWLIB_CPP_SRCS = \
_cxx.cpp 

STM32F4_HAL_SRCS = \
stm32f4xx_hal.c \
stm32f4xx_hal_cortex.c \
stm32f4xx_hal_dma.c \
stm32f4xx_hal_dma2d.c \
stm32f4xx_hal_flash.c \
stm32f4xx_hal_gpio.c \
stm32f4xx_hal_i2c.c \
stm32f4xx_hal_iwdg.c \
stm32f4xx_hal_ltdc.c \
stm32f4xx_hal_pwr.c \
stm32f4xx_hal_rcc.c \
stm32f4xx_hal_rcc_ex.c \
stm32f4xx_hal_rtc_ex.c \
stm32f4xx_hal_sdram.c \
stm32f4xx_hal_spi.c \
stm32f4xx_hal_tim.c \
stm32f4xx_hal_tim_ex.c \
stm32f4xx_ll_fmc.c 

FREERTOS_SRCS = \
port.c \
croutine.c \
event_groups.c \
list.c \
queue.c \
tasks.c \
timers.c \
MemMang/heap_2.c



SOURCES = \
$(MAIN_SRCS:%=$(MAIN_DIR)/src/%) \
$(FREERTOS_SRCS:%=$(FREERTOS_DIR)/src/%) \
$(CMSIS_SRCS:%=$(CMSIS_DIR)/src/%) \
$(DIAG_SRCS:%=$(DIAG_DIR)/src/%) \
$(STM32F4_HAL_SRCS:%=$(STM32F4_HAL_DIR)/src/%) \
$(STM32F429I_DISCOVERY_SRCS:%=$(STM32F429I_DISCOVERY_DIR)/src/%) \
$(CORTEXM_SRCS:%=$(CORTEXM_DIR)/src/%) \
$(NEWLIB_SRCS:%=$(NEWLIB_DIR)/src/%) 


#show:
#	@echo 
#	@echo Inc dirs: $(INC_DIRS)	
#	@echo 
#	@echo Includes: $(INCLUDES)	
#	@echo 
#	@echo Sources: $(SOURCES)	
 

# All Stellaris peripherie drivers which are referenced in project but not 
# listed in SOURCE variable above are taken out of following StellarisWare binary driver library: 

OBJS = $(SOURCES:.c=.o)

LDSCRIPT = -L ldscripts -T mem.ld -T libs.ld -T sections.ld

BUILD_DIR = build

# ---------------------------------------------------------------------------

# for debugging switch compiler optimisation off by selecting -O0 , or otherwise use -O2 for normal speed optimisation
CFLAGS += -O0 -g3 -DDEBUG
#CFLAGS += -O2 -g3 -DDEBUG

CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 
CFLAGS += -DSTM32F429xx -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 
CFLAGS += -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG 
CFLAGS += -std=gnu11 
CFLAGS += -Wall -Wextra -fmessage-length=0 
CFLAGS += -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants 
# CFLAGS += -Wno-attributes -Wno-unused-function
# ev. for src/stm32f4xx_hal_msp.c added:  -Wno-missing-prototypes -Wno-missing-declarations
CFLAGS += $(INCLUDES) 
CFLAGS += -MD -MP -MF dep/$(@F).d -MT $(*F).o 
# CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)"

CFLAGS += -u _printf_float 
# following was additionally set on stellaris configuration of FreeRTOS:
#CFLAGS += -D PACK_STRUCT_END=__attribute\(\(packed\)\) -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\)
#CFLAGS += -D inline=  -D sprintf=usprintf -D snprintf=usnprintf  # sprintf referenced in task.c but replaced here to use usprintf of Luminary Lib
#CFLAGS += -D printf=uipprintf -I $(UIP_COMMON_DIR) -I ./webserver 
#CFLAGS += -ffunction-sections -fdata-sections
#CFLAGS += -std=gnu99

# should use --gc-sections but the debugger does not seem to be able to cope with the option (mm: tested: --gc-sections is ok on openocd)

LDFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 
LDFLAGS += -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants 
LDFLAGS += -Wall -Wextra  -g3 $(LDSCRIPT)
LDFLAGS += -Wl,-Map,$(TARGET).map -o$(TARGET).elf
LDFLAGS += -nostartfiles -Xlinker --gc-sections --specs=nano.specs 
LDFLAGS += -Wl,--undefined=uxTopUsedPriority
#LDFLAGS += -nostdlib # you may use this if nothing at all of newlib is used
#LDFLAGS = --no-gc-sections # try this if debugger crashes   


# ---------------------------------------------------------------------------
# rules for flash programming and gdbserver by OpenOCD JTAG Debug Server...


# Remark: on Windows Win-USB driver needs to be installed eg. by 'zadig' 

OPENOCD = /Applications/GNU\ ARM\ Eclipse/OpenOCD/0.10.0-201701241841/bin/openocd

#OOCD_INIT  += -f board/stm32f429discovery.cfg
OOCD_INIT  += -f board/stm32f429disc1.cfg
OOCD_INIT  += -c "adapter_khz 1800"
OOCD_INIT  += -c init
OOCD_INIT  += -c "reset init"
#OOCD_INIT += -c "targets"

#OOCD_FLASH = -c "reset halt"
OOCD_FLASH += -c "flash write_image erase $(TARGET).elf"
OOCD_FLASH += -c "verify_image $(TARGET).elf"
OOCD_FLASH += -c "reset run"
OOCD_FLASH += -c shutdown

	
flash: $(TARGET).elf
	$(OPENOCD)  $(OOCD_INIT) $(OOCD_FLASH)


gdbserver:
#	$(OPENOCD)  $(OOCD_INIT)  -c "stm32f4x.cpu configure -rtos auto;"
	$(OPENOCD)  $(OOCD_INIT)


killgdbserver:
	# @echo "shutdown" | telnet localhost 4444
ifdef windir
	taskkill /IM $(OPENOCD).exe /F
else
	killall openocd
endif

# ---------------------------------------------------------------------------

all: $(TARGET).elf size 
	 
$(OBJS) : %.o : %.c
	@echo Compiling $< ...
	$(CC) -c $(CFLAGS) $< -o $@ 2>&1


$(TARGET).elf : $(OBJS)
	@echo  Linking $(PWD)/$@ ...
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LDFLAGS)  2>&1


#$(TARGET).bin : $(TARGET).elf
#	@echo Extracting $< ...
#	$(OBJCOPY) $(TARGET).elf -O binary $(TARGET).bin


size: $(TARGET).elf
	$(SIZE) -B $(TARGET).elf


clean:
	rm $(OBJS)  dep/*  $(TARGET).elf  $(TARGET).map


ifdef windir
-include  $(shell mkdir dep 2> nul)  $(wildcard dep/*)
else
-include  $(shell mkdir -p dep)  $(wildcard dep/*)
endif

.PHONY : all clean size flash gdbserver killgdbserver
# ---------------------------------------------------------------------------

