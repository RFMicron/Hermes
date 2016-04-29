#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-debug.mk)" "nbproject/Makefile-local-debug.mk"
include nbproject/Makefile-local-debug.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=debug
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/AS3993_uart.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/AS3993_uart.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/as3993.c ../src/gen2.c ../src/global.c ../../../common/firmware/microchip/src/logger.c ../src/main.c ../src/timer.c ../src/tuner.c ../../../common/firmware/microchip/src/uart_driver.c ../../../common/firmware/microchip/src/bootloadable.c ../../../common/firmware/microchip/src/spi_driver.c ../src/platform.c ../src/appl_commands.c ../src/appl_commands_table.c ../../../common/firmware/microchip/src/stream_dispatcher.c ../../../common/firmware/microchip/src/uart_stream_driver.c ../../../common/firmware/microchip/src/weak_stream_functions.c ../../../common/firmware/microchip/src/system_clock.c ../src/bitbang.c ../src/crc16.c ../src/iso6b.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1360937237/as3993.o ${OBJECTDIR}/_ext/1360937237/gen2.o ${OBJECTDIR}/_ext/1360937237/global.o ${OBJECTDIR}/_ext/1611813562/logger.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1360937237/timer.o ${OBJECTDIR}/_ext/1360937237/tuner.o ${OBJECTDIR}/_ext/1611813562/uart_driver.o ${OBJECTDIR}/_ext/1611813562/bootloadable.o ${OBJECTDIR}/_ext/1611813562/spi_driver.o ${OBJECTDIR}/_ext/1360937237/platform.o ${OBJECTDIR}/_ext/1360937237/appl_commands.o ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o ${OBJECTDIR}/_ext/1611813562/system_clock.o ${OBJECTDIR}/_ext/1360937237/bitbang.o ${OBJECTDIR}/_ext/1360937237/crc16.o ${OBJECTDIR}/_ext/1360937237/iso6b.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1360937237/as3993.o.d ${OBJECTDIR}/_ext/1360937237/gen2.o.d ${OBJECTDIR}/_ext/1360937237/global.o.d ${OBJECTDIR}/_ext/1611813562/logger.o.d ${OBJECTDIR}/_ext/1360937237/main.o.d ${OBJECTDIR}/_ext/1360937237/timer.o.d ${OBJECTDIR}/_ext/1360937237/tuner.o.d ${OBJECTDIR}/_ext/1611813562/uart_driver.o.d ${OBJECTDIR}/_ext/1611813562/bootloadable.o.d ${OBJECTDIR}/_ext/1611813562/spi_driver.o.d ${OBJECTDIR}/_ext/1360937237/platform.o.d ${OBJECTDIR}/_ext/1360937237/appl_commands.o.d ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.d ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.d ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.d ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.d ${OBJECTDIR}/_ext/1611813562/system_clock.o.d ${OBJECTDIR}/_ext/1360937237/bitbang.o.d ${OBJECTDIR}/_ext/1360937237/crc16.o.d ${OBJECTDIR}/_ext/1360937237/iso6b.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1360937237/as3993.o ${OBJECTDIR}/_ext/1360937237/gen2.o ${OBJECTDIR}/_ext/1360937237/global.o ${OBJECTDIR}/_ext/1611813562/logger.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1360937237/timer.o ${OBJECTDIR}/_ext/1360937237/tuner.o ${OBJECTDIR}/_ext/1611813562/uart_driver.o ${OBJECTDIR}/_ext/1611813562/bootloadable.o ${OBJECTDIR}/_ext/1611813562/spi_driver.o ${OBJECTDIR}/_ext/1360937237/platform.o ${OBJECTDIR}/_ext/1360937237/appl_commands.o ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o ${OBJECTDIR}/_ext/1611813562/system_clock.o ${OBJECTDIR}/_ext/1360937237/bitbang.o ${OBJECTDIR}/_ext/1360937237/crc16.o ${OBJECTDIR}/_ext/1360937237/iso6b.o

# Source Files
SOURCEFILES=../src/as3993.c ../src/gen2.c ../src/global.c ../../../common/firmware/microchip/src/logger.c ../src/main.c ../src/timer.c ../src/tuner.c ../../../common/firmware/microchip/src/uart_driver.c ../../../common/firmware/microchip/src/bootloadable.c ../../../common/firmware/microchip/src/spi_driver.c ../src/platform.c ../src/appl_commands.c ../src/appl_commands_table.c ../../../common/firmware/microchip/src/stream_dispatcher.c ../../../common/firmware/microchip/src/uart_stream_driver.c ../../../common/firmware/microchip/src/weak_stream_functions.c ../../../common/firmware/microchip/src/system_clock.c ../src/bitbang.c ../src/crc16.c ../src/iso6b.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-debug.mk dist/${CND_CONF}/${IMAGE_TYPE}/AS3993_uart.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ64GB002
MP_LINKER_FILE_OPTION=,--script="..\src\p24FJ64GB002.gld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1360937237/as3993.o: ../src/as3993.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/as3993.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/as3993.o.ok ${OBJECTDIR}/_ext/1360937237/as3993.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/as3993.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/as3993.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/as3993.o.d" -o ${OBJECTDIR}/_ext/1360937237/as3993.o ../src/as3993.c    
	
${OBJECTDIR}/_ext/1360937237/gen2.o: ../src/gen2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/gen2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/gen2.o.ok ${OBJECTDIR}/_ext/1360937237/gen2.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/gen2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/gen2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/gen2.o.d" -o ${OBJECTDIR}/_ext/1360937237/gen2.o ../src/gen2.c    
	
${OBJECTDIR}/_ext/1360937237/global.o: ../src/global.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/global.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/global.o.ok ${OBJECTDIR}/_ext/1360937237/global.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/global.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/global.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/global.o.d" -o ${OBJECTDIR}/_ext/1360937237/global.o ../src/global.c    
	
${OBJECTDIR}/_ext/1611813562/logger.o: ../../../common/firmware/microchip/src/logger.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/logger.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/logger.o.ok ${OBJECTDIR}/_ext/1611813562/logger.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/logger.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/logger.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/logger.o.d" -o ${OBJECTDIR}/_ext/1611813562/logger.o ../../../common/firmware/microchip/src/logger.c    
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.ok ${OBJECTDIR}/_ext/1360937237/main.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    
	
${OBJECTDIR}/_ext/1360937237/timer.o: ../src/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer.o.ok ${OBJECTDIR}/_ext/1360937237/timer.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/timer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/timer.o.d" -o ${OBJECTDIR}/_ext/1360937237/timer.o ../src/timer.c    
	
${OBJECTDIR}/_ext/1360937237/tuner.o: ../src/tuner.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/tuner.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/tuner.o.ok ${OBJECTDIR}/_ext/1360937237/tuner.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/tuner.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/tuner.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/tuner.o.d" -o ${OBJECTDIR}/_ext/1360937237/tuner.o ../src/tuner.c    
	
${OBJECTDIR}/_ext/1611813562/uart_driver.o: ../../../common/firmware/microchip/src/uart_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_driver.o.ok ${OBJECTDIR}/_ext/1611813562/uart_driver.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/uart_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/uart_driver.o.d" -o ${OBJECTDIR}/_ext/1611813562/uart_driver.o ../../../common/firmware/microchip/src/uart_driver.c    
	
${OBJECTDIR}/_ext/1611813562/bootloadable.o: ../../../common/firmware/microchip/src/bootloadable.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/bootloadable.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/bootloadable.o.ok ${OBJECTDIR}/_ext/1611813562/bootloadable.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/bootloadable.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/bootloadable.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/bootloadable.o.d" -o ${OBJECTDIR}/_ext/1611813562/bootloadable.o ../../../common/firmware/microchip/src/bootloadable.c    
	
${OBJECTDIR}/_ext/1611813562/spi_driver.o: ../../../common/firmware/microchip/src/spi_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/spi_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/spi_driver.o.ok ${OBJECTDIR}/_ext/1611813562/spi_driver.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/spi_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/spi_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/spi_driver.o.d" -o ${OBJECTDIR}/_ext/1611813562/spi_driver.o ../../../common/firmware/microchip/src/spi_driver.c    
	
${OBJECTDIR}/_ext/1360937237/platform.o: ../src/platform.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/platform.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/platform.o.ok ${OBJECTDIR}/_ext/1360937237/platform.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/platform.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/platform.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/platform.o.d" -o ${OBJECTDIR}/_ext/1360937237/platform.o ../src/platform.c    
	
${OBJECTDIR}/_ext/1360937237/appl_commands.o: ../src/appl_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands.o.ok ${OBJECTDIR}/_ext/1360937237/appl_commands.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/appl_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/appl_commands.o.d" -o ${OBJECTDIR}/_ext/1360937237/appl_commands.o ../src/appl_commands.c    
	
${OBJECTDIR}/_ext/1360937237/appl_commands_table.o: ../src/appl_commands_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.ok ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.d" -o ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o ../src/appl_commands_table.c    
	
${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o: ../../../common/firmware/microchip/src/stream_dispatcher.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.ok ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.d" -o ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o ../../../common/firmware/microchip/src/stream_dispatcher.c    
	
${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o: ../../../common/firmware/microchip/src/uart_stream_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.ok ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.d" -o ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o ../../../common/firmware/microchip/src/uart_stream_driver.c    
	
${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o: ../../../common/firmware/microchip/src/weak_stream_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.ok ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.d" -o ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o ../../../common/firmware/microchip/src/weak_stream_functions.c    
	
${OBJECTDIR}/_ext/1611813562/system_clock.o: ../../../common/firmware/microchip/src/system_clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/system_clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/system_clock.o.ok ${OBJECTDIR}/_ext/1611813562/system_clock.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/system_clock.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/system_clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/system_clock.o.d" -o ${OBJECTDIR}/_ext/1611813562/system_clock.o ../../../common/firmware/microchip/src/system_clock.c    
	
${OBJECTDIR}/_ext/1360937237/bitbang.o: ../src/bitbang.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bitbang.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bitbang.o.ok ${OBJECTDIR}/_ext/1360937237/bitbang.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bitbang.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/bitbang.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/bitbang.o.d" -o ${OBJECTDIR}/_ext/1360937237/bitbang.o ../src/bitbang.c    
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.ok ${OBJECTDIR}/_ext/1360937237/crc16.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/crc16.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d" -o ${OBJECTDIR}/_ext/1360937237/crc16.o ../src/crc16.c    
	
${OBJECTDIR}/_ext/1360937237/iso6b.o: ../src/iso6b.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/iso6b.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/iso6b.o.ok ${OBJECTDIR}/_ext/1360937237/iso6b.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/iso6b.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/iso6b.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/iso6b.o.d" -o ${OBJECTDIR}/_ext/1360937237/iso6b.o ../src/iso6b.c    
	
else
${OBJECTDIR}/_ext/1360937237/as3993.o: ../src/as3993.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/as3993.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/as3993.o.ok ${OBJECTDIR}/_ext/1360937237/as3993.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/as3993.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/as3993.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/as3993.o.d" -o ${OBJECTDIR}/_ext/1360937237/as3993.o ../src/as3993.c    
	
${OBJECTDIR}/_ext/1360937237/gen2.o: ../src/gen2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/gen2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/gen2.o.ok ${OBJECTDIR}/_ext/1360937237/gen2.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/gen2.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/gen2.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/gen2.o.d" -o ${OBJECTDIR}/_ext/1360937237/gen2.o ../src/gen2.c    
	
${OBJECTDIR}/_ext/1360937237/global.o: ../src/global.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/global.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/global.o.ok ${OBJECTDIR}/_ext/1360937237/global.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/global.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/global.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/global.o.d" -o ${OBJECTDIR}/_ext/1360937237/global.o ../src/global.c    
	
${OBJECTDIR}/_ext/1611813562/logger.o: ../../../common/firmware/microchip/src/logger.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/logger.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/logger.o.ok ${OBJECTDIR}/_ext/1611813562/logger.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/logger.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/logger.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/logger.o.d" -o ${OBJECTDIR}/_ext/1611813562/logger.o ../../../common/firmware/microchip/src/logger.c    
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.ok ${OBJECTDIR}/_ext/1360937237/main.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    
	
${OBJECTDIR}/_ext/1360937237/timer.o: ../src/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer.o.ok ${OBJECTDIR}/_ext/1360937237/timer.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/timer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/timer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/timer.o.d" -o ${OBJECTDIR}/_ext/1360937237/timer.o ../src/timer.c    
	
${OBJECTDIR}/_ext/1360937237/tuner.o: ../src/tuner.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/tuner.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/tuner.o.ok ${OBJECTDIR}/_ext/1360937237/tuner.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/tuner.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/tuner.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/tuner.o.d" -o ${OBJECTDIR}/_ext/1360937237/tuner.o ../src/tuner.c    
	
${OBJECTDIR}/_ext/1611813562/uart_driver.o: ../../../common/firmware/microchip/src/uart_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_driver.o.ok ${OBJECTDIR}/_ext/1611813562/uart_driver.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/uart_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/uart_driver.o.d" -o ${OBJECTDIR}/_ext/1611813562/uart_driver.o ../../../common/firmware/microchip/src/uart_driver.c    
	
${OBJECTDIR}/_ext/1611813562/bootloadable.o: ../../../common/firmware/microchip/src/bootloadable.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/bootloadable.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/bootloadable.o.ok ${OBJECTDIR}/_ext/1611813562/bootloadable.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/bootloadable.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/bootloadable.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/bootloadable.o.d" -o ${OBJECTDIR}/_ext/1611813562/bootloadable.o ../../../common/firmware/microchip/src/bootloadable.c    
	
${OBJECTDIR}/_ext/1611813562/spi_driver.o: ../../../common/firmware/microchip/src/spi_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/spi_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/spi_driver.o.ok ${OBJECTDIR}/_ext/1611813562/spi_driver.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/spi_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/spi_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/spi_driver.o.d" -o ${OBJECTDIR}/_ext/1611813562/spi_driver.o ../../../common/firmware/microchip/src/spi_driver.c    
	
${OBJECTDIR}/_ext/1360937237/platform.o: ../src/platform.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/platform.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/platform.o.ok ${OBJECTDIR}/_ext/1360937237/platform.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/platform.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/platform.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/platform.o.d" -o ${OBJECTDIR}/_ext/1360937237/platform.o ../src/platform.c    
	
${OBJECTDIR}/_ext/1360937237/appl_commands.o: ../src/appl_commands.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands.o.ok ${OBJECTDIR}/_ext/1360937237/appl_commands.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/appl_commands.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/appl_commands.o.d" -o ${OBJECTDIR}/_ext/1360937237/appl_commands.o ../src/appl_commands.c    
	
${OBJECTDIR}/_ext/1360937237/appl_commands_table.o: ../src/appl_commands_table.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.ok ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/appl_commands_table.o.d" -o ${OBJECTDIR}/_ext/1360937237/appl_commands_table.o ../src/appl_commands_table.c    
	
${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o: ../../../common/firmware/microchip/src/stream_dispatcher.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.ok ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o.d" -o ${OBJECTDIR}/_ext/1611813562/stream_dispatcher.o ../../../common/firmware/microchip/src/stream_dispatcher.c    
	
${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o: ../../../common/firmware/microchip/src/uart_stream_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.ok ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o.d" -o ${OBJECTDIR}/_ext/1611813562/uart_stream_driver.o ../../../common/firmware/microchip/src/uart_stream_driver.c    
	
${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o: ../../../common/firmware/microchip/src/weak_stream_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.ok ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o.d" -o ${OBJECTDIR}/_ext/1611813562/weak_stream_functions.o ../../../common/firmware/microchip/src/weak_stream_functions.c    
	
${OBJECTDIR}/_ext/1611813562/system_clock.o: ../../../common/firmware/microchip/src/system_clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1611813562" 
	@${RM} ${OBJECTDIR}/_ext/1611813562/system_clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1611813562/system_clock.o.ok ${OBJECTDIR}/_ext/1611813562/system_clock.o.err 
	@${RM} ${OBJECTDIR}/_ext/1611813562/system_clock.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1611813562/system_clock.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1611813562/system_clock.o.d" -o ${OBJECTDIR}/_ext/1611813562/system_clock.o ../../../common/firmware/microchip/src/system_clock.c    
	
${OBJECTDIR}/_ext/1360937237/bitbang.o: ../src/bitbang.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bitbang.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bitbang.o.ok ${OBJECTDIR}/_ext/1360937237/bitbang.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/bitbang.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/bitbang.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/bitbang.o.d" -o ${OBJECTDIR}/_ext/1360937237/bitbang.o ../src/bitbang.c    
	
${OBJECTDIR}/_ext/1360937237/crc16.o: ../src/crc16.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o.ok ${OBJECTDIR}/_ext/1360937237/crc16.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/crc16.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/crc16.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/crc16.o.d" -o ${OBJECTDIR}/_ext/1360937237/crc16.o ../src/crc16.c    
	
${OBJECTDIR}/_ext/1360937237/iso6b.o: ../src/iso6b.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/iso6b.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/iso6b.o.ok ${OBJECTDIR}/_ext/1360937237/iso6b.o.err 
	@${RM} ${OBJECTDIR}/_ext/1360937237/iso6b.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/iso6b.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -fno-short-double -Wall -DUSE_LOGGER -DUSE_UART_STREAM_DRIVER -I"../src" -I"../../../common/include" -I"../../../common/firmware/microchip/include" -O1 -MMD -MF "${OBJECTDIR}/_ext/1360937237/iso6b.o.d" -o ${OBJECTDIR}/_ext/1360937237/iso6b.o ../src/iso6b.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/AS3993_uart.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../src/p24FJ64GB002.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_ICD3=1 -o dist/${CND_CONF}/${IMAGE_TYPE}/AS3993_uart.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,--no-check-sections,-L"..",-Map="$(BINDIR_)$(TARGETBASE).map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_ICD3=1
else
dist/${CND_CONF}/${IMAGE_TYPE}/AS3993_uart.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../src/p24FJ64GB002.gld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/AS3993_uart.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,--no-check-sections,-L"..",-Map="$(BINDIR_)$(TARGETBASE).map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/AS3993_uart.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/debug
	${RM} -r dist/debug

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
