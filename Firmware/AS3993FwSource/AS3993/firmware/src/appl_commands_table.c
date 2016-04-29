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
  * @brief Table to branch into the functions associated with a specific command.
  *
  * This file provides a table (call_fkt_()) which allows to branch into functions
  * depending on which command has been received. Each command has an 8bit identifier.
  * This identifier is also used as offset in the table, to branch into the corresponding
  * function. The 8bit identifier is rx-ed from the host (command ID) as described
  * in \ref streamprotocol.
  *
  * @author Ulrich Herrmann
  * @author Bernhard Breinbauer
  */

#include "as3993_config.h"
#include "as3993.h"
#include "global.h"
#include "appl_commands.h"

void callWrongCommand(void);
void callReaderConfig(void);
void callTunerTable(void);
void callAutoTuner(void);
void callAntennaTuner(void);
void callAntennaPower(void);
void callConfigGen2(void);
void callConfigTxRx(void);
void callInventory6B(void);
void callReadFromTag6B(void);
void callWriteToTag6B(void);
void callInventoryGen2(void);
void callSelectTag(void);
void callWriteToTag(void);
void callChangeFreq(void);
void callGetTagData(void);
void callReadFromTag(void);
void callLockUnlockTag(void);
void callKillTag(void);
void callStartStop(void);
void callGenericCommand(void);
void callRSSIMeasureCMD(void);
void callConfigPA(void);

/**
 * Table of function pointers. The offset in the table matches the command identifier.
 * When a command is received the corresponding function will be called via this table.
 */
struct callFunction call_fkt_[CALL_FKT_SIZE] =
{
    { CMD_READER_CONFIG_MIN_REPLY_SIZE, CMD_READER_CONFIG_RX_SIZE, callReaderConfig },  /* CMD_READER_CONFIG       */
    { CMD_ANTENNA_POWER_REPLY_SIZE, CMD_ANTENNA_POWER_RX_SIZE, callAntennaPower },      /* CMD_ANTENNA_POWER_ID    */
    { CMD_CHANGE_FREQ_REPLY_SIZE, CMD_CHANGE_FREQ_RX_SIZE, callChangeFreq   },          /* CMD_CHANGE_FREQ_ID      */
    { CMD_GEN2_SETTINGS_REPLY_SIZE, CMD_GEN2_SETTINGS_RX_SIZE, callConfigGen2   },      /* CMD_GEN2_SETTINGS_ID    */
    { CMD_CONFIG_TX_RX_REPLY_SIZE, CMD_CONFIG_TX_RX_RX_SIZE, callConfigTxRx   },        /* CMD_CONFIG_TX_RX_ID     */
    { CMD_INVENTORY_GEN2_REPLY_SIZE, CMD_INVENTORY_GEN2_RX_SIZE, callInventoryGen2},    /* CMD_INVENTORY_GEN2_ID   */
    { CMD_SELECT_REPLY_SIZE, CMD_SELECT_RX_SIZE, callSelectTag    },                    /* CMD_SELECT_TAG_ID       */
    { CMD_WRITE_TO_TAG_REPLY_SIZE, CMD_WRITE_TO_TAG_RX_SIZE, callWriteToTag   },        /* CMD_WRITE_TO_TAG_ID     */
    { CMD_READ_FROM_TAG_REPLY_SIZE, CMD_READ_FROM_TAG_RX_SIZE, callReadFromTag  },      /* CMD_READ_FROM_TAG_ID    */
    { CMD_LOCK_REPLY_SIZE, CMD_LOCK_RX_SIZE, callLockUnlockTag   },                     /* CMD_LOCK_UNLOCK_ID      */
    { CMD_KILL_TAG_REPLY_SIZE, CMD_KILL_TAG_RX_SIZE, callKillTag      },                /* CMD_KILL_TAG_ID         */
    { CMD_GET_TAG_DATA_REPLY_SIZE, CMD_GET_TAG_DATA_RX_SIZE, callGetTagData      },   /* CMD_GET_TAG_DATA_ID     */
    { 0, CMD_START_STOP_RX_SIZE, callStartStop    },                                    /* CMD_START_STOP_ID       */
    { CMD_TUNER_TABLE_REPLY_SIZE, CMD_TUNER_TABLE_RX_SIZE, callTunerTable   },          /* CMD_TUNER_TABLE         */
    { CMD_AUTO_TUNER_REPLY_SIZE, CMD_AUTO_TUNER_RX_SIZE, callAutoTuner    },            /* CMD_AUTO_TUNER          */
    { CMD_ANTENNA_TUNER_REPLY_SIZE, CMD_ANTENNA_TUNER_RX_SIZE, callAntennaTuner },      /* CMD_ANTENNA_TUNER       */
    { 0, 0, callWrongCommand }, /* 16: was second gen2 inventory command, not used anymore */
    { CMD_INVENTORY_6B_REPLY_SIZE, CMD_INVENTORY_6B_RX_SIZE, callInventory6B  },        /* CMD_INVENTORY_6B_ID     */
    { CMD_READ_FROM_TAG_6B_REPLY_SIZE, CMD_READ_FROM_TAG_6B_RX_SIZE, callReadFromTag6B},/* CMD_READ_FROM_TAG_6B_ID */
    { CMD_WRITE_TO_TAG_6B_REPLY_SIZE, CMD_WRITE_TO_TAG_6B_RX_SIZE, callWriteToTag6B },  /* CMD_WRITE_TO_TAG_6B_ID  */
    { CMD_GENERIC_CMD_REPLY_SIZE, CMD_GENERIC_CMD_RX_SIZE, callGenericCommand },        /* CMD_GENERIC_CMD_ID      */
    { CMD_RSSI_MEAS_CMD_REPLY_SIZE, CMD_RSSI_MEAS_CMD_RX_SIZE, callRSSIMeasureCMD },    /* CMD_RSSI_MEAS_CMD_ID    */
    { CMD_CONFIG_PA_REPLY_SIZE, CMD_CONFIG_PA_RX_SIZE, callConfigPA }, /* CMD_CALL_PA_ID */
    { 0, 0, callWrongCommand }, /* 23 */
    { 0, 0, callWrongCommand }, /* 24 */
    { 0, 0, callWrongCommand }, /* 25 */
    { 0, 0, callWrongCommand }, /* 26 */
    { 0, 0, callWrongCommand }, /* 27 */
    { 0, 0, callWrongCommand }, /* 28 */
    { 0, 0, callWrongCommand }, /* 29 */
    { 0, 0, callWrongCommand }, /* 30 */
    { 0, 0, callWrongCommand }, /* 31 */
};
