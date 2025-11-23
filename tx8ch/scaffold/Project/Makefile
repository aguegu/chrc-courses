BIN     = ./bin
BIN_PREFIX = riscv-wch-elf

PORT = /dev/ttyUSB0

CFLAGS 	= -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g

DEFINES =

SCAFFOLD_ROOT = ..
SCAFFOLD_DIST = $(SCAFFOLD_ROOT)/dist

OUTPUT_DIR = ./dist

TARGET     = $(addprefix $(OUTPUT_DIR)/, $(notdir $(CURDIR)))

Project_ROOT    = .
Project_INC_DIR = $(Project_ROOT)/inc
Project_SRC_DIR = $(Project_ROOT)/src
Project_SOURCES = $(notdir $(wildcard $(Project_SRC_DIR)/*.c))
Project_OBJECTS = $(addprefix $(OUTPUT_DIR)/, $(Project_SOURCES:.c=.o))

MAIN_ROOT    = $(SCAFFOLD_ROOT)/Main
MAIN_INC_DIR = $(MAIN_ROOT)/inc
MAIN_SRC_DIR = $(MAIN_ROOT)/src
MAIN_SOURCES = $(notdir $(wildcard $(MAIN_SRC_DIR)/*.c))
MAIN_OBJECTS = $(addprefix $(SCAFFOLD_DIST)/, $(MAIN_SOURCES:.c=.o))

RVMSIS_ROOT			= $(SCAFFOLD_ROOT)/RVMSIS
RVMSIS_SOURCES 	= core_riscv.c
RVMSIS_OBJECTS  = $(addprefix $(SCAFFOLD_DIST)/, $(RVMSIS_SOURCES:.c=.o))

StdPeriphDriver_ROOT    = $(SCAFFOLD_ROOT)/StdPeriphDriver
StdPeriphDriver_INC_DIR = $(StdPeriphDriver_ROOT)/inc
StdPeriphDriver_SRC_DIR = $(StdPeriphDriver_ROOT)
StdPeriphDriver_SOURCES  = CH57x_sys.c CH57x_gpio.c CH57x_uart1.c CH57x_spi0.c CH57x_adc.c CH57x_clk.c
StdPeriphDriver_OBJECTS = $(addprefix $(SCAFFOLD_DIST)/, $(StdPeriphDriver_SOURCES:.c=.o))

Startup_ROOT		= $(SCAFFOLD_ROOT)/Startup
Startup_SOURCES = startup_CH573.S
Startup_OBJECTS = $(addprefix $(SCAFFOLD_DIST)/asm/, $(Startup_SOURCES:.S=.o))

BLELIB_ROOT = $(SCAFFOLD_ROOT)/BLE

HAL_ROOT	=	$(SCAFFOLD_ROOT)/HAL
HAL_INC_DIR	 	= $(HAL_ROOT)/inc
HAL_SOURCES 	= MCU.c RTC.c
HAL_OBJECTS 	= $(addprefix $(SCAFFOLD_DIST)/, $(HAL_SOURCES:.c=.o))

Common_ROOT = $(SCAFFOLD_ROOT)/libs
Common_INC_DIR = $(Common_ROOT)/inc
Common_SRC_DIR = $(Common_ROOT)/src
Common_SOURCES = gpio.c ringbuffer.c ssd1306.c uart1.c
Common_OBJECTS = $(addprefix $(SCAFFOLD_DIST)/, $(Common_SOURCES:.c=.o))

# LINK_FILE = $(SCAFFOLD_ROOT)/Ld/Link.ld
LINK_FILE = $(SCAFFOLD_ROOT)/Ld/Link.ld

INCLUDES = -I $(MAIN_INC_DIR) -I $(Project_INC_DIR) -I $(StdPeriphDriver_INC_DIR) -I $(RVMSIS_ROOT) -I $(Common_INC_DIR) -I $(BLELIB_ROOT) -I $(HAL_INC_DIR)

VPATH = $(MAIN_SRC_DIR):$(Project_SRC_DIR):$(StdPeriphDriver_SRC_DIR):$(Startup_ROOT):$(RVMSIS_ROOT):$(Common_SRC_DIR):$(HAL_ROOT)

LIBS = -lISP573 -lCH57xBLE

all: $(OUTPUT_DIR) $(TARGET).hex $(TARGET).siz $(TARGET).lst

version:
	$(BIN)/$(BIN_PREFIX)-gcc --version

$(OUTPUT_DIR):
	mkdir -p $(SCAFFOLD_DIST)/asm
	mkdir -p $(OUTPUT_DIR)

clean:
	rm -rf $(OUTPUT_DIR) $(SCAFFOLD_DIST)

$(SCAFFOLD_DIST)/asm/%.o: %.S
	$(BIN)/$(BIN_PREFIX)-gcc $(CFLAGS) -MMD -MP -x assembler-with-cpp -MF "$(@:%.o=%.d)" -MT $@ -c -o "$@" "$<"

$(SCAFFOLD_DIST)/%.o: %.c
	$(BIN)/$(BIN_PREFIX)-gcc $(CFLAGS) $(INCLUDES) $(DEFINES) -std=gnu99 -MMD -MP -MF "$(@:%.o=%.d)" -MT $@ -c -o "$@" "$<"

$(OUTPUT_DIR)/%.o: %.c
	$(BIN)/$(BIN_PREFIX)-gcc $(CFLAGS) $(INCLUDES) $(DEFINES) -std=gnu99 -MMD -MP -MF "$(@:%.o=%.d)" -MT $@ -c -o "$@" "$<"

$(TARGET).elf: $(MAIN_OBJECTS) $(Project_OBJECTS) $(StdPeriphDriver_OBJECTS) $(Startup_OBJECTS) $(RVMSIS_OBJECTS) $(Common_OBJECTS) $(HAL_OBJECTS)
	$(BIN)/$(BIN_PREFIX)-gcc $(CFLAGS) \
		-T "$(LINK_FILE)" -nostartfiles -Xlinker --gc-sections \
		-L "$(StdPeriphDriver_ROOT)" \
		-L "$(BLELIB_ROOT)" \
		-Xlinker --print-memory-usage -Wl,-Map,"$(@:%.elf=%.map)" --specs=nano.specs --specs=nosys.specs -o $@ $^ $(LIBS)

$(TARGET).hex: $(TARGET).elf
	$(BIN)/$(BIN_PREFIX)-objcopy -O ihex "$<" "$@"

$(TARGET).siz: $(TARGET).elf
	$(BIN)/$(BIN_PREFIX)-size --format=berkeley $^

$(TARGET).lst: $(TARGET).elf
	$(BIN)/$(BIN_PREFIX)-objdump --source --all-headers --demangle --line-numbers --wide $^ > $@

flash: $(TARGET).hex
	wchflash -p $(PORT) -f $<

.PHONY: version clean flash
