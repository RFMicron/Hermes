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
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/unity_test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/unity_test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../test/test_all.c ../test/unity.c ../test/test_all_Runner.c ../../src/i2c_driver.c ../../src/spi_driver.c ../../src/uart_driver.c ../../src/system_clock.c ../../src/weak_stream_functions.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/760600703/test_all.o ${OBJECTDIR}/_ext/760600703/unity.o ${OBJECTDIR}/_ext/760600703/test_all_Runner.o ${OBJECTDIR}/_ext/1445274692/i2c_driver.o ${OBJECTDIR}/_ext/1445274692/spi_driver.o ${OBJECTDIR}/_ext/1445274692/uart_driver.o ${OBJECTDIR}/_ext/1445274692/system_clock.o ${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/760600703/test_all.o.d ${OBJECTDIR}/_ext/760600703/unity.o.d ${OBJECTDIR}/_ext/760600703/test_all_Runner.o.d ${OBJECTDIR}/_ext/1445274692/i2c_driver.o.d ${OBJECTDIR}/_ext/1445274692/spi_driver.o.d ${OBJECTDIR}/_ext/1445274692/uart_driver.o.d ${OBJECTDIR}/_ext/1445274692/system_clock.o.d ${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/760600703/test_all.o ${OBJECTDIR}/_ext/760600703/unity.o ${OBJECTDIR}/_ext/760600703/test_all_Runner.o ${OBJECTDIR}/_ext/1445274692/i2c_driver.o ${OBJECTDIR}/_ext/1445274692/spi_driver.o ${OBJECTDIR}/_ext/1445274692/uart_driver.o ${OBJECTDIR}/_ext/1445274692/system_clock.o ${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o

# Source Files
SOURCEFILES=../test/test_all.c ../test/unity.c ../test/test_all_Runner.c ../../src/i2c_driver.c ../../src/spi_driver.c ../../src/uart_driver.c ../../src/system_clock.c ../../src/weak_stream_functions.c


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

# The following macros may be used in the pre and post step lines
Device=PIC24FJ128GB202
ProjectDir="D:\Documents\AS3955\AS3955_NEW\common\firmware\microchip\test\unity_test.X"
ConfName=default
ImagePath="dist\default\${IMAGE_TYPE}\unity_test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ImageDir="dist\default\${IMAGE_TYPE}"
ImageName="unity_test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}"
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IsDebug="true"
else
IsDebug="false"
endif

.build-conf:  .pre ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/unity_test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=24FJ128GB202
MP_LINKER_FILE_OPTION=,--script=p24FJ128GB202.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/760600703/test_all.o: ../test/test_all.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/760600703" 
	@${RM} ${OBJECTDIR}/_ext/760600703/test_all.o.d 
	@${RM} ${OBJECTDIR}/_ext/760600703/test_all.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../test/test_all.c  -o ${OBJECTDIR}/_ext/760600703/test_all.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/760600703/test_all.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/760600703/test_all.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/760600703/unity.o: ../test/unity.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/760600703" 
	@${RM} ${OBJECTDIR}/_ext/760600703/unity.o.d 
	@${RM} ${OBJECTDIR}/_ext/760600703/unity.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../test/unity.c  -o ${OBJECTDIR}/_ext/760600703/unity.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/760600703/unity.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/760600703/unity.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/760600703/test_all_Runner.o: ../test/test_all_Runner.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/760600703" 
	@${RM} ${OBJECTDIR}/_ext/760600703/test_all_Runner.o.d 
	@${RM} ${OBJECTDIR}/_ext/760600703/test_all_Runner.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../test/test_all_Runner.c  -o ${OBJECTDIR}/_ext/760600703/test_all_Runner.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/760600703/test_all_Runner.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/760600703/test_all_Runner.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/i2c_driver.o: ../../src/i2c_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/i2c_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/i2c_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/i2c_driver.c  -o ${OBJECTDIR}/_ext/1445274692/i2c_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/i2c_driver.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/i2c_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/spi_driver.o: ../../src/spi_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/spi_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/spi_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/spi_driver.c  -o ${OBJECTDIR}/_ext/1445274692/spi_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/spi_driver.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/spi_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/uart_driver.o: ../../src/uart_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/uart_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/uart_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/uart_driver.c  -o ${OBJECTDIR}/_ext/1445274692/uart_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/uart_driver.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/uart_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/system_clock.o: ../../src/system_clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/system_clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/system_clock.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/system_clock.c  -o ${OBJECTDIR}/_ext/1445274692/system_clock.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/system_clock.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/system_clock.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o: ../../src/weak_stream_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/weak_stream_functions.c  -o ${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o.d"      -g -D__DEBUG   -mno-eds-warn  -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/760600703/test_all.o: ../test/test_all.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/760600703" 
	@${RM} ${OBJECTDIR}/_ext/760600703/test_all.o.d 
	@${RM} ${OBJECTDIR}/_ext/760600703/test_all.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../test/test_all.c  -o ${OBJECTDIR}/_ext/760600703/test_all.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/760600703/test_all.o.d"      -mno-eds-warn  -g -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/760600703/test_all.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/760600703/unity.o: ../test/unity.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/760600703" 
	@${RM} ${OBJECTDIR}/_ext/760600703/unity.o.d 
	@${RM} ${OBJECTDIR}/_ext/760600703/unity.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../test/unity.c  -o ${OBJECTDIR}/_ext/760600703/unity.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/760600703/unity.o.d"      -mno-eds-warn  -g -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/760600703/unity.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/760600703/test_all_Runner.o: ../test/test_all_Runner.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/760600703" 
	@${RM} ${OBJECTDIR}/_ext/760600703/test_all_Runner.o.d 
	@${RM} ${OBJECTDIR}/_ext/760600703/test_all_Runner.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../test/test_all_Runner.c  -o ${OBJECTDIR}/_ext/760600703/test_all_Runner.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/760600703/test_all_Runner.o.d"      -mno-eds-warn  -g -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/760600703/test_all_Runner.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/i2c_driver.o: ../../src/i2c_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/i2c_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/i2c_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/i2c_driver.c  -o ${OBJECTDIR}/_ext/1445274692/i2c_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/i2c_driver.o.d"      -mno-eds-warn  -g -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/i2c_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/spi_driver.o: ../../src/spi_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/spi_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/spi_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/spi_driver.c  -o ${OBJECTDIR}/_ext/1445274692/spi_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/spi_driver.o.d"      -mno-eds-warn  -g -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/spi_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/uart_driver.o: ../../src/uart_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/uart_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/uart_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/uart_driver.c  -o ${OBJECTDIR}/_ext/1445274692/uart_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/uart_driver.o.d"      -mno-eds-warn  -g -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/uart_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/system_clock.o: ../../src/system_clock.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/system_clock.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/system_clock.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/system_clock.c  -o ${OBJECTDIR}/_ext/1445274692/system_clock.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/system_clock.o.d"      -mno-eds-warn  -g -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/system_clock.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o: ../../src/weak_stream_functions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1445274692" 
	@${RM} ${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o.d 
	@${RM} ${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../../src/weak_stream_functions.c  -o ${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o.d"      -mno-eds-warn  -g -omf=elf -O0 -I"../../include" -I"../../../../include" -DUNITY_TEST -msmart-io=1 -Wall -msfr-warn=off
	@${FIXDEPS} "${OBJECTDIR}/_ext/1445274692/weak_stream_functions.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/unity_test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/unity_test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG   -omf=elf     -Wl,--local-stack,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/unity_test.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/unity_test.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -Wl,--local-stack,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/unity_test.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
endif

.pre:
	@echo "--------------------------------------"
	@echo "User defined pre-build step: [C:\Ruby21-x64\bin\ruby.exe ${ProjectDir}\unity\generate_test_runner.rb ..\test\test_all.c]"
	@C:\Ruby21-x64\bin\ruby.exe ${ProjectDir}\unity\generate_test_runner.rb ..\test\test_all.c
	@echo "--------------------------------------"

# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
