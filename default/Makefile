###############################################################################
# Makefile for the project ece353_lab_2
###############################################################################

## General Flags
PROJECT = ece353_lab_2
MCU = atmega32
TARGET = ece353_lab_2.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99           -DF_CPU=4000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=ece353_lab_2.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Include Directories
INCLUDES = -I"D:\src\include" 

## Objects that must be built in order to link
OBJECTS = main.o gme_counter.o gme_digital_io.o gme_eeprom.o gme_error.o gme_midimsg.o gme_usart.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) ece353_lab_2.hex ece353_lab_2.eep ece353_lab_2.lss size

## Compile
main.o: ../main.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

gme_counter.o: ../gme_counter.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

gme_digital_io.o: ../gme_digital_io.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

gme_eeprom.o: ../gme_eeprom.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

gme_error.o: ../gme_error.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

gme_midimsg.o: ../gme_midimsg.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

gme_usart.o: ../gme_usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) ece353_lab_2.elf dep/* ece353_lab_2.hex ece353_lab_2.eep ece353_lab_2.lss ece353_lab_2.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

