/*
 *****************************************************************************
 * Copyright by ams AG                                                       *
 * All rights are reserved.                                                  *
 *                                                                           *
 * IMPORTANT - PLEASE READ CAREFULLY BEFORE COPYING, INSTALLING OR USING     *
 * THE SOFTWARE.                                                             *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT         *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS         *
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT          *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
 *****************************************************************************
 */
/** @file
 * @brief This file is the include file for the appl_commands.c file.
 *
 * This file contains defines for handling appl commands, eg: The defines for the
 * index of command functions in call_ftk_() and expected rx and tx data sizes.
 *
 * @author Ulrich Herrmann
 * @author Bernhard Breinbauer
 * @author Rene Eggerstorfer
 */

#ifndef __USB_COMMANDS_H__
#define __USB_COMMANDS_H__

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "global.h"

#define CALL_FKT_SIZE           32

struct callFunction {
    u16 min_tx_size;
    u16 min_rx_size;
    void (*const func)(void);
};

/*
 ******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************
 */
extern u8 commands( u8 protocol, u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData );
extern void initCommands(void);
extern u8 uartCommands(void);
extern u8 sendTagData( u8 * protocol, u16 * txSize, u8 * txData, u16 remainingSize );
extern int doCyclicInventory(void);

extern u8 readRegister(u8 addr, u16 * txSize, u8 * txData);
extern u8 readRegisters(u16 * txSize, u8 * txData);
extern u8 writeRegister(u8 addr, u8 value, u16 * txSize, u8 * txData);


/* command functions table */
extern struct callFunction call_fkt_[CALL_FKT_SIZE];

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

/*USB COMMANDS (READER) */
/** appl command ID for callReaderConfig() */
#define CMD_READER_CONFIG       0
/** appl command ID for callAntennaPower() */
#define CMD_ANTENNA_POWER       1
/** appl command ID for callChangeFreq() */
#define CMD_CHANGE_FREQ         2
/** appl command ID for callConfigGen2() */
#define CMD_GEN2_SETTINGS       3
/** appl command ID for callConfigTxRx() */
#define CMD_CONFIG_TX_RX        4
/** appl command ID for callInventoryGen2() */
#define CMD_INVENTORY_GEN2      5
/** appl command ID for callSelectTag() */
#define CMD_SELECT_TAG          6
/** appl command ID for callWriteToTag() */
#define CMD_WRITE_TO_TAG        7
/** appl command ID for callReadFromTag() */
#define CMD_READ_FROM_TAG       8
/** appl command ID for callLockUnlockTag() */
#define CMD_LOCK_UNLOCK_TAG     9
/** appl command ID for callKillTag() */
#define CMD_KILL_TAG            10
/** appl command ID for getTagData()  */
#define CMD_GET_TAG_DATA        11
/** appl command ID for callStartStop() */
#define CMD_START_STOP          12
/** appl command ID for callTunerTable() */
#define CMD_TUNER_TABLE         13
/** appl command ID for callAutoTuner() */
#define CMD_AUTO_TUNER          14
/** appl command ID for callAntennaTuner() */
#define CMD_ANTENNA_TUNER       15
/** appl command ID for callInventory6B() */
#define CMD_INVENTORY_6B        17
/** appl command ID for callReadFromTag6B() */
#define CMD_READ_FROM_TAG_6B    18
/** appl command ID for callWriteToTag6B() */
#define CMD_WRITE_TO_TAG_6B     19
/** appl command ID for callGenericCMD() */
#define CMD_GENERIC_CMD_ID      20
/** appl command ID for callRSSIMeasureCMD() */
#define CMD_RSSI_MEAS_CMD_ID    21

/** appl command ID for callReaderConfig() */
#define CMD_READER_CONFIG_RESP       22
/** appl command ID for callAntennaPower() */
#define CMD_ANTENNA_POWER_RESP       23
/** appl command ID for callChangeFreq() */
#define CMD_CHANGE_FREQ_RESP         24
/** appl command ID for callConfigGen2() */
#define CMD_GEN2_SETTINGS_RESP       25
/** appl command ID for callConfigTxRx() */
#define CMD_CONFIG_TX_RX_RESP        26
/** appl command ID for callInventoryGen2() */
#define CMD_INVENTORY_GEN2_RESP      27
/** appl command ID for callSelectTag() */
#define CMD_SELECT_TAG_RESP          28
/** appl command ID for callWriteToTag() */
#define CMD_WRITE_TO_TAG_RESP        29
/** appl command ID for callReadFromTag() */
#define CMD_READ_FROM_TAG_RESP       30
/** appl command ID for callLockUnlockTag() */
#define CMD_LOCK_UNLOCK_TAG_RESP     31
/** appl command ID for callKillTag() */
#define CMD_KILL_TAG_RESP            32
/** appl command ID for callStartStop() */
#define CMD_START_STOP_RESP          33
/** appl command ID for callTunerTable() */
#define CMD_TUNER_TABLE_RESP         34
/** appl command ID for callAutoTuner() */
#define CMD_AUTO_TUNER_RESP          35
/** appl command ID for callAntennaTuner() */
#define CMD_ANTENNA_TUNER_RESP       36
/** appl command ID for callInventory6B() */
#define CMD_INVENTORY_6B_RESP        37
/** appl command ID for callReadFromTag6B() */
#define CMD_READ_FROM_TAG_6B_RESP    38
/** appl command ID for callWriteToTag6B() */
#define CMD_WRITE_TO_TAG_6B_RESP     39
/** appl command ID for callGenericCMD() */
#define CMD_GENERIC_CMD_ID_RESP      40
/** appl command ID for callRSSIMeasureCMD() */
#define CMD_RSSI_MEAS_CMD_ID_RESP    41
/** appl command ID for getTagData() */
#define CMD_GET_TAG_DATA_RESP        42

#define COM_CTRL_CMD_FW_NUMBER_RESP  43
#define COM_CTRL_CMD_RESET_RESP      44
#define COM_CTRL_CMD_FW_INFORMATION_RESP 45
#define COM_CTRL_CMD_ENTER_BOOTLOADER_RESP 46
#define COM_WRITE_REG_RESP           47
#define COM_READ_REG_RESP            48

/*Size */
#define CMD_READER_CONFIG_MIN_REPLY_SIZE    9
#define CMD_READER_CONFIG_REPLY_SIZE        10

#define CMD_READER_CONFIG_RX_SIZE           2

#define CMD_TUNER_TABLE_REPLY_SIZE          3
#define CMD_TUNER_TABLE_RX_SIZE             1

#define CMD_AUTO_TUNER_REPLY_SIZE           0
#define CMD_AUTO_TUNER_RX_SIZE              1

#if RADON
#define CMD_ANTENNA_TUNER_REPLY_SIZE        12
#else
#define CMD_ANTENNA_TUNER_REPLY_SIZE        6
#endif
#define CMD_ANTENNA_TUNER_RX_SIZE           6

#define CMD_ANTENNA_POWER_REPLY_SIZE        1
#define CMD_ANTENNA_POWER_RX_SIZE           2

#define CMD_WRITE_TO_TAG_REPLY_SIZE         2
#define CMD_WRITE_TO_TAG_RX_SIZE            9

#define CMD_READ_FROM_TAG_REPLY_SIZE        2
#define CMD_READ_FROM_TAG_RX_SIZE           9

#define CMD_INVENTORY_6B_REPLY_SIZE         0x0
#define CMD_INVENTORY_6B_RX_SIZE            0x0

#define CMD_WRITE_TO_TAG_6B_REPLY_SIZE      0x01
#define CMD_WRITE_TO_TAG_6B_RX_SIZE         0x01

#define CMD_GENERIC_CMD_REPLY_SIZE          0
#define CMD_GENERIC_CMD_RX_SIZE             0

#define CMD_RSSI_MEAS_CMD_REPLY_SIZE        0
#define CMD_RSSI_MEAS_CMD_RX_SIZE           0

#define CMD_READ_FROM_TAG_6B_REPLY_SIZE     0x08
#define CMD_READ_FROM_TAG_6B_RX_SIZE        0x08

#define CMD_CHANGE_FREQ_REPLY_SIZE          0
#define CMD_CHANGE_FREQ_RX_SIZE             1
#define CMD_CHANGE_FREQ_RSSI_RX_SIZE        4
#define CMD_CHANGE_FREQ_REFL_RX_SIZE        5
#define CMD_CHANGE_FREQ_ADD_RX_SIZE         6
#define CMD_CHANGE_FREQ_SETHOP_RX_SIZE      8
#define CMD_CHANGE_FREQ_CONTMOD_RX_SIZE     18

#define CMD_INVENTORY_GEN2_REPLY_SIZE       0
#define CMD_INVENTORY_GEN2_RX_SIZE          3

#define CMD_LOCK_REPLY_SIZE                 1
#define CMD_LOCK_RX_SIZE                    7

#define CMD_SELECT_REPLY_SIZE               0
#define CMD_SELECT_RX_SIZE                  2

#define CMD_KILL_TAG_REPLY_SIZE             1
#define CMD_KILL_TAG_RX_SIZE                6

#define CMD_GET_TAG_DATA_REPLY_SIZE         0
#define CMD_GET_TAG_DATA_RX_SIZE            0

#define CMD_GEN2_SETTINGS_REPLY_SIZE        16
#define CMD_GEN2_SETTINGS_RX_SIZE           16

#define CMD_CONFIG_TX_RX_REPLY_SIZE         4
#define CMD_CONFIG_TX_RX_RX_SIZE            4

#define CMD_START_STOP_REPLY_SIZE           0
#define CMD_START_STOP_RX_SIZE              5

#define CMD_ERROR_REPLY_SIZE                1
/*Protocol Bytes */

/*Command Antenna Power */
#define ANT_POWER_OFF           0x00
#define ANT_POWER_ON            0xFF

/* read/write registers */
#define WRITE_REG_REPLY_SIZE            1
#define WRITE_REG_RX_SIZE               2

#define READ_REG_REPLY_SIZE             1
#define READ_REG_RX_SIZE                2

#define READ_REGS_COMPLETE_REPLY_SIZE   48
#define READ_REGS_COMPLETE_RX_SIZE      2

#define CMD_CONFIG_PA_REPLY_SIZE  1
#define CMD_CONFIG_PA_RX_SIZE  2


#endif

