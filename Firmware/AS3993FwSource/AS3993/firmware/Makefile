# *****************************************************************************
# * Copyright @ by ams AG                                                     *
# * All rights are reserved.                                                  *
# *                                                                           *
# * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
# * THE SOFTWARE.                                                             *
# *                                                                           *
# * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       * 
# * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
# * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
# * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
# * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
# * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
# * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
# * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
# * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
# * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
# * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
# *****************************************************************************

###############################################################################
# Define here the target for that you want to build 
# either uncomment one target here or call: make TARGET=as3933 all
###############################################################################
TARGET=as3993
# to enable debugging define the symbol DEBUG 
#DEBUG=1
# to enable logging on UART tx define USE_LOGGER
#USE_LOGGER=1
# to enable generic UART rx capabilities define USE_UART_RECEIVE
USE_UART_RECEIVE=1
# to enable host communication over UART (instead of USB) define USE_UART_STREAM_DRIVER
USE_UART_STREAM_DRIVER=1

###############################################################################
# Define some variables later used by the makefile
###############################################################################
#CPU can also be set when calling make (eg: in createDelivery.sh)
ifndef CPU
CPU=24FJ64GB004
endif
#bootloader can be disabled when calling make (eg: for AS3994)
ifndef DISABLE_BOOTLOADER
ENABLE_BOOTLOADER=1
endif


C30DIR = "C:\Program Files (x86)\Microchip\MPLAB C30"

C30_RESOURE_FILE = $(C30DIR)"/bin/c30_device.info"

ROOT_DIR=../..
MCU_DIR=$(ROOT_DIR)/common/firmware/microchip
COMMON_DIR=$(ROOT_DIR)/common
COMMON_SOURCE_DIR=$(MCU_DIR)/src

SOURCE_DIR=src
BUILD_DIR=build
DOC_DIR=doc
DOC_OUTPUT_DIR=$(DOC_DIR)/generated
GUI_DIR=../GUI/Reader_Tool

# additional libraries to be linked.
# e.g. the peripheral library for the pic24f: pPIC24Fxxx
LIBS=pPIC24Fxxx

###############################################################################
# Try to find the microchip toolchain - if the variable c30dir is not set as
# e.g. in Windows as a system variable.
###############################################################################
#ifndef c30dir
#c30dir= $(wildcard /cygdrive/c/c30 C:\MPLABC30)
#endif
#C30DIR="$(c30dir)"

###############################################################################
# Set tool chain
###############################################################################
CC = $(C30DIR)/bin/pic30-gcc
AS = $(C30DIR)/bin/pic30-as
LD = $(C30DIR)/bin/pic30-ld
AR = $(C30DIR)/bin/pic30-ar
HEXER = $(C30DIR)/bin/pic30-bin2hex
HEX2BIN = $(ROOT_DIR)/common/Tools/objcopy -I ihex -O binary
SIZE_CONV= perl ../../common/Tools/scripts/convert_c30_object_sizes.pl
OBJDUMP = $(C30DIR)/bin/pic30-objdump
DOXYGEN = doxygen
PICPROGRAMMER = "/cygdrive/c/Program Files (x86)/Microchip/MPLAB IDE/Programmer Utilities/ICD3"/ICD3CMD.exe
AMSUPDATER = $(ROOT_DIR)/common/Tools/AMSFirmwareUpdater/App/ReleaseApp/AMSFirmwareUpdater.exe
###############################################################################
# Set flags
###############################################################################
LIBRARIES=$(addprefix -l,$(LIBS))
INC_DIRS=$(addprefix -I,$(INCLUDE_DIRS))
CFLAGS=\
	-D__PIC$(CPU)__=1 \
	$(INC_DIRS)  \
        -omf=elf \
	-Wall -O1 -fno-omit-frame-pointer -mcpu=$(CPU) -omf=elf\
	-mresource=$(C30_RESOURE_FILE) 

ASFLAGS=-p$(CPU)
LDFLAGS=-mcpu=$(CPU) -omf=elf \
        -omf=elf \
	-Wl,--heap=0,--script="$(LINKER_SCRIPT)",--report-mem,-Map,$(BUILD_DIR)/$(TARGET).map

ifdef USE_LOGGER
CFLAGS+=-DUSE_LOGGER
endif

ifdef USE_UART_RECEIVE
CFLAGS+=-DUSE_UART_RECEIVE
endif

CFLAGS+=-DPOSC_NOMINAL=12000000ULL

ifdef ENABLE_BOOTLOADER
C_FILES+=\
    bootloadable.c
endif

ifdef USE_UART_STREAM_DRIVER
CFLAGS+=-DUSE_UART_STREAM_DRIVER
C_FILES+=\
    uart_stream_driver.c
else
C_FILES+=\
    usb_hid_stream_driver.c \
    usb_descriptors.c \
    usb_device.c \
    usb_hal_pic24.c \
    usb_function_hid.c
endif

ifdef DEBUG
CFLAGS+=-DDEBUG_ON -DDEBUG -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -g 
LDFLAGS+=-Wl,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1,--defsym=__ICD2RAM=1
endif

LINKER_SCRIPT = $(SOURCE_DIR)/p$(CPU).gld

###############################################################################
# Add all include directories
###############################################################################
INCLUDE_DIRS+=\
    $(COMMON_DIR)/include \
    $(MCU_DIR)/include \
    $(C30DIR)/support/PIC24F/h \
    $(C30DIR)/support/peripheral_24F \
    $(C30DIR)/support/generic/h \
    $(C30DIR)/include \
    $(SOURCE_DIR) 


vpath %.c $(SOURCE_DIR)
vpath %.s $(SOURCE_DIR)
vpath %.c $(COMMON_SOURCE_DIR)
vpath %.h $(MCU_DIR)/include
vpath %.h $(SOURCE_DIR)
vpath %.h $(COMMON_DIR)/include

C_FILES+=\
    main.c	\
    stream_dispatcher.c \
    weak_stream_functions.c \
    timer.c \
    uart_driver.c \
    global.c \
    appl_commands.c \
    appl_commands_table.c \
    gen2.c \
	iso6b.c \
	bitbang.c \
	crc16.c \
    as3993.c \
    logger.c \
    spi_driver.c \
    platform.c \
    system_clock.c \
    tuner.c 

ASM_FILES=\

H_FILES= $(patsubst %.c,%.h,$(C_FILES))
H_FILES:= $(filter-out appl_commands_table.h main.h usb_descriptors.h weak_stream_functions.h, $(H_FILES))
H_FILES+= platform.h as3993_config.h as3993_public.h errno_as3993.h ams_stream.h usb_config.h stream_dispatcher.h errno.h


###############################################################################
# Object file *.o are all c-source and asm-source files. First we remove
# everything but the name of the file, than we substitute the suffix to *.o
# and finally we add the build directory as a prefix to *.o.
###############################################################################
OBJECTS:=\
	$(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.o,$(C_FILES)))   \
	$(addprefix $(BUILD_DIR)/,$(patsubst %.s,%.o,$(ASM_FILES))) \

###############################################################################
# Rules
###############################################################################

.PHONY: all
all: $(BUILD_DIR)/$(TARGET).bin $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).dis


# build the executable by linking all o-files
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $(OBJECTS) $(LIBRARIES) | tee $(BUILD_DIR)/linker_stdout.txt 
	-size $(BUILD_DIR)/*.o | $(SIZE_CONV) > $(BUILD_DIR)/$(TARGET)_sizes.txt

# build the hex file
%.hex: %.elf
	$(HEXER) $< -omf=elf

# build the binary file from the hex file
%.bin: %.hex
	$(HEX2BIN) $< $@
ifneq ($(wildcard $(GUI_DIR)/firmware/),)
	cp $@ $(GUI_DIR)/firmware/as3993_head.bin
endif

# build the binary file from the hex file
%.dis: %.elf
	$(OBJDUMP) -omf=elf -D -S $< > $@

.PHONY: doc
doc: all doc_files

.PHONY: doc_files
doc_files: $(C_FILES) $(H_FILES) HowtoBuild.dox
	echo "Creating doxygen configuration file"; \
	echo "@INCLUDE               = \"$(DOC_DIR)/Doxyfile\"" > $(DOC_DIR)/Doxyfile.make ; \
	echo "PROJECT_NAME           = \"$(TARGET) Firmware\"" >> $(DOC_DIR)/Doxyfile.make ; \
	echo "INPUT                  = $^" >> $(DOC_DIR)/Doxyfile.make ; \
	echo "OUTPUT_DIRECTORY       = \"$(DOC_OUTPUT_DIR)\"" >> $(DOC_DIR)/Doxyfile.make ; \
	echo "Generate doxygen documentation"; \
	$(DOXYGEN) $(DOC_DIR)/Doxyfile.make

.PHONY: modules
modules: $(OBJECTS)

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)/.dir
	$(CC) -c $< $(CFLAGS) -o $@
	$(CC) -E $< $(CFLAGS) -o $@.ppd

$(BUILD_DIR)/%.o: %.s $(BUILD_DIR)/.dir
	$(AS) $< $(ASFLAGS) -o $@

clean:
	rm -rf $(BUILD_DIR) $(DOC_OUTPUT_DIR) $(DOC_DIR)/Doxyfile.make

# Mark .dir files as not being intermediate, otherwise it will be deleted and 
# subsequently recreated forcing rebuild of all objects
.PRECIOUS: %/.dir
%/.dir: 
	mkdir -p $*
	touch $@

.PHONY: program
program:
	$(AMSUPDATER) $(BUILD_DIR)/$(TARGET).bin picusb --streampid=c02e

.PHONY: program_raw
program_raw:
	$(PICPROGRAMMER) -P$(CPU) -E -M -F$(BUILD_DIR)/$(TARGET).hex -L

.PHONY: bootprogram
bootprogram:
	$(PICPROGRAMMER) -P$(CPU) -E -M -F../../common/firmware/microchip/bootloader/usb/PIC24F_USB_HID_Bootloader_v021601_nolog.hex -L

.PHONY: tags
tags: 
	ctags -R src \
                 ../../common/firmware/microchip/src \
                 ../../common/firmware/microchip/include \
                 $(C30DIR)/support/peripheral_24F/ \
                 $(C30DIR)/support/generic/h \
                 $(C30DIR)/include \
                 $(C30DIR)/support/PIC24F/h/p$(CPU).h


.PHONY: help
help:
	@echo "make clean"
	@echo "make all"
	@echo "make program"
	@echo "make doc"
	@echo "make tags"


