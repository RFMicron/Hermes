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
 * @brief Functions which handle commands received via USB or UART.
 *
 * This file contains all functions for processing commands received via either USB or UART.
 * It implements the parsing of reports data, executing the requested command
 * and sending data back.
 *
 * A description of the protocol between host and FW is included in the
 * documentation for commands().
 * The documentation of the various appl command functions (call*) will only discuss the
 * payload of the command data and will not include the header information for
 * every transmitted packet, as this is already described for commands().
 *
 * \n
 * The frequency hopping is also done in this file before calling protocol/device
 * specific functions.
 *
 * @author Ulrich Herrmann
 * @author Bernhard Breinbauer
 * @author Rene Eggerstorfer
 */

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "as3993_config.h"
#include "platform.h"
#include "global.h"
#include "gen2.h"
#include "as3993_public.h"
#include "appl_commands.h"
#include "logger.h"
#include "errno.h"
#include "iso6b.h"
#include "timer.h"
#include "string.h"
#include <limits.h>
#include "as3993.h"
#include "tuner.h"

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */
/** Define this to 1 if you want to have logging for appl commands. */
#define APPLDEBUG               0

/** Identifier for gen2 protocol session */
#define SESSION_GEN2            1
/** Identifier for ISO6B protocol session */
#define SESSION_ISO6B           2

/** Maximum of consecutive select commands currently supported. */
#define MAX_SELECTS             3

/** value for #readerPowerDownMode. Activates power down mode. (EN low)*/
#define POWER_DOWN              0
/** value for #readerPowerDownMode. Activates normal mode. (EN high, RF off, stdby 0)*/
#define POWER_NORMAL            1
/** value for #readerPowerDownMode. Activates normal mode with rf field on. (EN high, RF off, stdby 0)*/
#define POWER_NORMAL_RF         2
/** value for #readerPowerDownMode. Activates standby mode. (EN high, RF off, stdby 1)*/
#define POWER_STANDBY           3

#if APPLDEBUG && (USE_LOGGER == LOGGER_ON)
#define APPLOG dbgLog /*!< macro used for printing debug messages */
#define APPLOGDUMP dbgHexDump /*!< macro used for dumping buffers */
#else
#define APPLOG(...) /*!< macro used for printing debug messages if USE_LOGGER is set */
#define APPLOGDUMP(...) /*!< macro used for dumping buffers if USE_LOGGER is set */
#endif

#define SENDTAGHEADER       3
#define SENDNOTAGINFO       7
#define TAGFIXDATALEN      11
#define SENDTAGFIXDATALEN   8
#define SENDTAGMINLEN       (SENDTAGHEADER + SENDTAGFIXDATALEN)

/*
 ******************************************************************************
 * LOCAL TYPES
 ******************************************************************************
 */
/** Local structure, which provides access to parameters of command() to all
 * appl command functions (call*()). */
struct CmdBuffer{
    const u8 *rxData;
    u16 rxSize;
    u8 *txData;
    u16 txSize;
    s8 result;
};

/*
 ******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************
 */
#if RUN_ON_AS3980
/** Default Gen2 configuration, this is the only supported configuration for AS3980. */
static struct gen2Config gen2Configuration = {TARI_25, GEN2_LF_40, GEN2_COD_MILLER8, TREXT_ON, 0, GEN2_SESSION_S0, 0};
/** Start value for Q for Gen2 inventory rounds, this is the only supported value for AS3980. */
u8 gen2qbegin = 0;
#else

#if FEMTO2 || FEMTO2_1 || RADON || (NEWTON && !RUN_ON_AS3980 )
/** Default Gen2 configuration, can be changed callConfigGen2(). */
static struct gen2Config gen2Configuration = {TARI_25, GEN2_LF_256, GEN2_COD_MILLER4, TREXT_OFF, 0, GEN2_SESSION_S0, 0};
/** Start value for Q for Gen2 inventory rounds, can be changed callConfigGen2(). */
u8 gen2qbegin = 4;
#else
/** Default Gen2 configuration, can be changed callConfigGen2(). */
static struct gen2Config gen2Configuration = {TARI_125, GEN2_LF_256, GEN2_COD_MILLER4, TREXT_OFF, 0, GEN2_SESSION_S0, 0};
/** Start value for Q for Gen2 inventory rounds, can be changed callConfigGen2(). */
u8 gen2qbegin = 4;
#endif

#endif

/** Internal number of currently configured select commands. */
static int num_selects;
/** Parameters for select commands, can be changed via callSelectTag()*/
static struct gen2SelectParams selParams[MAX_SELECTS];

/** Internal variable which contains the number of received tags in the last inventory round. */
static u8 num_of_tags;

/** Internal variable which tells if the special Constant CW select command was performed, which means rest of inventory is aborted. */
static u8 special_select_performed = 0;

/** New tag data has been received in last inventory round and can be sent in sendTagData() now.*/
static u8 tagDataAvailable;

/** AS3993 init status. This is the return value of as3993Initialize() */
u16 readerInitStatus;

/** Array of Structures which stores all necessary Information about the Tags.
 */
Tag tags_[MAXTAG];

/**Pointer to data of currently selected Tag.*/
Tag *selectedTag;

/**Contains the list of used frequencies.*/
Freq Frequencies;
/**Profile settings which can be set via GUI commands. We default to europe settings.*/
static u8 guiActiveProfile = 1;
/**Profile settings which can be set via GUI commands. We default to europe settings.*/
static u8 guiNumFreqs=4;
/**Profile settings which can be set via GUI commands. We default to europe settings.*/
static u32 guiMinFreq = 865700;
/**Profile settings which can be set via GUI commands. We default to europe settings.*/
static u32 guiMaxFreq = 867500;
/** If rssi measurement is above this threshold the channel is regarded as
    used and the system will hop to the next frequency. Otherwise this frequency is used */
static s8 rssiThreshold;
/**Profile settings which can be set via GUI commands. We default to europe settings.\n
 Before starting the frequency hop this time (in ms) will be waited for.*/
static u16 idleTime = 0;
/**Profile settings which can be set via GUI commands. We default to europe settings.\n
 Maximum allocation time (in ms) of a channel.*/
static u16 maxSendingTime = 10000;
/**Profile settings which can be set via GUI commands. We default to europe settings.\n
 Measure rssi for this time (in ms) before deciding if the channel should be used.*/
static u16 listeningTime = 1;
/** Maximal channel arbitration time in ms (internal value of maxSendingTime) */
static u16 maxSendingLimit;
/** Will be set to 1 when #maxSendingLimit in continueCheckTimeout() timed out. */
static u8 maxSendingLimitTimedOut;
/** Will be set to 1 if a cyclic inventory is performed, see callStartStop(). */
static u8 cyclicInventory;
/** If set to 0 inventory round commandos will be triggered by the FW, otherwise
 * the autoACK feature of the reader will be used which sends the required Gen2
 * commands automatically.*/
static u8 autoAckMode;
/** If set to 0 normal inventory round will be performed, if set to 1 fast inventory rounds will be performed.
 * The value is set in callStartStop() and callInventoryGen2().
 * For details on normal/fast inventory rounds see parameter singulate of gen2SearchForTags().*/
static u8 fastInventory;
/** If set to 1 an read of the TID will be performed in invontory rounds.
 * The value is set in callStartStop() and callInventoryGen2().
 * For details see parameter followTagCommand of gen2SearchForTags() and gen2FollowRead(Tag *tag, s8 *readErr).
 */
static u8 read_TID_CAL_inInventoryRound;
static u8 read_MMS_VFC_TEMP_inInventoryRound;

/** Value for register AS3993_REG_STATUSPAGE. This defines what RSSI value is sent
 * to the host along with the tag data. The value is set in callStartStop() and callInventoryGen2(). */
static u8 rssiMode;
/** Currently configured power down mode of reader. Available modes are: #POWER_DOWN, #POWER_NORMAL,
 *  #POWER_NORMAL_RF and #POWER_STANDBY */
static u8 readerPowerDownMode;
/** To be communicated to GUI, basically result of hopFrequencies(), having information on skipped rounds */
static s8 inventoryResult;

#ifdef ANTENNA_SWITCH
/** Stores which antenna port is used atm. valid values are 1 and 2. */
#if RADON || FEMTO2 || FEMTO2_1 || NEWTON
static u8 usedAntenna = 2;
#else
static u8 usedAntenna = 1;
#endif
#endif
#ifdef TUNER
/** The tuningTable contains a list of frequencies and values for the tuner for every frequency.
 * When frequency hopping is performed (hopFrequencies()) the closest frequency in the list is looked up
 * and the corresponding tuner values are applied to the DTCs of the tuner.
 * The tuningTable can be modified via callTunerTable().  */
static TuningTable tuningTable __attribute__((far));
/** Default and current tuner settings. */
static TunerParameters tunerParams = {15, 15, 15};
#if RADON
static TunerParameters tunerAnt1Params = {15, 15, 15};
#endif
#endif

/** Structure which contains the command data which has been received and shall be sent.
 * Provides access to parameters of commands() to all appl command funtions (call*()). */
static struct CmdBuffer cmdBuffer;
/** Index of currently used frequency in frequency table #Frequencies. Used in hopFrequencies(). */
static u16 currentFreqIdx;
/** Currently used protocol, valid values are: #SESSION_GEN2 and #SESSION_ISO6B. */
static u8 currentSession = 0;      // start with invalid session (neither Gen2 nor ISO 6b)

void configTxRx(void);
void readerConfig(void);
void antennaPower(void);
void antennaTuner(void);
void autoTuner(void);
void tunerTable(void);
u8 inventoryGen2(void);
void gen2FollowRead(Tag *tag, s8 *readErr);
void wrongCommand(void);
void initCommands(void);

void selectTag(void);
void writeToTag(void);
void readFromTag(void);
void executeGenericCommand(void);
void executeRSSICMD(void);
void lockUnlockTag(void);
static void hopChannelRelease(void);
static s8 hopFrequencies(void);
static void powerDownReader(void);
static void powerUpReader(void);
#ifdef TUNER
static void applyTunerSettingForFreq(u32 freq);
#endif
/**
 * This function can be used as callback parameter for gen2SearchForTags().
 * It will return 1 as long as allocation timeout has not been exceeded yet.
 * If allocation timeout has occured it will return 0.
 * @return 1 if allocation timeout has not been exceeded yet.
 */
static BOOL continueCheckTimeout( ) 
{
    if (maxSendingLimit == 0) return 1;
    if ( slowTimerValue() >= maxSendingLimit )
    {
        APPLOG("allocation timeout\n");
        slowTimerStop();
        maxSendingLimitTimedOut = 1;
        return 0;
    }
    return 1;
}

/** This function can be used instead of continueCheckTimeout() to circumvent
 * allocation timeouts as this function always returns 1.  */
//static BOOL continueAlways(void)
//{
//    return 1;
//}

/** This funcition checks the current session, if necessary closes it
and opens a new session. Valid session values are: #SESSION_GEN2 and #SESSION_ISO6B. */
static void checkAndSetSession( u8 newSession)
{
    if (currentSession == newSession) return;
    switch (currentSession)
    {
        case SESSION_GEN2:
            gen2Close();
            break;
        case SESSION_ISO6B:
#if ISO6B
            iso6bClose();
#endif
            break;
    }
    switch (newSession)
    {
        case SESSION_GEN2:
            gen2Open(&gen2Configuration);
            break;
        case SESSION_ISO6B:
#if ISO6B
            iso6bOpen();
#endif
            break;
    }
    currentSession = newSession;
}

/**
 * Dummy appl command which is called if an invalid command ID was received.
 * Logs error message and returns error ERR_REQUEST to host.
 */
void callWrongCommand(void)
{
    wrongCommand();
}

void wrongCommand(void)
{

    APPLOG("WRONG COMMAND\n");
    cmdBuffer.result = ERR_REQUEST;
}

void performSelects()
{
    int i;
    for (i = 0; i<num_selects; i++)
    {
        gen2Select(selParams + i);
        if((selParams[i].action == 0x06) &&
           (selParams[i].mask == 0)   &&
           (selParams[i].mask_address == 0xE0) &&
           (selParams[i].mask_len == 0)  &&
           (selParams[i].mem_bank == 0xE0) &&
           (selParams[i].target == 3) &&
           (selParams[i].truncation == 0))
        {
                delay_ms(3);
        }
        else if(selParams[i].target == 8)
        {
            delay_ms(selParams[i].action * 1000)
            special_select_performed = 1;
        }
        /* We would have to wait T4=2*RTcal here (max 140us). Logic analyzer showed enough time without delaying */
    }
}

#define POWER_AND_SELECT_TAG() do{ status = hopFrequencies(); if (status) goto exit; powerAndSelectTag(); if(num_of_tags==0){status=GEN2_ERR_SELECT;goto exit;}}while(0)

static int powerAndSelectTag( void )
{
    performSelects();
    num_of_tags = gen2SearchForTagsAutoAck(tags_+1, 1, 0, continueCheckTimeout, 1, 0, NULL);
    if (num_of_tags == 0)
    {
        APPLOG("Could not select tag\n");
    }
    else
    {
        selectedTag = tags_+1;
    }
    return num_of_tags;
}


/** This function enables/disables the RF field and is executed when a stream
 * packet with protocol = #CMD_ANTENNA_POWER is received.\n
 * The payload of the stream packet looks like this:
 * <table>
 *   <tr><th>   Byte</th><th>     0                     </th><th>  1  </th></tr>
 *   <tr><th>Content</th><td>0x00 for off<br>0xFF for on</td><td>eval_mode</td></tr>
 * </table>
 * eval_mode: reserved, should be zero \n
 * The device sends back:
 * <table>
 *   <tr><th>   Byte</th><th>0 </th></tr>
 *   <tr><th>Content</th><td>0 </td></tr>
 * </table>
 * Status of the reply packet indicates success or error.
 */
void callAntennaPower(void)
{
    antennaPower();
}

void antennaPower(void)
{
    switch (cmdBuffer.rxData[0])
    {
        case ANT_POWER_OFF:
            APPLOG("ANTENNA OFF\n");
            powerUpReader();
            as3993AntennaPower(0);
            powerDownReader();
            cmdBuffer.result = ERR_NONE;
            break;
        case ANT_POWER_ON:
            APPLOG("ANTENNA ON\n");
            powerUpReader();
            as3993AntennaPower(1);
            cmdBuffer.result = ERR_NONE;
            break;
        default:
            APPLOG("ANTENNA ERROR\n");
            cmdBuffer.result = ERR_PARAM;
            break;
    }
    cmdBuffer.txData[0] = 0;
    cmdBuffer.txSize = CMD_ANTENNA_POWER_REPLY_SIZE;
}

/** This function sets and reads antenna tuner network related values and is
 * executed when a stream packet with protocol = #CMD_ANTENNA_TUNER is received.
 * There are different kind of tuning networks available. The full network which
 * is currently supported by the Firmware looks like this:
  \code
                         .--- L ---.
                         |        |
  in ----.----.----- L --.--C_len--.--.-----.-----.----- out
         |    |                      |     |     |
        C_in  L                     C_out   L     R
         |    |                      |     |     |
        ___  ___                    ___   ___   ___
         -    -                      -     -     -
         '    '                      '     '     '
  \endcode
 * On the Femto Reader there is a reduced tuning network: Clen is missing.\n
 * The DTCs capacity values range from:
 * <ul>
 * <li> C_* = 32 steps from 1.3pF to 5.5pF, step size 0.131pF, +-10% </li>
 * </ul>
 * The format of the payload from the host is:
 * Get/Set current tuner parameters:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>1</th>
 *      <th>2</th>
 *      <th>3</th>
 *      <th>4</th>
 *      <th>5</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>set_cin</td>
 *      <td>cin</td>
 *      <td>set_clen</td>
 *      <td>clen</td>
 *      <td>set_cout</td>
 *      <td>cout</td>
 *  </tr>
 * </table>
 * The values are only being set if the proper set_X value is set to 1.\n
 * The device sends back:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>1</th>
 *      <th>2</th>
 *      <th>3</th>
 *      <th>4</th>
 *      <th>5</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>reserved (0)</td>
 *      <td>cin</td>
 *      <td>reserved(0)</td>
 *      <td>clen</td>
 *      <td>reserved(0)</td>
 *      <td>cout</td>
 *  </tr>
 * </table>
 * 
 * Reply status will be set to ERR_NONE if operation was successful. If reader does not support
 * tuning status will be set to ERR_REQUEST.
 */
void callAntennaTuner(void)
{
    antennaTuner();
}

extern TunerConfiguration mainTuner;
#if RADON
extern TunerConfiguration ant1Tuner;
#endif
void antennaTuner(void)
{
    APPLOG("antennaTuner\n");
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);
    memset(cmdBuffer.txData, 0, CMD_ANTENNA_TUNER_REPLY_SIZE);

#ifdef TUNER
    if (cmdBuffer.rxData[0] && mainTuner.cinSenFn)
    {
        tunerParams.cin = cmdBuffer.rxData[1];
        if (tunerParams.cin > 31)
            tunerParams.cin = 31;
        tunerSetCap(&mainTuner, TUNER_CIN, tunerParams.cin);
    }
    if (cmdBuffer.rxData[2] && mainTuner.clenSenFn)
    {
        tunerParams.clen = cmdBuffer.rxData[3];
        if (tunerParams.clen > 31)
            tunerParams.clen = 31;
        tunerSetCap(&mainTuner, TUNER_CLEN, tunerParams.clen);
    }
    if (cmdBuffer.rxData[4] && mainTuner.coutSenFn)
    {
        tunerParams.cout = cmdBuffer.rxData[5];
        if (tunerParams.cout > 31)
            tunerParams.cout = 31;
        tunerSetCap(&mainTuner, TUNER_COUT, tunerParams.cout);
    }
    APPLOG("receive main tuner parameters cin= %hhx, clen= %hhx cout=%hhx\n", tunerParams.cin, tunerParams.clen, tunerParams.cout);
#if RADON
    if (cmdBuffer.rxData[6] && ant1Tuner.cinSenFn)
    {
        tunerAnt1Params.cin = cmdBuffer.rxData[7];
        if (tunerAnt1Params.cin > 31)
            tunerAnt1Params.cin = 31;
        tunerSetCap(&ant1Tuner, TUNER_CIN, tunerAnt1Params.cin);
    }
    if (cmdBuffer.rxData[8] && ant1Tuner.clenSenFn)
    {
        tunerAnt1Params.clen = cmdBuffer.rxData[9];
        if (tunerAnt1Params.clen > 31)
            tunerAnt1Params.clen = 31;
        tunerSetCap(&ant1Tuner, TUNER_CLEN, tunerAnt1Params.clen);
    }
    if (cmdBuffer.rxData[10] && ant1Tuner.coutSenFn)
    {
        tunerAnt1Params.cout = cmdBuffer.rxData[11];
        if (tunerAnt1Params.cout > 31)
            tunerAnt1Params.cout = 31;
        tunerSetCap(&ant1Tuner, TUNER_COUT, tunerAnt1Params.cout);
    }
    APPLOG("receive ant1 tuner parameters cin= %hhx, clen= %hhx cout=%hhx\n", tunerAnt1Params.cin, tunerAnt1Params.clen, tunerAnt1Params.cout);
#endif

    cmdBuffer.txData[0] = ERR_NONE;
    cmdBuffer.txData[1] = tunerParams.cin;
    cmdBuffer.txData[3] = tunerParams.clen;
    cmdBuffer.txData[5] = tunerParams.cout;
#if RADON
    cmdBuffer.txData[7] = tunerAnt1Params.cin;
    cmdBuffer.txData[9] = tunerAnt1Params.clen;
    cmdBuffer.txData[11]= tunerAnt1Params.cout;
    APPLOG("send ant1 tuner parameters cin= %hhx, clen= %hhx cout=%hhx\n", tunerAnt1Params.cin, tunerAnt1Params.clen, tunerAnt1Params.cout);
#endif
    cmdBuffer.txSize = CMD_ANTENNA_TUNER_REPLY_SIZE;
    APPLOG("send main tuner parameters cin= %hhx, clen= %hhx cout=%hhx\n", tunerParams.cin, tunerParams.clen, tunerParams.cout);
    APPLOGDUMP(cmdBuffer.txData, cmdBuffer.txSize);
    cmdBuffer.result = ERR_NONE;
#else
    cmdBuffer.txSize = CMD_ANTENNA_TUNER_REPLY_SIZE;
    cmdBuffer.result = ERR_REQUEST;
#endif
}

/** This function allows to trigger a auto tuning cycle. For in detail information
 * to antenna tuning see callAntennaTuner() and tuner.h.\n
 * The function is executed when a stream packet with
 * protocol = #CMD_AUTO_TUNER is received.
 * The format of the payload from the host is:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>auto_tune</td>
 *  </tr>
 * </table>
 * If the auto_tune value is not equal zero a auto tuner cycle will be started:
 * <ul>
 *   <li>0: no autotuning will be done.</li>
 *   <li>1: autotune procedure tunerOneHillClimb() will be performed.</li>
 *   <li>2: autotune procedure tunerMultiHillClimb() will be performed.</li>
 * </ul>
 * \n
 * The device does not send a reply back, but the result of the auto tuning
 * procedure can be read by callAntennaTuner(). \n
 * \note Depending on the used auto tune algorithm there will be some delay until
 * the tune algorithm finshed and the new tuned DTC values are avaiable. The tuner
 * algorithm 2 takes roughly 3 seconds, algorithm 1 is faster.
 */
void callAutoTuner(void)
{
    powerUpReader();
    autoTuner();
    powerDownReader();
}

void autoTuner(void)
{
#ifdef TUNER
    u8 algo = cmdBuffer.rxData[0] & 0xf;
    u8 tuner_id = cmdBuffer.rxData[0] >> 4;
    APPLOG("autoTuner\n");
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);
    TunerConfiguration *tuner = NULL;
    TunerParameters *params = NULL;

    if(0 == tuner_id)
    {
        tuner = &mainTuner;
        params = &tunerParams;
    }

    if (cmdBuffer.rxData[0])
    {
        APPLOG("tune antenna, algorithm: %hhx, id=%hhx\n", cmdBuffer.rxData[0],tuner_id);
    }

#if RADON
    if(1 == tuner_id)
    {
        tuner = &ant1Tuner;
        params = &tunerAnt1Params;
    }
#endif

    if (NULL != tuner)
    {
        switch (algo)
        {
            case (1): /* Simple: hill climb from current setting */
                as3993AntennaPower(1);
                tunerOneHillClimb(tuner, params, 100);
                as3993AntennaPower(0);
                break;
            case (2): /* advanced: hill climb from more points */
                as3993AntennaPower(1);
                tunerMultiHillClimb(tuner, params);
                as3993AntennaPower(0);
                break;
            case (3): /* try all combinations */
                as3993AntennaPower(1);
                tunerTraversal(tuner, params);
                as3993AntennaPower(0);
                break;
            default:
                break;
        }
        tunerSetTuning(tuner, params->cin, params->clen, params->cout);
    }

    cmdBuffer.txSize = CMD_AUTO_TUNER_REPLY_SIZE;
    APPLOG("send tune parameters cin= %hhx, clen= %hhx cout=%hhx\n", params->cin, params->clen, params->cout);
    APPLOGDUMP(cmdBuffer.txData, cmdBuffer.txSize);
    cmdBuffer.result = ERR_NONE;
#else
    APPLOG("autoTuner\n");
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);
    cmdBuffer.txSize = CMD_AUTO_TUNER_REPLY_SIZE;
    cmdBuffer.result = ERR_REQUEST;
#endif
}

#ifdef TUNER
/**
 * adds data in current USB buffer to tuning table, should be only called from callAntennaTuner().
 */
static u8 addToTuningTable(void)
{
    u8 idx;
    u16 iq1 = 0, iq2 = 0;
    u32 freq;
    freq = 0;
    freq += (long)cmdBuffer.rxData[1];
    freq += ((long)cmdBuffer.rxData[2]) << 8;
    freq += ((long)cmdBuffer.rxData[3]) << 16;

    idx = tuningTable.tableSize;
    if (idx > MAXTUNE)
        idx = MAXTUNE;
    tuningTable.freq[idx] = freq;
    tuningTable.tuneEnable[idx] = 0;
    if (cmdBuffer.rxData[4] > 0)
    {
        //set antenna 1
        tuningTable.tuneEnable[idx] += 1;
        tuningTable.cin[0][idx] = cmdBuffer.rxData[5];
        tuningTable.clen[0][idx] = cmdBuffer.rxData[6];
        tuningTable.cout[0][idx] = cmdBuffer.rxData[7];
        iq1 = (u16)cmdBuffer.rxData[8];
        iq1 += ((u16)cmdBuffer.rxData[9]) << 8;
        tuningTable.tunedIQ[0][idx] = iq1;
    }
    if (cmdBuffer.rxData[10] > 0)
    {
        //set antenna 2
        tuningTable.tuneEnable[idx] += 2;
        tuningTable.cin[1][idx] = cmdBuffer.rxData[11];
        tuningTable.clen[1][idx] = cmdBuffer.rxData[12];
        tuningTable.cout[1][idx] = cmdBuffer.rxData[13];
        iq2 = (u16)cmdBuffer.rxData[14];
        iq2 += ((u16)cmdBuffer.rxData[15]) << 8;
        tuningTable.tunedIQ[1][idx] = iq2;
    }
    if (tuningTable.tuneEnable[idx] > 0)    //if this tuning entry is used adjust size of table.
        tuningTable.tableSize++;

    APPLOG("add tunetable f=%x%x, tablesize=%hhx, tune1=%hhx cin1=%hhx clen1=%hhx cout1=%hhx iq1=%hx "
               "tune2=%hhx cin2=%hhx clen2=%hhx cout2=%hhx iq2=%hx\n", freq, tuningTable.tableSize,
               cmdBuffer.rxData[4], cmdBuffer.rxData[5], cmdBuffer.rxData[6], cmdBuffer.rxData[7], iq1,
               cmdBuffer.rxData[10], cmdBuffer.rxData[11], cmdBuffer.rxData[12], cmdBuffer.rxData[13], iq2);

    if (tuningTable.tableSize >= MAXTUNE)
    {
        tuningTable.tableSize = MAXTUNE;
        return MAXTUNE + 1;
    }
    else
        return tuningTable.tableSize;
}
#endif

/** This function allows to update the tuner table. For in detail information
 * to antenna tuning see callAntennaTuner() and tuner.h.\n
 * The function is executed when a stream packet with
 * protocol = #CMD_TUNER_TABLE is received.
 * The format of the payload received from the host is one of the following:
 * <ul>
 * <li>Get current tuning table size:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0x00 (SubCmd ID)</td>
 *  </tr>
 * </table>
 * The device sends back:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>1</th>
 *      <th>2</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0x00 (SubCmd ID)</td>
 *      <td>maximum tuning table size this device supports</td>
 *      <td>current tuning table size</td>
 *  </tr>
 * </table>
 * </li>
 *
 * <li>Delete current tuning table:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0x01 (SubCmd ID)</td>
 *  </tr>
 * </table>
 * The device sends back:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>1</th>
 *      <th>2</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0x01 (SubCmd ID)</td>
 *      <td>maximum tuning table size this device supports</td>
 *      <td>n/a</td>
 *  </tr>
 * </table>
 * </li>
 *
 * <li>Add new entry in tuning table:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>1..3</th>
 *      <th>4</th>
 *      <th>5</th>
 *      <th>6</th>
 *      <th>7</th>
 *      <th>8..9</th>
 *      <th>10</th>
 *      <th>11</th>
 *      <th>12</th>
 *      <th>13</th>
 *      <th>14..15</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0x02 (SubCmd ID)</td>
 *      <td>frequency</td>
 *      <td>Ant1: tune enable</td>
 *      <td>Ant1: cin</td>
 *      <td>Ant1: clen</td>
 *      <td>Ant1: cout</td>
 *      <td>Ant1: I+Q</td>
 *      <td>Ant2: tune enable</td>
 *      <td>Ant2: cin</td>
 *      <td>Ant2: clen</td>
 *      <td>Ant2: cout</td>
 *      <td>Ant2: I+Q</td>
 *  </tr>
 * </table>
 * Adds the data to the internal tuning table. The first set of data is for
 * antenna 1 the second set for antenna 2. \n
 * The device responds:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>1</th>
 *      <th>2</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0x02 (SubCmd ID)</td>
 *      <td>remaining size in tuningTable</td>
 *      <td>n/a</td>
 *  </tr>
 * </table>
 * If no more space is left in tuningTable the status will be set to ERR_NOMEM. If adding
 * to tuningTable was successful status will be set to ERR_NONE.
 * </li>
 * 
 * <li> If the device does not support antenna tuning or if a unsupported SubCmd ID
 * was received the device sends back:
 *  <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>1</th>
 *      <th>2</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0xFF</td>
 *      <td>0x00</td>
 *      <td>n/a</td>
 *  </tr>
 * </table>
 * The reply status will be set to ERR_REQUEST.
 * </li>
 *</ul>
 */
void callTunerTable(void)
{
    tunerTable();
}

void tunerTable(void)
{
    APPLOG("tunerTable\n");
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);
    memset(cmdBuffer.txData, 0, CMD_TUNER_TABLE_REPLY_SIZE);

#ifdef TUNER
    cmdBuffer.result = ERR_NONE;
    switch (cmdBuffer.rxData[0])
    {
    case 0x00:
        /*retrieve tuning table size */
        cmdBuffer.txData[0] = 0x00;    //subcmd
        cmdBuffer.txData[1] = MAXTUNE;
        cmdBuffer.txData[2] = tuningTable.tableSize;
        break;
    case 0x01:
        /*delete tuning table */
        APPLOG("delete tuning table tablesize= %hhx\n", tuningTable.tableSize);
        tuningTable.currentEntry = 0;
        tuningTable.tableSize = 0;
        cmdBuffer.txData[0] = 0x01;    //subcmd
        cmdBuffer.txData[1] = MAXTUNE;
        break;
    case 0x02:  /* add new entry in tuning table */
        if (cmdBuffer.rxSize < 16)
            cmdBuffer.result = ERR_PARAM;
        else
            if (addToTuningTable() > MAXTUNE)
                cmdBuffer.result = ERR_NOMEM;    //maximum of tuning table reached
        cmdBuffer.txData[0] = 0x02;    //subcmd
        cmdBuffer.txData[1] = MAXTUNE - tuningTable.tableSize;
        break;
    default:
        cmdBuffer.txData[0] = 0xFF; //subcmd
        cmdBuffer.txData[1] = MAXTUNE;
        cmdBuffer.result = ERR_REQUEST;
        break;
    }
    cmdBuffer.txSize = CMD_TUNER_TABLE_REPLY_SIZE;
    APPLOGDUMP(cmdBuffer.txData, cmdBuffer.txSize);
#else
    cmdBuffer.txSize = CMD_ANTENNA_TUNER_REPLY_SIZE;
    cmdBuffer.result = ERR_REQUEST;
    cmdBuffer.txData[0] = 0xFF; //subcmd
    cmdBuffer.txData[1] = 0;
#endif

}


/*!This function gets/sets the current reader config and is executed
 * when a stream packet with protocol = #CMD_READER_CONFIG is received.
 * The reader config contains the power down mode which can be changed and
 * various non-changeable parameters like:
 * error codes, FW compile time switches, ...\n
 * The format of the payload from the host is:
 * <table>
 *   <tr>
 *       <th>Byte</th>
 *       <th>0</th>
 *       <th>1</th>
 *   </tr>
 *   <tr>
 *       <th>Content</th>
 *       <td>set_powermode</td>
 *       <td>powermode</td>
 *   </tr>
 * </table>
 * The values are only being set if the proper set_X value is set to 1.\n
 * The powermode value will be applied to #readerPowerDownMode variable and used in powerDownReader() and powerUpReader().\n
 * The device reply payload is:
 * <table>
 *   <tr><th>   Byte</th><th>       0   </th><th>   1       </th><th> 2 </th><th>3 </th><th>  4  </th><th>    5    </th><th>  6  </th><th>      7      </th><th>    8      </th></tr>
 *   <tr><th>Content</th><td>WrongChipId</td><td>InitErrCode</td><td>VCO</td><td>PA</td><td>INPUT</td><td>AntSwitch</td><td>Tuner</td><td>powerDownMode</td><td>Hardware ID</td></tr>
 * </table>
 * The values for the parameters are:
 * <table>
 *   <tr><th>Parameter</th><th>Values</th></tr>
 *   <tr><td>WrongChipId</td><td>   0-99: No Error. FW and reader chip match. The value is the last to digits of the reader chip
 *                                      eg: for AS3993 the value is 93.\n
 *                                  0xFF: FW is not compatible with the reader chip on this board.</td></tr>
 *   <tr><td>Init Error</td><td>    0: No Error. Init of reader chip was successful.\n
 *                                  1: Writing and reading SPI failed.\n
 *                                  2: Reset via EN low + high failed.\n
 *                                  3: IRQ line failed.\n
 *                                  4: Crystal not stable (Bit0 in AGC and Internal Status register 0x2A).\n
 *                                  5: PLL not locked (Bit1 in AGC and Internal Status register 0x2A)</td></tr>
 *   <tr><td>VCO</td><td>           0: FW was compiled for internal VCO (INTVCO).\n
 *                                  1: FW was compiled for external VCO (EXTVCO).\n</td></tr>
 *   <tr><td>PA</td><td>            0: FW was compiled for internal PA (INTPA).\n
 *                                  1: FW was compiled for external PA (EXTPA).\n</td></tr>
 *   <tr><td>Input</td><td>         0: FW was compiled for balanced input (BALANCEDINP).\n
 *                                  1: FW was compiled for single input (SINGLEINP).\n</td></tr>
 *   <tr><td>AntSwitch</td><td>     0: FW was not compiled for 2 antenna ports.\n
 *                                  1: FW was compiled to use 2 antenna ports (ANTENNA_SWITCH).\n</td></tr>
 *   <tr><td>Tuner</td><td>         Or'ed tuner configuration, every bit repesents a tunable cap (DTC).\n
 *                                  0: FW does not support tuning.\n
 *                                  0x01: Cin tuner is used (TUNER_CIN).\n
 *                                  0x02: Clen tuner is used (TUNER_CLEN).\n
 *                                  0x04: Cout tuner is used (TUNER_COUT).\n
 *                                  other bits: not yet supported.</td></tr>
 *   <tr><td>powerDownMode</td><td> Currently configured power down mode of reader.\n
 *                                  Available modes are: #POWER_DOWN, #POWER_NORMAL,
 *                                  #POWER_NORMAL_RF and #POWER_STANDBY.</td></tr>
 *   <tr><td>Hardware ID</td><td>   Identification number for the board (HARDWARE_ID_NUM)\n
 *                                  Available IDs are defined in as3993_config.h</td></tr>
 * </table>
 * If any of the parameters is set to 0xFF an unidentified error/configuration occured.
 */
void callReaderConfig(void)
{
    readerConfig();
}

void readerConfig(void)
{
    u8 result = ERR_NONE;
    APPLOG("readerConfig\n");
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);

    if (cmdBuffer.rxData[0])    // change power down configuration
    {
        if (cmdBuffer.rxData[1] > POWER_STANDBY)    //invalid power down configuration
        {
            result = ERR_PARAM;
        }
        else
        {   // before changing configuration we have to power up
            powerUpReader();
            readerPowerDownMode = cmdBuffer.rxData[1];
            powerDownReader();
        }
    }

    memset(cmdBuffer.txData, 0xFF, CMD_READER_CONFIG_REPLY_SIZE);
#if RUN_ON_AS3993
    cmdBuffer.txData[0] = 93;
#elif RUN_ON_AS3994
    cmdBuffer.txData[0] = 94;
#elif RUN_ON_AS3980
    cmdBuffer.txData[0] = 80;
#else
    cmdBuffer.txData[0] = 0xFF;        //unsupported
#endif

    cmdBuffer.txData[1] = readerInitStatus;

#ifdef INTVCO
    cmdBuffer.txData[2] = 0;
#endif
#ifdef  EXTVCO
    cmdBuffer.txData[2] = 1;
#endif

#ifdef INTPA
    cmdBuffer.txData[3] = 0;
#endif
#ifdef EXTPA
    cmdBuffer.txData[3] = 1;
#endif

#ifdef BALANCEDINP
    cmdBuffer.txData[4] = 0;
#endif
#ifdef SINGLEINP
    cmdBuffer.txData[4] = 1;
#endif

#ifdef ANTENNA_SWITCH
    cmdBuffer.txData[5] = 1;
#else
    cmdBuffer.txData[5] = 0;
#endif

#ifdef TUNER
    cmdBuffer.txData[6] = 
        ((mainTuner.cinSenFn==0)?0:TUNER_CIN)|  
        ((mainTuner.clenSenFn==0)?0:TUNER_CLEN)|  
        ((mainTuner.coutSenFn==0)?0:TUNER_COUT)|  
        0;
#else
    cmdBuffer.txData[6] = 0;
#endif
    cmdBuffer.txData[7] = readerPowerDownMode;
    cmdBuffer.txData[8] = HARDWARE_ID_NUM;

#ifdef TUNER
#if RADON
    cmdBuffer.txData[9] = 
        ((ant1Tuner.cinSenFn==0)?0:TUNER_CIN)|  
        ((ant1Tuner.clenSenFn==0)?0:TUNER_CLEN)|  
        ((ant1Tuner.coutSenFn==0)?0:TUNER_COUT)|  
        (2<<4)| /* Disabled for antenna 2 */
        0;
#else
    cmdBuffer.txData[9] = 0;
#endif
#endif

    //if (cmdBuffer.txSize > CMD_READER_CONFIG_REPLY_SIZE)
    cmdBuffer.txSize = CMD_READER_CONFIG_REPLY_SIZE;
    cmdBuffer.result = result;
}

/**
 * writes to AS3993 register at address addr the value and prepares tx answer to host.\n
 * see cmdWriteReg() for reply structure.
 * @param addr register address
 * @param value the value to write
 * @param txSize expected tx size
 * @param txData buffer for reply
 * @return error code
 */
u8 writeRegister(u8 addr, u8 value, u16 * txSize, u8 * txData)
{
    APPLOG("WRITE\n");
    powerUpReader();
    if (addr < 0x80)
    {
        as3993SingleWrite(addr, value);
    }
    else
    {
        as3993SingleCommand(addr);
    }
    txData[0] = 0;
    *txSize = WRITE_REG_REPLY_SIZE;
    /* do not power down reader after writing register, as user probably wants to
     * see the effect of his register write. */
    //powerDownReader();
    return ERR_NONE;
}

/**
 * reads one AS3993 register at address and puts the value into the reply to the host.\n
 * see cmdReadReg() for reply structure.
 * @param addr register address
 * @param txSize expected tx size
 * @param txData buffer for reply
 * @return error code
 */
u8 readRegister(u8 addr, u16 * txSize, u8 * txData)
{
    APPLOG("READ\n");
    powerUpReader();
    txData[0] = as3993SingleRead(addr);

    *txSize = READ_REG_REPLY_SIZE;
    powerDownReader();
    return ERR_NONE;
}

/*! This function sets and reads various RF Tx/Rx related settings and is
  executed when a stream packet with protocol = #CMD_CONFIG_TX_RX is received.
  \note Not every parameters is available on every board, eg: switching antenna boards
  is only available on boards with 2 antenna ports. The FW will silently ignore
  not applicable parameters.

  The format of the payload from the host is:
  <table>
    <tr>
        <th>Byte</th>
        <th>0</th>
        <th>1</th>
        <th>2</th>
        <th>3</th>
    </tr>
    <tr>
        <th>Content</th>
        <td>set_sensitivity</td>
        <td>sensitivity</td>
        <td>set_antenna</td>
        <td>antenna id</td>
    </tr>
  </table>
  The values are only being set if the proper set_X value is set to 1.<br>
  The device sends back:
  <table>
    <tr>
        <th>Byte</th>
        <th>0</th>
        <th>1</th>
        <th>2</th>
        <th>3</th>
    </tr>
    <tr>
        <th>Content</th>
        <td>reserved(0)</td>
        <td>sensitivity</td>
        <td>reserved (0)</td>
        <td>antenna id</td>
    </tr>
  </table>
  Values for the different parameters are:
  <table>
    <tr><th>Name</th></th><th>values</th></tr>
    <tr><td>sensitivity</td><td>-128 .. 127 (dBm)
                       </td></tr>
    <tr><td>antenna id</td><td> 1: antenna port 1<br>
                                2: antenna port 2
                       </td></tr>
 * </table>
 */
void callConfigTxRx(void)
{
    powerUpReader();
    configTxRx();
    powerDownReader();
}

void configTxRx()
{
    APPLOG("configTxParams\n");
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);

    if (cmdBuffer.rxData[0]) as3993SetSensitivity( cmdBuffer.rxData[1] );
#ifdef ANTENNA_SWITCH
    if (cmdBuffer.rxData[2])
    {
        usedAntenna = cmdBuffer.rxData[3];
        if (usedAntenna > 2)        //prevent mis-configuration of antenna switch.
            usedAntenna = 2;
        else if (usedAntenna < 1)
            usedAntenna = 1;
        SWITCH_ANTENNA(usedAntenna);
    }
#endif

    cmdBuffer.txSize = CMD_CONFIG_TX_RX_REPLY_SIZE;

    cmdBuffer.txData[1] = (u8) as3993GetSensitivity();
#ifdef ANTENNA_SWITCH
    cmdBuffer.txData[3] = usedAntenna;
#endif

    cmdBuffer.result = ERR_NONE;

}

void configGen2()
{
    APPLOG("configGen2, len: %hx\n", cmdBuffer.rxSize);
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);
    if (currentSession == SESSION_GEN2) currentSession = 0;

    if (cmdBuffer.rxData[0])  gen2Configuration.linkFreq = cmdBuffer.rxData[1];
    if (cmdBuffer.rxData[2])  gen2Configuration.miller   = cmdBuffer.rxData[3];
    if (cmdBuffer.rxData[4])  gen2Configuration.session  = cmdBuffer.rxData[5];
    if (cmdBuffer.rxData[6])  gen2Configuration.trext    = cmdBuffer.rxData[7];
    if (cmdBuffer.rxData[8])  gen2Configuration.tari     = cmdBuffer.rxData[9];
    if (cmdBuffer.rxData[10]) gen2qbegin                 = cmdBuffer.rxData[11];
    if (cmdBuffer.rxData[12]) gen2Configuration.sel      = cmdBuffer.rxData[13];
    if (cmdBuffer.rxData[14]) gen2Configuration.target   = cmdBuffer.rxData[15];
/*
    powerUpReader();
    gen2Configure(&gen2Configuration);
    powerDownReader();
*/
    memset(cmdBuffer.txData, 0, CMD_GEN2_SETTINGS_REPLY_SIZE);
    cmdBuffer.txSize = CMD_GEN2_SETTINGS_REPLY_SIZE;
    cmdBuffer.result = ERR_NONE;

    cmdBuffer.txData[1] = gen2Configuration.linkFreq;
    cmdBuffer.txData[3] = gen2Configuration.miller;
    cmdBuffer.txData[5] = gen2Configuration.session;
    cmdBuffer.txData[7] = gen2Configuration.trext;
    cmdBuffer.txData[9] = gen2Configuration.tari;
    cmdBuffer.txData[11] = gen2qbegin;
    cmdBuffer.txData[13] = gen2Configuration.sel;
    cmdBuffer.txData[15] = gen2Configuration.target;
    APPLOGDUMP(cmdBuffer.txData, cmdBuffer.txSize);
}

/*! This function sets and reads various gen2 related settings and is
 executed when a stream packet with protocol = #CMD_CONFIG_TX_RX is received.
 Most of the parameters are related to the QUERY commmand of the Gen2
 protocol specification.\n
 The format of the payload from the host is:
  <table>
    <tr>
        <th>Byte</th>
        <th>0</th>
        <th>1</th>
        <th>2</th>
        <th>3</th>
        <th>4</th>
        <th>5</th>
        <th>6</th>
        <th>7</th>
        <th>8</th>
        <th>9</th>
        <th>10</th>
        <th>11</th>
        <th>12</th>
        <th>13</th>
        <th>14</th>
        <th>15</th>
    </tr>
    <tr>
        <th>Content</th>
        <td>set_lf</td>
        <td>lf</td>
        <td>set_coding</td>
        <td>coding</td>
        <td>set_session</td>
        <td>session</td>
        <td>set_trext</td>
        <td>trext</td>
        <td>set_tari</td>
        <td>tari</td>
        <td>set_qbegin</td>
        <td>qbegin</td>
        <td>set_sel</td>
        <td>sel</td>
        <td>set_target</td>
        <td>target</td>
    </tr>
  </table>
  The values are only being set if the proper set_X value is set to 1.<br>
  The device sends back:
  <table>
    <tr>
        <th>Byte</th>
        <th>0</th>
        <th>1</th>
        <th>2</th>
        <th>3</th>
        <th>4</th>
        <th>5</th>
        <th>6</th>
        <th>7</th>
        <th>8</th>
        <th>9</th>
        <th>10</th>
        <th>11</th>
        <th>12</th>
        <th>13</th>
        <th>14</th>
        <th>15</th>
    </tr>
    <tr>
        <th>Content</th>
        <td>reserved(0)</td>
        <td>lf</td>
        <td>reserved(0)</td>
        <td>coding</td>
        <td>reserved(0)</td>
        <td>session</td>
        <td>reserved(0)</td>
        <td>trext</td>
        <td>reserved(0)</td>
        <td>tari</td>
        <td>reserved(0)</td>
        <td>qbegin</td>
        <td>reserved(0)</td>
        <td>sel</td>
        <td>reserved(0)</td>
        <td>target</td>
    </tr>
  </table>
  Values for the different parameters are:
  <table>
    <tr><th>Name</th></th><th>values</th></tr>
    <tr><td>lf</td><td> 0 = 40 kHz,<br>
                        3 = 80 kHz not AS3993,<br>
                        6 = 160 kHz,<br>
                        8 = 213 kHz,<br>
                        9 = 256 kHz,<br>
                       12 = 320 kHz,<br>
                       15 = 640 kHz
                       </td></tr>
    <tr><td>coding</td><td>0 = FM0,<br>
                           1 = Miller2,<br>
                           2 = Miller4,<br>
                           3 = Miller8
                       </td></tr>
    <tr><td>session</td><td>0 = S0,<br>
                            1 = S1,<br>
                            2 = S2,<br>
                            3 = S3
                       </td></tr>
    <tr><td>trext</td><td>0 = short preamble, no pilot tone,<br>
                          1 = long preamble, pilot tone
                       </td></tr>
    <tr><td>tari</td><td>0 = 6.25 us,<br>
                         1 = 12.5 us,<br>
                         2 = 25 us
                       </td></tr>
    <tr><td>qbegin</td><td>0 .. 15. Initial gen2 round is 2^qbegin long. Please be careful with higher values.
                       </td></tr>
    <tr><td>sel</td><td>0 .. 1 = All,<br>
                        2 = ~SL,<br>
                        3 = SL
                       </td></tr>
    <tr><td>target</td><td>0 = A,<br>
                           1 = B
                       </td></tr>
  </table>
 */
void callConfigGen2(void)
{
    configGen2();
}

/**
 * reads all AS3993 registers and puts the values into the reply to the host.\n
 * see cmdReadReg() for reply structure.
 * @param txSize expected tx size
 * @param txData buffer for reply
 * @return error code
 */
u8 readRegisters(u16 * txSize, u8 * txData)
{
    u8 i;
    u8 idx = 0;

    APPLOG("READ registers complete\n");
    powerUpReader();

    for ( i = 0; i < 0x3f; i++)
    {
        /* register data is transmitted without gaps like in register layout of AS3993. */
        if (!(     (i == 0x0F)
                || (i > 0x1D && i < 0x22)
                || (i > 0x22 && i < 0x29)
                || (i > 0x2E && i < 0x33)
                || (i == 0x34)))
        {
            txData[idx] = as3993SingleRead(i);
            idx++;
        }
    }
    *txSize = idx + 1;       // data + command byte
    APPLOGDUMP(txData, *txSize);
    powerDownReader();
    return ERR_NONE;
}


static void initTagInfo()
{
    u32 i;
    u8 *ptr;

    ptr = (u8*)tags_;

    for (i = 0; i <  sizeof(tags_) ; i ++)
    {
        *ptr++ = 0;
    }
}

/**  
  This function performs one inventory round using ISO18000-6b protocol.
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>       0</th><th>1..8</th><th>  9</th></tr>
    <tr><th>Content</th><td>address</tr><td>word_data </td><td>mask</td></tr>
  </table>
where 
<ul>
<li>address: address where data will be compared</li> 
<li>word_data: data which will be compared</li> 
<li>mask: the mask value for GROUP_SELECT_EQ command, 0 will select all tags</li>
</ul>
  The device sends back:
  <table>
    <tr><th>   Byte</th><th>0             </th><th>1..8           </th><th>9..16</th><th>...</th></tr>
    <tr><th>Content</th><td>number of tags</td><td>ID of first tag</td><td>ID of second tag</td><td>...</td></tr>
  </table>
 */
void callInventory6B(void)
{
#if ISO6B
    int i = 0;
    unsigned char word_data[8];

    memcpy(&word_data, &cmdBuffer.rxData[i+1], 8);

    APPLOG("call 6b Inventory\n");

    powerUpReader();

    checkAndSetSession(SESSION_ISO6B);
    as3993AntennaPower(1);

    num_of_tags = iso6bInventoryRound (tags_, MAXTAG, cmdBuffer.rxData[0], &word_data[0], cmdBuffer.rxData[9]);

    powerDownReader();
#if RADON
    delay_ms(100);
#endif

    cmdBuffer.txData[0] = num_of_tags;

    for(i = 1; i <= num_of_tags;i++)
    {
        memcpy(&cmdBuffer.txData[1 + (i-1)*8],tags_[i-1].epc,8); // uid length is always 4 in iso6b
    }
    cmdBuffer.result = ERR_NONE;
    cmdBuffer.txSize = 1+ num_of_tags * 8;

    APPLOGDUMP(&cmdBuffer.txData[0], cmdBuffer.txSize);
#else
    cmdBuffer.txData[0] = 0; /* no tags */
    cmdBuffer.result = ERR_REQUEST;
    cmdBuffer.txSize = 1;
#endif
}

/**
  This function reads from a tag using ISO18000-6b protocol command READ.
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>0..7</th><th>8      </th></tr>
    <tr><th>Content</th><td>ID  </td><td>address</td></tr>
  </table>

  The device sends back:
  <table>
    <tr><th>   Byte</th><th>0..7     </th></tr>
    <tr><th>Content</th><td>word_data</td></tr>
  </table>
 */
void callReadFromTag6B(void)
{
#if ISO6B
    s8 result;
    unsigned char word_data[8];
    unsigned char uid[8];
    memcpy(&uid, &cmdBuffer.rxData[0], 8);

    APPLOG("call 6b Read \n");

    powerUpReader();

    checkAndSetSession(SESSION_ISO6B);
    as3993AntennaPower(1);
    delay_ms(1);
    result = iso6bRead(&uid[0],cmdBuffer.rxData[8], &word_data[0]);

    as3993SingleWrite(AS3993_REG_STATUSCTRL, 0x00);
    powerDownReader();
#if RADON
    delay_ms(100);
#endif
    cmdBuffer.result = result;
    cmdBuffer.txSize = CMD_READ_FROM_TAG_6B_REPLY_SIZE;
    memcpy(&cmdBuffer.txData[0], &word_data[0], cmdBuffer.txSize);
#else
    cmdBuffer.result   = ERR_REQUEST;
    cmdBuffer.txSize = CMD_READ_FROM_TAG_6B_REPLY_SIZE;
#endif
}

/** 
  This function writes to a tag using ISO18000-6b protocol command WRITE.
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>0..7         </th><th>8      </th><th>9        </th></tr>
    <tr><th>Content</th><td>ID of the tag</td><td>address</td><td>byte_data</td></tr>
  </table>
  The device sends back:
  <table>
    <tr><th>   Byte</th><th>       0</th></tr>
    <tr><th>Content</th><td>acknowledge</td></tr>
  </table>
 */
void callWriteToTag6B(void)
{
#if ISO6B
    s8 result;
    unsigned char response[1];
    unsigned char uid[8];
    memcpy(&uid, &cmdBuffer.rxData[0], 8);

    APPLOG("call 6b Write \n");

    powerUpReader();
    checkAndSetSession(SESSION_ISO6B);
    as3993AntennaPower(1);
    delay_ms(1);
    result = iso6bWrite(&uid[0],cmdBuffer.rxData[8],cmdBuffer.rxData[9], &response[0]);

    as3993SingleWrite(AS3993_REG_STATUSCTRL, 0x00);

    powerDownReader();
#if RADON
    delay_ms(100);
#endif
    cmdBuffer.result = result;
    cmdBuffer.txSize = 1;
    memcpy(&cmdBuffer.txData[0], &response[0], cmdBuffer.txSize);

    APPLOGDUMP(&cmdBuffer.txData[0], cmdBuffer.txSize);
#else
    cmdBuffer.result = ERR_REQUEST;
    cmdBuffer.txSize = CMD_WRITE_TO_TAG_6B_REPLY_SIZE;
#endif
}

/**
 * This function is called by callInventoryGen2() and doCyclicInventory() and performs
 * the actual Gen2 inventory round.
 */
void callInventoryGen2Internal()
{
    u8 foundTags;
    if (tagDataAvailable)       // wait until all tag data has been sent before starting next inventory round
    {
        return;
    }
    foundTags = inventoryGen2();
    tagDataAvailable = 1;
    APPLOG("inventory Gen2, found tags: %hhx\n", foundTags);
}

/** This function performs a single gen2 protocol inventory round according to
 parameters configured by callConfigGen2() and callSelectTag().\n
 The function is executed when a stream packet with protocol = #CMD_INVENTORY_GEN2 is received.\n
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>     0    </th><th>   1                 </th><th>   2   </th></tr>
    <tr><th>Content</th><td>  autoAck </td><td> tid(2LSB)+fast(LSB) </td><td> rssi  </td></tr>
  </table>
  Where autoAck defines if the autoACK mode of the reader is used. If autoAck = 0
  autoACK will not be used, otherwise autoACK will be used.
  The parameter tid defines if a read of the TID Memory will be performed in inventory rounds.
  If tid = 1 the TID memory will be read, otherwise no read will be done.
  Parameter fast defines how the inventory rounds are performed. If fast = 0 the inventory
  round is executed until the tag is in the Open state. If fast = 1 the inventory round
  is stopped when the tag reaches Acknowledged state (no Req_RN command is sent).
  See Gen2 protocol specification page 47: Tag state diagram for details.\n
  The rssi parameter defines which rssi value should be measured when getting a tag.
  The value of rssi parameter will be written to register 0x29. \n
  The device sends back all the tags using the following report:
  <table>
    <tr>
      <th>  Byte</th>
      <th>     0</th>
      <th>     1</th>
      <th>     2</th>
      <th>     3</th>
      <th>     4</th>
      <th>5 .. 7</th>
      <th>     8</th>
      <th>     9</th>
      <th>    10</th>
      <th>11 .. 11+epclen</th>
      <th>12+epclen</th>
      <th>13+epclen .. 15+epclen</th>
      <th>...</th>
    </tr>
    <tr>
      <th>Content</th>
      <td>cyclic</td>
      <td>tags_left</td>
      <td>tags_in_reply</td>
      <td>AGC and status (reg 0x2A)</td></tr>
      <td>RSSI_value (reg 0x2B)</td>
      <td>base_freq</td>
      <td>epclen+pclen</tr>
      <td>pc[0]</td>
      <td>pc[1]</td>
      <td>      epc      </td>
      <td>next tag: RSSI</td>
      <td>next tag: base_freq</td>
      <td>...</td>
    </tr>
  </table>

<ul>
<li>cyclic: is set to 1 if cyclic(continuous) inventory rounds are performed, otherwise 0.</li>
<li>tags_left: number of tags still to be sent after this reply. The next tag data will be sent in next iteration of sendTagData(). </li>
<li>tags_in_reply: number of tags in reply. </li>
<li> agc and status: subc_phase, agc status, i/q channel (content of register 0x2A) </li>
<li>RSSI_value: upper 4 bits I channel, lower 4 bits Q channel (content of register 0x2B) </li>
<li>base_freq: base frequency at which the tag was found. </li>
<li>epclen+pclen: added length of pc and epc.</li>
</ul>
 Sending the reply is implemented in function sendTagData(), which has to be called
 * repeatedly until all tag data has been sent.
 */
void callInventoryGen2(void)
{
    autoAckMode = cmdBuffer.rxData[0];
    fastInventory = cmdBuffer.rxData[1] & 0x01;
    read_TID_CAL_inInventoryRound = (cmdBuffer.rxData[1] & 0x02) >> 1;
    read_MMS_VFC_TEMP_inInventoryRound = (cmdBuffer.rxData[1] & 0x04) >> 2;
    rssiMode = cmdBuffer.rxData[2];
    callInventoryGen2Internal();
    cmdBuffer.result = ERR_NONE;
}

u8 getTagData( u16 *txSize, u8 *txData )
{
    unsigned char element = 0;
    *txSize = 0;
    if (!tagDataAvailable)
    {
        return ERR_REQUEST;
    }
    
    if (num_of_tags == 0)
    {
        txData[0] = inventoryResult;
        txData[1] = read_MMS_VFC_TEMP_inInventoryRound << 2 | read_TID_CAL_inInventoryRound << 1; //inventory type
        txData[2] = num_of_tags;
        txData[3] = 0;
        txData[4] = Frequencies.freq[currentFreqIdx] & 0xff;
        txData[5] = (Frequencies.freq[currentFreqIdx] >>  8) & 0xff;
        txData[6] = (Frequencies.freq[currentFreqIdx] >> 16) & 0xff;
        *txSize = SENDNOTAGINFO;
        tagDataAvailable = 0;
        return ERR_NONE;
    }
    
    txData[0] = inventoryResult;  // inventory result
    txData[1] = read_MMS_VFC_TEMP_inInventoryRound << 2 | read_TID_CAL_inInventoryRound << 1; //inventory type
    txData[2] = num_of_tags;
    
    *txSize = 3;
    
    while(num_of_tags > 0)
    {
        txData[(*txSize)++] = tags_[element].agc;
        txData[(*txSize)++] = tags_[element].rssi;
        txData[(*txSize)++] = Frequencies.freq[currentFreqIdx] & 0xff;
        txData[(*txSize)++] = (Frequencies.freq[currentFreqIdx] >>  8) & 0xff;
        txData[(*txSize)++] = (Frequencies.freq[currentFreqIdx] >> 16) & 0xff;
        txData[(*txSize)++] = tags_[element].epclen + 2;
        txData[(*txSize)++] = tags_[element].pc[0];
        txData[(*txSize)++] = tags_[element].pc[1];
        memcpy(&txData[*txSize], tags_[element].epc, tags_[element].epclen);

        *txSize += tags_[element].epclen;

        if(read_TID_CAL_inInventoryRound)
        {
            txData[(*txSize)++] = tags_[element].tidlength;
            memcpy(&txData[*txSize], tags_[element].tid, tags_[element].tidlength);
            *txSize += tags_[element].tidlength;
            txData[(*txSize)++] = tags_[element].cal[0];
            txData[(*txSize)++] = tags_[element].cal[1];
            txData[(*txSize)++] = tags_[element].cal[2];
            txData[(*txSize)++] = tags_[element].cal[3];
            txData[(*txSize)++] = tags_[element].cal[4];
            txData[(*txSize)++] = tags_[element].cal[5];
            txData[(*txSize)++] = tags_[element].cal[6];
            txData[(*txSize)++] = tags_[element].cal[7];
        }

        if(read_MMS_VFC_TEMP_inInventoryRound)
        {
            txData[(*txSize)++] = tags_[element].mms[0];
            txData[(*txSize)++] = tags_[element].mms[1];
            txData[(*txSize)++] = tags_[element].vfc[0];
            txData[(*txSize)++] = tags_[element].vfc[1];
            txData[(*txSize)++] = tags_[element].temp[0];
            txData[(*txSize)++] = tags_[element].temp[1];
        }
        num_of_tags--;
        element++;
    }
    
    tagDataAvailable = 0;
    
    return ERR_NONE;
}

void callGetTagData()
{
    cmdBuffer.result = getTagData( &cmdBuffer.txSize, cmdBuffer.txData );
}

u8 inventoryGen2(void)
{
    s8 result;
    void (*followTagCommand)(Tag *tag, s8 *readErr) = NULL;
    num_of_tags = 0;
    APPLOG("INVENTORY Gen2 , autoAck: %hhx, fast: %hhx, rssi: %hhx\n", autoAckMode, fastInventory, rssiMode);
    result = hopFrequencies();
    if( !result )
    {
        checkAndSetSession(SESSION_GEN2);
        as3993SingleWrite(AS3993_REG_STATUSPAGE, rssiMode);
        if (rssiMode == RSSI_MODE_PEAK)      //if we use peak rssi mode, we have to send anti collision commands
            as3993SingleCommand(AS3993_CMD_ANTI_COLL_ON);

       performSelects();
       if(special_select_performed == 0)
       {
            if (read_TID_CAL_inInventoryRound || read_MMS_VFC_TEMP_inInventoryRound)
            {
                followTagCommand = gen2FollowRead;
            }

            if( !autoAckMode )
                num_of_tags = gen2SearchForTags(tags_, MAXTAG, gen2qbegin, continueCheckTimeout, fastInventory?0:1, 1, followTagCommand);
            else
                num_of_tags = gen2SearchForTagsAutoAck(tags_, MAXTAG, gen2qbegin, continueCheckTimeout, fastInventory?0:1, 1, followTagCommand);

            if (rssiMode == RSSI_MODE_PEAK)      //if we use peak rssi mode, we have to send anti collision commands
                as3993SingleCommand(AS3993_CMD_ANTI_COLL_OFF);
       }
       else
       {
            special_select_performed = 0;  
       }
    }
    inventoryResult = result;
    hopChannelRelease();
#if RADON
    //delay_ms(100);
    //delay_ms(25);
#endif

    APPLOG("end inventory, found tags: %hhx\n", num_of_tags);
    return num_of_tags;
}

/** This function singulates a Gen2 tag using the given mask for subsequent
 * operations like inventory/read/write. Several (#MAX_SELECTS) Select commands can be
 * configured, which are executed sequentially, allowing union and intersection
 * based tag partitioning. See Gen2 protocol specification for further details. \n
 * This function is executed when a stream packet with
 * protocol = #CMD_SELECT_TAG is received. \n
 * \note This command will reply with ERR_REQUEST on AS3980 as Select command
 * is not supported by AS3980.
 * 
 * The format of the payload received from the host is one of the following:
 * <ul>
 * <li>Clear list of Select commands (no Select commands is executed):
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>...</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0x00 (SubCmd ID)</td>
 *      <td>ignored</td>
 *  </tr>
 * </table>
 * The device sends back status ERR_NONE and no payload.
 * </li>
 *
 * <li>Add to list of Select commands, execute the Select commands and try to
 * singulate a tag:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>1</th>
 *      <th>2</th>
 *      <th>3</th>
 *      <th>4 .. 5</th>
 *      <th>6</th>
 *      <th>7</th>
 *      <th>8 .. 8 + mask_len</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0x01 (SubCmd ID)</td>
 *      <td>target</td>
 *      <td>action</td>
 *      <td>mem_bank</td>
 *      <td>address</td>
 *      <td>mask_len</td>
 *      <td>truncate</td>
 *      <td>mask</td>
 *  </tr>
 * </table>
 * The device sends back a status (no payload). ERR_MEM if no more select command can be selected
 * or a specific Gen2 error if singulating tag failed.
 * </li>
 *
 * <li>Clear list of Select commands, add to list of Select commands,
 * execute the Select commands and try to singulate a tag:
 * <table>
 *  <tr>
 *      <th>Byte</th>
 *      <th>0</th>
 *      <th>1</th>
 *      <th>2</th>
 *      <th>3</th>
 *      <th>4 .. 5</th>
 *      <th>6</th>
 *      <th>7</th>
 *      <th>8 .. 8 + mask_len</th>
 *  </tr>
 *  <tr>
 *      <th>Content</th>
 *      <td>0x02 (SubCmd ID)</td>
 *      <td>target</td>
 *      <td>action</td>
 *      <td>mem_bank</td>
 *      <td>address</td>
 *      <td>mask_len</td>
 *      <td>truncate</td>
 *      <td>mask</td>
 *  </tr>
 * </table>
 * The device sends back a status (no payload). ERR_MEM if no more select command can be selected
 * or a specific Gen2 error if singulating tag failed.
 * </li>
 * </ul>
 *
 * Values for the different parameters are:
 * <table>
 *   <tr><th>Name</th></th><th>values</th></tr>
 *   <tr><td>target</td><td> 0 = Inventoried (S0),<br>
 *                       1 = Inventoried (S1),<br>
 *                       2 = Inventoried (S2),<br>
 *                       3 = Inventoried (S3),<br>
 *                       4 = SL,<br>
 *                       >4 = RFU
 *                      </td></tr>
 *   <tr><td>action</td><td>Action tag should perform is Select matches.
 *                       See Table 6.20 in Gen2 protocol specification.
 *                      </td></tr>
 *   <tr><td>mem_bank</td><td>0 = RFU,<br>
 *                           1 = EPC,<br>
 *                           2 = TID,<br>
 *                           3 = User
 *                      </td></tr>
 *   <tr><td>address</td><td>16bit pointer in memory bank, where mask should be applied.
 *                           Will be converted into EBV format before Select command is sent.
 *                      </td></tr>
 *   <tr><td>mask_len</td><td>Length of mask in bits.
 *                      </td></tr>
 *   <tr><td>truncate</td><td>0 = Disable truncation.<br>
 *                           1 = Enable truncation.
 *                      </td></tr>
 *   <tr><td>mask</td><td>Actual mask data.
 *                      </td></tr>
 * </table>
 */
void callSelectTag(void)
{
#if RUN_ON_AS3980
    cmdBuffer.txSize = CMD_SELECT_REPLY_SIZE;
    cmdBuffer.result = ERR_REQUEST;
#else
    //powerUpReader();
    //checkAndSetSession(SESSION_GEN2);
    selectTag();
    //powerDownReader();
#endif
}

void selectTag(void)
{
    s8 status=ERR_NONE;
    u8 idx = 0;

    if (cmdBuffer.rxData[0] == 0)
    { /* Clear list */
        num_selects = 0;
        goto exit;
    }
    else if (cmdBuffer.rxData[0] == 1)
    { /* Add to list */
        if(num_selects == MAX_SELECTS)
        {
            status = ERR_NOMEM;
            goto exit;
        }
    }
    else if (cmdBuffer.rxData[0] == 2)
    { /* Clear and Add to list */
        num_selects = 0;
    }
    if(cmdBuffer.rxSize < 8)
    {
        status = ERR_PARAM;
        goto exit;
    }
    idx = num_selects;
    num_selects++;

    selParams[idx].target = cmdBuffer.rxData[1];
    selParams[idx].action = cmdBuffer.rxData[2];
    selParams[idx].mem_bank = cmdBuffer.rxData[3];
    selParams[idx].mask_address = cmdBuffer.rxData[4];
    selParams[idx].mask_address |= cmdBuffer.rxData[5]<<8;
    selParams[idx].mask_len = cmdBuffer.rxData[6];
    selParams[idx].truncation = cmdBuffer.rxData[7];

    if((cmdBuffer.rxSize - 9) < (selParams[idx].mask_len+7)/8)
    {
        memcpy(selParams[idx].mask, cmdBuffer.rxData+8, (cmdBuffer.rxSize - 8));
    }
    else
    {
        memcpy(selParams[idx].mask, cmdBuffer.rxData+8, (selParams[idx].mask_len+7)/8);
    }

    APPLOG("SELECT Tag target\n");
    APPLOGDUMP(selParams[idx].mask, (selParams[idx].mask_len+7)/8);
    initTagInfo();
  #if 0
    status = hopFrequencies();
    num_of_tags = 0;
    if (!status )
    {
        performSelects();
        num_of_tags = gen2SearchForTags(tags_, 1, 0, continueCheckTimeout, 1, 1, NULL);
    }
    hopChannelRelease();
    APPLOG("SELECTed %hx tags\n",num_of_tags);
    if (num_of_tags)
    {
        selectedTag = &tags_[0];
        status = ERR_NONE;    /* Tag found */
    }
    else
    {
        selectedTag = 0;
        status = GEN2_ERR_SELECT; /*Tag not found */
    }
  #else
    status = ERR_NONE;    /* Tag found */
  #endif

exit:
    cmdBuffer.txSize = CMD_SELECT_REPLY_SIZE;
    cmdBuffer.result = status;
}

/** This function writes to a previously selected (callSelectTag()) gen2 tag
  and is executed when a stream packet with protocol = #CMD_WRITE_TO_TAG
  is received. \n
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>       0</th><th>      1</th><th>  2 .. 5</th><th>6 .. (rxsize-1)</th></tr>
    <tr><th>Content</th><td>mem_bank</td><td>address</td><td>acces_pw</td><td>data           </td></tr>
  </table>
where 
<ul>
<li>mem_bank:<ul><li>0:reserved membank</li><li>1:EPC membank</li><li>2:TID membank</li><li>3:USER membank</li></ul>
<li>access_pw: if access password is nonzero the tag will be accessed first. (tag enters Secured state.)
</ul>
  The device sends back:
  <table>
    <tr><th>   Byte</th><th>     0           </th><th>     1                              </th></tr>
    <tr><th>Content</th><td>num_words_written</td><td>tag error code if status==ERR_HEADER</td></tr>
  </table>
 */
void callWriteToTag(void)
{
    powerUpReader();
    checkAndSetSession(SESSION_GEN2);
    writeToTag();
}

u8 writeTagMem(u32 memAdress,Tag const * tag, u8 const * data_buf, u8 data_length_words, u8 mem_type, s8* status, u8* tag_error)
{
    s8 error = ERR_NONE;
    u8 length = 0;
    u8 count;
    APPLOG("len=%hhx\n",data_length_words);

    while (length < data_length_words)
    {
        /*writing pc into tag */
        count=0;
        do
        {
            error = gen2WriteWordToTag(tag, mem_type, memAdress + length, &data_buf[2*length], tag_error);
            count++;
            if (error) delay_ms(20); /* Potentially writing is still ongoing, avoid metastable eeprom and stuff*/
        } while ( (count<7) && (error!=0) && continueCheckTimeout()); /* Give them 3 possible trials before giving up */
        if (maxSendingLimitTimedOut) error = GEN2_ERR_CHANNEL_TIMEOUT;

        if (error)
        {
            *status = error;
            return(length); /*error occourd while writing to tag */
        }
        length += 1;
    }

    return length;
}

void writeToTag(void)
{
    u8 len = 0;
    s8 status=0;
    cmdBuffer.txData[1] = 0;
    u8 membank = cmdBuffer.rxData[0];
    u32 address = readU32FromLittleEndianBuffer(&cmdBuffer.rxData[1]);
    u8 datalen = (cmdBuffer.rxSize - 9)/2;

    APPLOG("WRITE TO Tag\n");
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);
    POWER_AND_SELECT_TAG();
    if ( (cmdBuffer.rxData[5]==0) \
            &&(cmdBuffer.rxData[6]==0) \
            &&(cmdBuffer.rxData[7]==0) \
            &&(cmdBuffer.rxData[8]==0) ) /* accesspwd not set */
    {
        APPLOG("no need to access Tag\n");
    }
    else
    {
        status=gen2AccessTag(selectedTag, cmdBuffer.rxData+5);
        APPLOG("need to access Tag");
        if (status==0)
        {
            APPLOG(" -> suceeded\n");
        }
        else
        {
            APPLOG(" -> failed\n");
            goto exit;
        }
    }

    len = writeTagMem(address, selectedTag, cmdBuffer.rxData+9,
            datalen, membank, &status, cmdBuffer.txData + 1);

exit:

    hopChannelRelease();
    cmdBuffer.txSize = CMD_WRITE_TO_TAG_REPLY_SIZE;
    cmdBuffer.txData[0] = len;
    cmdBuffer.result = status;
}

#define RNDI 7          //index of first random value in buffer
static void pseudoRandomContinuousModulation()
{
    static u8 bufferIndex = 0;
    static u16 rnd;
    u8 buf[2];
    u16 rxbits = 0;

    //prepare a pseudo random value
    rnd ^= (cmdBuffer.rxData[RNDI+bufferIndex*2] | (cmdBuffer.rxData[RNDI+bufferIndex*2+1]<<8));      // get the random value from the GUI
    rnd ^= TMR3;            // add currently running timer value, to get some additional entropy

    bufferIndex++;
    if (bufferIndex > 5)
        bufferIndex = 0;

    // now send the data
#if RUN_ON_AS3980
    buf[0] = (rnd & 0x7F);
#else
    buf[0] = (rnd & 0xFF);
#endif
    buf[1] = (rnd >> 8) & 0xFF;
    //APPLOG("rnd: %hhx\n", rnd);
    //APPLOGDUMP(buf,2);
    as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRC, buf, 16, 0, &rxbits, 0, 0, 1);
}

/** This function sets/adds/measures frequency related stuff and is executed
  when a stream packet with protocol = #CMD_CHANGE_FREQ is received.
  \note The freq parameter contains the frequency in kHz and is always 3 bytes
  long. The first byte in the payload is the LSB. The frequency is extracted
  from the payload like this:
  \code
    u32 freq;
    freq = 0;
    freq += (u32)cmdBuffer.rxData[2];
    freq += ((u32)cmdBuffer.rxData[3]) << 8;
    freq += ((u32)cmdBuffer.rxData[4]) << 16;
  \endcode

  The format of the payload received from the host is one of the following:
  <ul>
  <li>Get RSSI level
  <table>
    <tr><th>   Byte</th><th>         0</th><th>1 .. 3</th></tr>
    <tr><th>Content</th><td>1 (SubCmd)</td><td>freq  </td></tr>
  </table>
  to which the reader replies with ERR_REQUEST since this features is not implemented/would require HW change.
  </li>
 
  <li>Get Reflected Power level
  <table>
    <tr><th>   Byte</th><th>         0</th><th>1 .. 3</th><th>4</th></tr>
    <tr><th>Content</th><td>2 (SubCmd)</td><td>freq  </td><td>Apply Tuner Setting</td></tr>
  </table>
  to which the reader replies with:
  <table>
    <tr><th>   Byte</th><th>        0</th><th>        1</th></tr>
    <tr><th>Content</th><td>I-channel</td><td>Q-channel</td></tr>
  </table>
  </li>

  <li>Add frequency to frequency list used for hopping. If clear_list is set the
      list will be deleted before adding the new frequency.
      rssi_threshhold and profile_id are global values and not set for every frequency.
      Therefore the last value which they have been set to is valid.
  <table>
    <tr><th>   Byte</th><th>         0</th><th>1 .. 3</th><th>    4     </th><th>          5         </th><th>    6     </th></tr>
    <tr><th>Content</th><td>4 (SubCmd)</td><td>freq  </td><td>clear_list</td><td>rssi_threshhold(dBm)</td><td>profile_id</td></tr>
  </table>
  Where rssi_threshold is used by the "listen before talk" feature: Before hopping
  to a new channel the reader listens on the frequency if the rssi value is smaller than this threshold.\n
  profile_id is used by the Reader Suite to enumerate the various frequency profiles. \n
  The reader replies with this:
  <table>
    <tr><th>   Byte</th><th>   0</th></tr>
    <tr><th>Content</th><td>0x00</td></tr>
  </table>
  </li>
 Status will be set to ERR_MEM if no more frequency can be added (see #MAXFREQ).
 If command was successful status will be ERR_NONE.

 <li>Get frequency list related parameters
  <table>
    <tr><th>   Byte</th><th>         0</th></tr>
    <tr><th>Content</th><td>5 (SubCmd)</td></tr>
  </table>
  The reader replies with:
  <table>
    <tr><th>   Byte</th><th>    0     </th><th> 1 .. 3 </th><th> 4 .. 6 </th><th>     7       </th><th>       8       </th></tr>
    <tr><th>Content</th><td>profile_id</td><td>min_freq</td><td>max_freq</td><td>curr_num_freqs</td><td>host_num_freqs</td></tr>
  </table>
  </li>
  Where profile_id and rssi_threshold refer to the values used in SubCmd 4.
  Values for the other parameters are:
  <table>
    <tr><th>Name</th></th><th>values</th></tr>
    <tr><td>min_freq</td><td>Minimum frequency of current frequency list.
                       </td></tr>
    <tr><td>max_freq</td><td>Maximum frequency of current frequency list.
                       </td></tr>
    <tr><td>curr_num_freq</td><td>Number of frequencies in current frequency list.
                       </td></tr>
    <tr><td>host_num_freq</td><td>Number of frequencies the host sent to add with SubCmd 4.
                           Might be bigger than curr_num_freq.
                       </td></tr>
  </table>
 
 <li>Set frequency hopping related parameters
  <table>
    <tr><th>   Byte</th><th>         0</th><th>1 .. 2       </th><th>3 .. 4        </th><th>5 .. 6  </th></tr>
    <tr><th>Content</th><td>8 (SubCmd)</td><td>listeningTime</td><td>maxSendingTime</td><td>idleTime</td></tr>
  </table>
  The reader replies with:
  <table>
    <tr><th>   Byte</th><th>   0</th></tr>
    <tr><th>Content</th><td>0x00</td></tr>
  </table>
  </li>
  Status of reply will be ERR_NONE.

  <li>Get frequency hopping related parameters
  <table>
    <tr><th>   Byte</th><th>         0</th></tr>
    <tr><th>Content</th><td>9 (SubCmd)</td></tr>
  </table>
  The reader replies with:
  <table>
    <tr><th>   Byte</th><th>     0 .. 1   </th><th>     2.. 3      </th><th>  4 .. 5 </th></tr>
    <tr><th>Content</th><td>listening_time</td><td>max_sending_time</td><td>idle_time</td></tr>
  </table>
  </li>
  Status of reply will be ERR_NONE.

  <li>Continuous modulation test
  <table>
    <tr><th>   Byte</th><th>          0</th><th>1 .. 3</th><th>    4 .. 5    </th><th>   6  </th><th>   7 .. 16 </th></tr>
    <tr><th>Content</th><td>16 (SubCmd)</td><td> freq </td><td>duration in ms</td><td>random</td><td>random_data</td></tr>
  </table>
  causes continuous modulation of the RF field for given duration. If random is set to 0 Gen2 NAK commands are sent (On AS3980 NAK
  direct command is not available therefore Transmit with CRC with dummy payload is used).
  If random is set to 1 pseudo random data is sent. For this the random_data and timer values are used as random data to send. \n
  \note The reader does not reply any data to this command as it enters continuous modulation mode directly.

  \n
  \note Communication with host is not possible while continous modulation is performed. Wait at least for duration
        time before starting communication attempt with reader.
  </li>
  </ul>
 */
void callChangeFreq(void)
{
    u16 reflectedValues;
    u16 noiseLevel;
    u32 freq;
    freq = 0;
    freq += (u32)cmdBuffer.rxData[1];
    freq += ((u32)cmdBuffer.rxData[2]) << 8;
    freq += ((u32)cmdBuffer.rxData[3]) << 16;

    APPLOG("CHANGE FREQ f=%x%x, subcmd=%hhx\n", freq, cmdBuffer.rxData[0]);
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);

    switch (cmdBuffer.rxData[0])
    {
        case 0x02:  // get reflected Power
            {
                if (cmdBuffer.rxSize < CMD_CHANGE_FREQ_REFL_RX_SIZE)
                {
                    cmdBuffer.result = ERR_PARAM;
                    cmdBuffer.txSize = 2;
                    break;
                }
                powerUpReader();
                as3993SetBaseFrequency(AS3993_REG_PLLMAIN1, freq);
                #ifdef TUNER
                if (cmdBuffer.rxData[4] > 0)
                {
                    applyTunerSettingForFreq(freq);
                }
                #endif
                noiseLevel = as3993GetReflectedPowerNoiseLevel();
                as3993AntennaPower(1);
                reflectedValues = as3993GetReflectedPower();
                as3993AntennaPower(0);
                cmdBuffer.txSize = 2;
                cmdBuffer.txData[0] = (reflectedValues & 0xff) - (noiseLevel & 0xff);
                cmdBuffer.txData[1] = ((reflectedValues >> 8)&0xff) - ((noiseLevel >> 8)&0xff);
                cmdBuffer.result = ERR_NONE;
                powerDownReader();
                break;
            }
        case 0x04:
            { /* Add to the frequency List */
                if (cmdBuffer.rxSize < CMD_CHANGE_FREQ_ADD_RX_SIZE)
                {
                    cmdBuffer.result = ERR_PARAM;
                    cmdBuffer.txSize = 1;
                    break;
                }
                if (cmdBuffer.rxData[4])      // clear frequency list before adding
                {
                    Frequencies.numFreqs = 0;
                    guiNumFreqs = 0;
                    guiMaxFreq = freq;
                    guiMinFreq = freq;
                }
                Frequencies.numFreqs++;
                guiNumFreqs++;
                if (Frequencies.numFreqs > MAXFREQ)
                {
                    Frequencies.numFreqs = MAXFREQ;
                    cmdBuffer.txSize = 1;
                    cmdBuffer.txData[0] = 0;
                    cmdBuffer.result = ERR_NOMEM;
                }
                else
                {
                    Frequencies.freq[Frequencies.numFreqs - 1] =  freq;
                    guiActiveProfile = cmdBuffer.rxData[5];
                    if (guiMaxFreq < freq) guiMaxFreq = freq;
                    if (guiMinFreq > freq) guiMinFreq = freq;
                    cmdBuffer.txSize = 1;
                    cmdBuffer.txData[0] = 0x00;
                    cmdBuffer.result = ERR_NONE;
                }
                break;
            }
        case 0x05:
            {   // get frequency list parameters
                cmdBuffer.txData[0] = guiActiveProfile;
                cmdBuffer.txData[1] = guiMinFreq & 0xff;
                cmdBuffer.txData[2] = (guiMinFreq >> 8) & 0xff;
                cmdBuffer.txData[3] = (guiMinFreq >> 16) & 0xff;
                cmdBuffer.txData[4] = guiMaxFreq & 0xff;
                cmdBuffer.txData[5] = (guiMaxFreq >> 8) & 0xff;
                cmdBuffer.txData[6] = (guiMaxFreq >> 16) & 0xff;
                cmdBuffer.txData[7] = Frequencies.numFreqs;
                cmdBuffer.txData[8] = guiNumFreqs;
                cmdBuffer.txSize = 9;
                cmdBuffer.result = ERR_NONE;
                break;
            }
        case 0x08:
            {
                /* set parameters for frequency hopping */
                if (cmdBuffer.rxSize < CMD_CHANGE_FREQ_SETHOP_RX_SIZE)
                {
                    cmdBuffer.result = ERR_PARAM;
                    cmdBuffer.txSize = 1;
                    break;
                }
                listeningTime  =  cmdBuffer.rxData[1];
                listeningTime |= (cmdBuffer.rxData[2]<<8);
                maxSendingTime  =  cmdBuffer.rxData[3];
                maxSendingTime |= (cmdBuffer.rxData[4]<<8);
                idleTime  =  cmdBuffer.rxData[5];
                idleTime |= (cmdBuffer.rxData[6]<<8);
                rssiThreshold =  cmdBuffer.rxData[7];
                cmdBuffer.txSize = 1;
                cmdBuffer.txData[0] = 0x00;
                if (maxSendingTime <50)
                {
                    maxSendingTime = 50;
                }
                cmdBuffer.result = ERR_NONE;
                break;
            }
        case 0x09:
            {   /* get frequency hopping infos */
                cmdBuffer.txSize = 7;
                cmdBuffer.txData[0] = listeningTime & 0xff;
                cmdBuffer.txData[1] = (listeningTime >> 8) & 0xff;
                cmdBuffer.txData[2] = maxSendingTime & 0xff;
                cmdBuffer.txData[3] = (maxSendingTime >> 8) & 0xff;
                cmdBuffer.txData[4] = idleTime & 0xff;
                cmdBuffer.txData[5] = (idleTime >> 8) & 0xff;
                cmdBuffer.txData[6] = rssiThreshold;
                cmdBuffer.result = ERR_NONE;
                break;
            }
        case 0x10:
            {
                /* continuous modulation */
                if (cmdBuffer.rxSize < CMD_CHANGE_FREQ_CONTMOD_RX_SIZE)
                {
                    cmdBuffer.result = ERR_PARAM;
                    cmdBuffer.txSize = 0;
                    break;
                }
                u16 time_ms = cmdBuffer.rxData[4] | (cmdBuffer.rxData[5]<<8);
                u8 rxnorespwait, rxwait;
                u16 rxbits = 0;
#if RUN_ON_AS3980 
                u8 txbuf[] = {0x9E, 0x9E};
#endif
                APPLOG("continuous modulation, duration: %hx\n", time_ms);
                powerUpReader();
                rxnorespwait = as3993SingleRead( AS3993_REG_RXNORESPONSEWAITTIME );
                rxwait       = as3993SingleRead( AS3993_REG_RXWAITTIME);
                as3993SingleWrite( AS3993_REG_RXNORESPONSEWAITTIME, 0 );
                as3993SingleWrite( AS3993_REG_RXWAITTIME, 0 );
                as3993SetBaseFrequency(AS3993_REG_PLLMAIN1, freq);
                checkAndSetSession(SESSION_GEN2);
                cmdBuffer.txSize = 0;
                maxSendingLimit = time_ms;
                maxSendingLimitTimedOut = 0;
                as3993AntennaPower(1);
                delay_ms(1);
                slowTimerStart();
                do{
                    if (cmdBuffer.rxData[6] == 0x01)
                    {   /* pseudo random continuous modulation */
                        pseudoRandomContinuousModulation();
                    }
                    else
                    {   /* static modulation */
#if RUN_ON_AS3980
                        as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRC, txbuf, 16, 0, &rxbits, 0, 0, 1);
#else
                        as3993TxRxGen2Bytes(AS3993_CMD_NAK, 0, 0, 0, &rxbits, 0, 0, 1);
#endif
                    }
                    as3993ClrResponse();
                }while(continueCheckTimeout());
                as3993AntennaPower(0);
                as3993SingleWrite( AS3993_REG_RXNORESPONSEWAITTIME, rxnorespwait );
                as3993SingleWrite( AS3993_REG_RXWAITTIME, rxwait );
                powerDownReader();
                APPLOG("finished sending \n");
                return;
            }


        default:
            {
                cmdBuffer.txSize = 2;
                cmdBuffer.txData[0] = 0xFF;
                cmdBuffer.txData[1] = 0xFF;
                cmdBuffer.result = ERR_REQUEST;
                break;
            }
    }
    APPLOG("changeFreq finished, reply data: \n");
    APPLOGDUMP(cmdBuffer.txData, cmdBuffer.txSize);
}

/** This function reads from a previously selected gen2 tag and is executed
  when a stream packet with protocol = #CMD_READ_FROM_TAG is received. \n
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>       0</th><th>      1..4</th><th>    5..8</th></tr>
    <tr><th>Content</th><td>mem_bank</td><td>address</td><td>password</td></tr>
  </table>
  rxSize divided by two defines the number of words to be read. If rxSize is 
  odd then word by word reading is used. If even the desired memory is read 
  as one bulk.
  Parameters are:
  <ul>
  <li>mem_bank:<ul><li>0:reserved membank</li><li>1:EPC membank</li><li>2:TID membank</li><li>3:USER membank</li></ul>
  <li>address: the wordPtr where to read from</li>
  </ul>
  The device sends back the following report:
  <table>
    <tr><th>   Byte</th><th>0..txSize(max)</th></tr>
    <tr><th>Content</th><td> data         </td></tr>
  </table>
  If the returned error is ERR_HEADER, the txSize will be odd and the last byte will be the error code returned by the tag.
 */
void callReadFromTag(void)
{
    powerUpReader();
    checkAndSetSession(SESSION_GEN2);
    readFromTag();
}

void readFromTag(void)
{
    s8 status = ERR_NONE;
    u8 membank = cmdBuffer.rxData[0];
    u32 wrdPtr = readU32FromLittleEndianBuffer(&cmdBuffer.rxData[1]);
    u8 datalen = cmdBuffer.txSize;
    u8 rxed = CMD_ERROR_REPLY_SIZE;// set to 1 if POWER_AND_SELECT_TAG failed

    APPLOG("READ FROM Tag\n");
    APPLOG("membank = %hhx\n", membank);
    APPLOG("wrdptr = %hx%hx\n", wrdPtr);
    APPLOG("raw datalen = %hhx\n", datalen);
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);
    memset(cmdBuffer.txData, 0x00, cmdBuffer.txSize);

    POWER_AND_SELECT_TAG();

    if ( (cmdBuffer.rxData[5]==0) \
            &&(cmdBuffer.rxData[6]==0) \
            &&(cmdBuffer.rxData[7]==0) \
            &&(cmdBuffer.rxData[8]==0) ) /* accesspwd not set */
    {
        APPLOG("no need to access Tag\n");
    }
    else
    {
        status=gen2AccessTag(selectedTag, cmdBuffer.rxData+5);
        APPLOG("need to access Tag");
        if (status==0)
        {
            APPLOG(" -> suceeded\n");
        }
        else
        {
            APPLOG(" -> failed status: %hhx\n", status);
            status=GEN2_ERR_ACCESS;
            goto exit;
        }
    }
    rxed = 0;
    if (datalen & 1)
    { /* Special mode for odd size: read word by word until error */
        u8 * b = cmdBuffer.txData;
        datalen /= 2;
        while(ERR_NONE == status && datalen && continueCheckTimeout()) 
        {
            status = gen2ReadFromTag(selectedTag, membank, wrdPtr++, 1, b);
            if (ERR_CHIP_HEADER == status)
            { /* Header bit means only one byte status */
                rxed++;
            }
            else if (ERR_NONE == status)
            {
                rxed+=2;
            }
            b+=2;
            datalen--;
        }
        if (maxSendingLimitTimedOut) status = GEN2_ERR_CHANNEL_TIMEOUT;
    }
    else
    {
        datalen /= 2;
        status = gen2ReadFromTag(selectedTag, membank, wrdPtr, datalen, cmdBuffer.txData);
        if (ERR_CHIP_HEADER == status)
        { /* Header bit means only one byte status */
            rxed = 1;
        }
        else if (ERR_NONE == status)
        {
            rxed = datalen * 2;
        }
    }
exit:
    hopChannelRelease();
    cmdBuffer.txSize = rxed;
    cmdBuffer.result = status;
    APPLOG("read finished, status: %hhx  rxed: %hhx\n", status, rxed);
    APPLOGDUMP(cmdBuffer.txData, cmdBuffer.txSize);
}

/** This function locks\\unlocks a gen2 tag and is executed
  when a stream packet with protocol = #CMD_LOCK_UNLOCK_TAG is received. \n
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>          0..2 </th><th>         3 .. 5</th></tr>
    <tr><th>Content</th><td>mask_and_action</td><td>access password</td></tr>
  </table>
  The device sends back the following report:
  <table>
    <tr><th>   Byte</th><th>       0</th></tr>
    <tr><th>Content</th><td>tag_code</td></tr>
  </table>
 */
void callLockUnlockTag(void)
{
    powerUpReader();
    checkAndSetSession(SESSION_GEN2);
    lockUnlockTag();
}
void lockUnlockTag(void)
{
    const u8 *mask = cmdBuffer.rxData;
    s8 status;
    APPLOG("Lock Tag\n");
    APPLOG("Command  %hhx %hhx\n", cmdBuffer.rxData[0], cmdBuffer.rxData[1]);
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);

    POWER_AND_SELECT_TAG();

    if ( (cmdBuffer.rxData[3]==0)
            &&(cmdBuffer.rxData[4]==0)
            &&(cmdBuffer.rxData[5]==0)
            &&(cmdBuffer.rxData[6]==0) ) /* accesspwd not set */
    {
        APPLOG("no need to access Tag\n");
    }
    else
    {
        status=gen2AccessTag(selectedTag, &cmdBuffer.rxData[3]);
        if (status!=0) goto exit;
        APPLOG("need to access Tag");
        if (status==0)APPLOG(" access suceeded\n");
        else APPLOG(" access failed\n");
    }

    APPLOG("LOCK UNLOCK Tag: mask_and_action %hhx %hhx %hhx \n", mask[0],mask[1],mask[2]);
    status = gen2LockTag(selectedTag, mask, cmdBuffer.txData);

exit:
    if (status) delay_ms(20); /* Potentially transaction is still ongoing, avoid metastable eeprom and stuff*/
    hopChannelRelease();
    cmdBuffer.result = status;
    cmdBuffer.txSize = CMD_LOCK_REPLY_SIZE;
}

/** This function kills a gen2 tag and is executed
  when a stream packet with protocol = #CMD_KILL_TAG is received. \n
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>0 .. 3       </th><th>   4 </th></tr>
    <tr><th>Content</th><td>kill password</td><td>recom</td></tr>
  </table>
  where:
  <ul>
    <li>recom: see GEN2 standard: table on "XPC_W1 LSBs and a Tag's recomissioned status"
    </li>
  </ul>
  The device sends back the following report:
  <table>
    <tr><th>   Byte</th><th>     0</th></tr>
    <tr><th>Content</th><td>status</td></tr>
  </table>
 */
void callKillTag(void)
{
    s8 status;
    const u8* password = cmdBuffer.rxData;
    u8 rfu   = cmdBuffer.rxData[4];
    u8 recom = cmdBuffer.rxData[5];

    powerUpReader();
    checkAndSetSession(SESSION_GEN2);

    APPLOG("KILL Tag\n");

    POWER_AND_SELECT_TAG();

    status = gen2KillTag(selectedTag, password, rfu, recom, cmdBuffer.txData);

exit:
    if (status) delay_ms(20); /* Potentially writing is still ongoing, avoid metastable eeprom and stuff*/
    hopChannelRelease();
    cmdBuffer.txSize = CMD_KILL_TAG_REPLY_SIZE;
    cmdBuffer.result = status;
}

/** This function starts/stops cyclic inventory rounds and is executed when a stream
  packet with protocol = #CMD_START_STOP is received.\n
  The cyclic inventory rounds are performed by
  calling callInventoryGen2() from the main loop. Therefore after starting
  a cyclic inventory, the host has to expect data just like after issueing
  the callInventoryGen2() because sendTagData() will be called repeatedly.\n
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>     0</th><th>    1</th><th>      2</th><th>                   3</th><th>   4</th></tr>
    <tr><th>Content</th><td>update</td><td>start</td><td>autoAck</td><td> tid(2LSB)+fast(LSB)</td><td>rssi</td></tr>
  </table>
  If update is zero the command is ignored. Parameter start defines if the cyclic inventories are
  started (1) or stopped (0). \n
  For description of autoAck,tid, fast and rssi parameters see callInventoryGen2(). \n
  The device sends back:
  <table>
    <tr><th>   Byte</th><th>                  0</th></tr>
    <tr><th>Content</th><td>current start value</td></tr>
  </table>
    Subsequently callInventoryGen2() result packets are returned in a
    dense continuous loop.
 */
void callStartStop(void)
{
    APPLOG("STARTSTOP: %hhx  %hhx\n", cmdBuffer.rxData[0], cmdBuffer.rxData[1]);
    if (cmdBuffer.rxData[0])
    {
        cyclicInventory = cmdBuffer.rxData[1];
        if (cyclicInventory)
        {
            autoAckMode = cmdBuffer.rxData[2];
            fastInventory = cmdBuffer.rxData[3] & 0x01;
            read_TID_CAL_inInventoryRound = (cmdBuffer.rxData[3] & 0x02) >> 1 ;
            read_MMS_VFC_TEMP_inInventoryRound = (cmdBuffer.rxData[3] & 0x04) >> 2;
            rssiMode = cmdBuffer.rxData[4];
        }
    }
    cmdBuffer.txSize = CMD_START_STOP_REPLY_SIZE;
    cmdBuffer.result   = ERR_NONE;
    if(!cyclicInventory)
        powerDownReader();
}
/** This function executes an generic command from a previously selected gen2 tag
  when a stream packet with protocol = #CMD_GENERIC_CMD_ID is received. \n
  The format of the payload from the host is:
  <table>
    <tr><th>   Byte</th><th>    0..3</th><th>   4..5[UpperNibble]</th><th>  5[LowerNibble]..6</th><th>             7</th><th>               8</th><th>9..9+TransmitByteSize</th></tr>
    <tr><th>Content</th><td>password</td><td>length_transmit_data</td><td>length_receive_data</td><td>direct command</td><td>no_response_time</td><td>        transmit data</td></tr>
  </table>
 
  Parameters are:
  <ul>
  <li>password: access password; if the password is nonzero the tag will accessed first</li>
  <li>length_transmit_data: bit size of the data to transmit without RN16 and CRC-16</li>
  <li>length_receive_data: bit size of the data to receive without RN16 and CRC-16</li>
  <li>direct command:<ul><li>0x90 AS3993_CMD_TRANSMCRC</li><li>0x91 AS3993_CMD_TRANSMCRCEHEAD</li><li>0x92 AS3993_CMD_TRANSMNOCRC</li></ul></li>
  <li>no_response_time: no respone interrupt wait time</li>
  <li>transmit data: data to transmit to the tag</li>
  </ul>
 
  The device sends back the following report:
  <table>
    <tr><th>   Byte</th><th>                   0</th><th>                   1</th><th>2..2+ReceivedByteSize</th></tr>
    <tr><th>Content</th><td>status</td><td>length_received_data</td><td>received data </td></tr>
  </table>

 */
void callGenericCommand(void)
{
    powerUpReader();
    checkAndSetSession(SESSION_GEN2);
    executeGenericCommand();
}

void executeGenericCommand()
{
    s8 status = ERR_NONE;
    u16 toTagSize, fromTagSize;
    u8 toTagBuffer[200];
    u8 cmd, norestime;
    u16 dataBytes, leftBits, startBytehandle;
    u16 receivedBytes=CMD_ERROR_REPLY_SIZE;// set to CMD_ERROR_REPLY_SIZE if POWER_AND_SELECT_TAG failed

    APPLOG("Execute GCom: rxed: %hhx, txed: %hhx\n ", cmdBuffer.rxSize, cmdBuffer.txSize);
    APPLOGDUMP(cmdBuffer.rxData, cmdBuffer.rxSize);
    memset(cmdBuffer.txData, 0x00, cmdBuffer.txSize);

    POWER_AND_SELECT_TAG();

    if ( (cmdBuffer.rxData[0]==0) &&(cmdBuffer.rxData[1]==0) &&(cmdBuffer.rxData[2]==0) &&(cmdBuffer.rxData[3]==0) ) /* accesspwd not set */
    {
        APPLOG("no need to access Tag\n");
    }
    else
    {
        status=gen2AccessTag(selectedTag, cmdBuffer.rxData);
        APPLOG("need to access Tag");
        if (status==0)
        {
            APPLOG(" -> suceeded\n");
        }
        else
        {
            APPLOG(" -> failed status: %hhx\n", status);
            status=GEN2_ERR_ACCESS;
            goto exit;
        }
    }

    // Get Values from Stream
    toTagSize = (cmdBuffer.rxData[4] << 4) | ((cmdBuffer.rxData[5] & 0xF0) >> 4);
    fromTagSize = ((cmdBuffer.rxData[5] & 0x0F) << 8) | (cmdBuffer.rxData[6]);
    cmd = cmdBuffer.rxData[7];
    norestime = cmdBuffer.rxData[8];

    dataBytes = (toTagSize - 16 + 7)/8 ;
    leftBits = (dataBytes * 8) -(toTagSize-16);
    startBytehandle = dataBytes - 1;
    if ( leftBits == 0 )
    {
        startBytehandle++;
        leftBits = 8;
    }
    insertBitStream(&toTagBuffer[0], (cmdBuffer.rxData)+9, dataBytes, 8 );
    insertBitStream(&toTagBuffer[startBytehandle], selectedTag->handle, 2, leftBits);

    status = as3993TxRxGen2Bytes(cmd, toTagBuffer, toTagSize, (cmdBuffer.txData + 2), &fromTagSize, norestime, 0, 1);

    cmdBuffer.txData[1] = (u8)fromTagSize;
    
    receivedBytes = (fromTagSize+7)/8 + 2;
    APPLOG("fromTagBuf: status: %hhx  txed: %hhx\n", status, fromTagSize);
    APPLOGDUMP(cmdBuffer.txData, receivedBytes);
    delay_ms(20);
exit:
    hopChannelRelease();
    cmdBuffer.txData[0] = (u8) status;
    cmdBuffer.txSize = receivedBytes;
    cmdBuffer.result = status;
}

void callRSSIMeasureCMD(void)
{
    powerUpReader();
    checkAndSetSession(SESSION_GEN2);
    executeRSSICMD();
}

void executeRSSICMD()
{
    s8 status = ERR_NONE;
    u8 count = cmdBuffer.txSize / 4;
    u8 *b = cmdBuffer.txData;
    u32 freq;

    freq = 0;
    freq += (u32)cmdBuffer.rxData[0];
    freq += ((u32)cmdBuffer.rxData[1]) << 8;
    freq += ((u32)cmdBuffer.rxData[2]) << 16;

    APPLOG("RSSI CMD f=%x%x\n", freq);

    cmdBuffer.txSize = 0;

    as3993AntennaPower(1);

    as3993SetBaseFrequency(AS3993_REG_PLLMAIN1, freq);

    performSelects();

    while ( count-- )
    {
        status = gen2QueryMeasureRSSI(b+0, b+1, (s8*) b+2, (s8*) b+3);
        if (status)
        {/* Pick lowest values and invalid agc_reg to denote fail */
            b[0] = 0;
            b[1] = 0;
            b[2] = SCHAR_MIN;
            b[3] = SCHAR_MIN;
        }
        b+=4; 
        cmdBuffer.txSize += 4;
    }

    as3993AntennaPower(0);
}

void initCommands(void)
{
    currentSession = 0;
    cyclicInventory = 0;
    fastInventory = 0;
    read_TID_CAL_inInventoryRound = 0;
    read_MMS_VFC_TEMP_inInventoryRound = 0;
    autoAckMode = 1;
    rssiMode = 0x06;    //rssi at 2nd byte
    rssiThreshold = -40;
    readerPowerDownMode = POWER_NORMAL;

    tagDataAvailable = 0;
    num_of_tags = 0;
#ifdef ANTENNA_SWITCH
    SWITCH_ANTENNA(usedAntenna);
#endif
    as3993AntennaPower(0);
    powerDownReader();
}

#ifdef TUNER
static void applyTunerSettingForFreq(u32 freq)
{
    u8 i, idx;
    unsigned long int diff, best;

    if (tuningTable.tableSize == 0)     //tuning is disabled
        return;

    //find the best matching frequency
    best = 1.0e4;
    idx = 0;
    for(i=0; i<tuningTable.tableSize; i++)
    {
        //CON_print("***** find tune f=%x%x, tablefreq=%x%x, idx=%hhx", freq, tuningTable.freq[i], idx);
        if (tuningTable.freq[i] > freq)
            diff = tuningTable.freq[i] - freq;
        else
            diff = freq - tuningTable.freq[i];
        if (diff < best)
        {
            idx = i;
            best = diff;
        }
    }
    //apply the found parameters if enabled for the current antenna
    tuningTable.currentEntry = idx;
    if (tuningTable.tuneEnable[idx] & usedAntenna)
    {
        tunerSetTuning(&mainTuner, tuningTable.cin[usedAntenna - 1][idx],
                tuningTable.clen[usedAntenna - 1][idx],
                tuningTable.cout[usedAntenna - 1][idx]);
        APPLOG("***** apply tune f=%x%x, idx=%hhx,  tune1=%hhx cin=%hhx clen=%hhx cout=%hhx\n", freq, idx,
                tuningTable.tuneEnable[idx], tuningTable.cin[usedAntenna-1][idx],
                tuningTable.clen[usedAntenna-1][idx], tuningTable.cout[usedAntenna-1][idx]);
        tunerParams.cin = tuningTable.cin[usedAntenna - 1][idx];
        tunerParams.clen = tuningTable.clen[usedAntenna - 1][idx];
        tunerParams.cout = tuningTable.cout[usedAntenna - 1][idx];
    }
}
#endif


static s8 hopFrequencies(void)
{
    s8 dBm = -128;
    u16 idleDelay;

    slowTimerStart();       //start timer for idle delay
    powerUpReader();
    as3993AntennaPower(0);

    idleDelay = slowTimerValue();
    if ( idleTime > idleDelay)
    { /* wait for idle time */
        delay_ms(idleTime -  idleDelay);
    }
    slowTimerStop();
    if (Frequencies.numFreqs == 0)
    {
        if (!cyclicInventory) 
            powerDownReader();
        return GEN2_ERR_CHANNEL_TIMEOUT;
    }

    if ( ++currentFreqIdx >= Frequencies.numFreqs ) currentFreqIdx = 0;
    as3993SetBaseFrequency(AS3993_REG_PLLMAIN1, Frequencies.freq[currentFreqIdx]);

    maxSendingLimit = maxSendingTime;

    if (dBm <= rssiThreshold)
    {
        slowTimerStart();
        maxSendingLimitTimedOut = 0;
#ifdef TUNER
        applyTunerSettingForFreq(Frequencies.freq[currentFreqIdx]);
#endif
#if RADON
        delay_ms(1); // FIXME uhe 2013-11-21 an extra delay is needed after as3993GetRSSI() before turning on field otherwise refleced power values are really high
#endif
        as3993AntennaPower(1);
#if RADON
        {
            u16 refl;
            u8 regs[2];
            /* Read configured gain values */
            regs[0] = as3993SingleRead(AS3993_REG_TRCALHIGH);
            regs[1] = as3993SingleRead(AS3993_REG_RXMIXERGAIN);
            /* Write nominal gain values */
            as3993SingleWrite(AS3993_REG_TRCALHIGH, 0x00);
            as3993SingleWrite(AS3993_REG_RXMIXERGAIN, 0x00);
            refl = tunerGetReflected();
            /* Restore configured gain values */
            as3993SingleWrite(AS3993_REG_TRCALHIGH, regs[0]);
            as3993SingleWrite(AS3993_REG_RXMIXERGAIN,regs[1]);
            if (refl > 6032)
            {
                REFLPOWERTOOHIGHLED(LEDON);
                maxSendingLimitTimedOut = 1;
                as3993AntennaPower(0);
                if (!cyclicInventory) 
                    powerDownReader();
                delay_ms(100);
                APPLOG("hopFrequencies failed: reflected power too high: %hx > %hx\n",refl,6032);
                return ERR_REFLECTED_POWER;
            }
            else 
            {
                REFLPOWERTOOHIGHLED(LEDOFF);
            }
        }
#endif
#ifdef TUNER
        if ( tuningTable.tableSize > 0 &&
                ++Frequencies.countFreqHop[currentFreqIdx] > 500 )
            /*If tuning is enabled and this frequency has been selected for the 500th time,
              check if we have to do a re-tune (autotune) */
        {
            u16 refl;
            refl = tunerGetReflected();
            APPLOG("countFreqHop has reached %hhx\n",Frequencies.countFreqHop[currentFreqIdx]);
            APPLOG("old reflected power: %hx\n", tuningTable.tunedIQ[usedAntenna-1][tuningTable.currentEntry]);
            APPLOG("measured reflected power: %hx\n", refl);
            Frequencies.countFreqHop[currentFreqIdx] = 0;
            if ( refl > tuningTable.tunedIQ[usedAntenna-1][tuningTable.currentEntry] * 1.3 ||
                    refl < tuningTable.tunedIQ[usedAntenna-1][tuningTable.currentEntry] * 0.7 )
                /* if reflected power differs 30% compared to last tuning time, redo tuning */
            {
                tunerOneHillClimb(&mainTuner, &tunerParams, 100);
                //tunerMultiHillClimb(&antennaParams);
                APPLOG("redo tuning, old cin: %hhx, clen: %hhx, cout: %hhx\n", tuningTable.cin[usedAntenna-1][tuningTable.currentEntry],
                        tuningTable.clen[usedAntenna-1][tuningTable.currentEntry], tuningTable.cout[usedAntenna-1][tuningTable.currentEntry]);
                APPLOG("new values cin: %hhx, clen: %hhx, cout: %hhx, iq: %hx\n", tunerParams.cin,
                        tunerParams.clen, tunerParams.cout, tunerParams.reflectedPower);
                tuningTable.cin[usedAntenna-1][tuningTable.currentEntry] = tunerParams.cin;
                tuningTable.clen[usedAntenna-1][tuningTable.currentEntry] = tunerParams.clen;
                tuningTable.cout[usedAntenna-1][tuningTable.currentEntry] = tunerParams.cout;
                tuningTable.tunedIQ[usedAntenna-1][tuningTable.currentEntry] = tunerParams.reflectedPower;
            }
        }
#endif
        return GEN2_OK;
    }
    else
    {
        maxSendingLimitTimedOut = 1;
        as3993AntennaPower(0);
        if (!cyclicInventory) 
            powerDownReader();
        APPLOG("hopFrequencies failed: did not find a free channel\n");
        return GEN2_ERR_CHANNEL_TIMEOUT;
    }
}

static void hopChannelRelease(void)
{
    slowTimerStop();
    if (readerPowerDownMode != POWER_NORMAL_RF)
        as3993AntennaPower(0);
    if (!cyclicInventory)
        powerDownReader();
}

/**
 * This function is called periodically from main() loop. If a cyclic inventory has
 * been started callInventoryGen2Internal() is called.
 */
int doCyclicInventory(void)
{
    if (cyclicInventory)
    {
        callInventoryGen2Internal();
        return 1;
    }
    return 0;
}

/** Handles the configured power down mode of the reader. The power down mode
 * is define in readerPowerDownMode variable and can be changed via callReaderConfig().
 * Available modes are: #POWER_DOWN, #POWER_NORMAL, #POWER_NORMAL_RF and #POWER_STANDBY
 */
static void powerDownReader(void)
{
    switch (readerPowerDownMode)
    {
        case POWER_DOWN:
            as3993EnterPowerDownMode();
            break;
        case POWER_NORMAL:
            as3993EnterPowerNormalMode();
            break;
        case POWER_NORMAL_RF:
            as3993EnterPowerNormalRfMode();
            break;
        case POWER_STANDBY:
            as3993EnterPowerStandbyMode();
            break;
        default:
            as3993EnterPowerDownMode();
    }
}

/**
 * Handles power up of reader. Basically reverts changes done by powerDownReader().
 */
static void powerUpReader(void)
{
    switch (readerPowerDownMode)
    {
        case POWER_DOWN:
            as3993ExitPowerDownMode();
            break;
        case POWER_NORMAL:
            as3993ExitPowerNormalMode();
            break;
        case POWER_NORMAL_RF:
            as3993ExitPowerNormalRfMode();
            break;
        case POWER_STANDBY:
            as3993ExitPowerStandbyMode();
            break;
        default:
            as3993ExitPowerDownMode();
    }
}


/**
 * Process various generic commands.
 * @param protocol : forward from applProcessCmd()
 * @param rxData : forward from applProcessCmd()
 * @param rxSize : forward from applProcessCmd()
 * @param txData : forward from applProcessCmd()
 * @param txSize : forward from applProcessCmd()
 * @return error code for processed command
 *
 * \section streamprotocol Protocol description
 * The following description gives an overview of the
 * ams stream protocol and how the data
 * of stream packets is forwarded to the commands() function.
 * The protocol can be executed over USB or UART.
 * Only the header bytes between USB and UART are different.
 *
 * For a detailed description of the ams stream protocol have a look in
 * ams_stream.h
 *
 * \subsection streamprotocolUSB Protocol for USB:
 * A simple USB stream packet (not splitted, only 1 protocol in packet) is shown in
 * the following table, which maps the transmitted data to the function parameters
 * of commands().
 * <table>
 *   <tr><th>     Byte</th><th>  0</th><th>      1</th><th>       2</th><th>       3</th><th>  4..5</th><th>  6..7</th><th>8..8+txprot-1</th></tr>
 *   <tr><th>  Content</th><td>TID</td><td>payload</td><td>reserved</td><td>protocol</td><td>txprot</td><td>rxprot</td><td>data</td></tr>
 *   <tr><th>Parameter</th><td>   </td><td>       </td><td>        </td><td>protocol</td><td>rxSize</td><td>txSize</td><td>rxData[0..rxSize-1]</td></tr>
 * </table>
 * where
 * <ul>
 *   <li> \e TID : transaction ID.
 *   <li> \e payload: amount of data in report.
 *   <li> \e protocol: defines which action should be triggered. If commands() is
 *      is called it refers to a firmware application command (no generic stream command).
 *   <li> \e txprot: amount of data which has been received and should be forwarded
 *      to command function.
 *   <li> \e rxprot: This variable contains the expected size of data this command
 *      will return and will be set to the size of the actual data to be tx-ed back
 *      to host.
 *   <li> \e data: input data for command
 * </ul>
 * The response will look like this:
 * <table>
 *   <tr><th>     Byte</th><th>  0</th><th>      1</th><th>            2</th><th>       3</th><th>       4</th><th>          5</th><th>  6..7</th><th>8..8+txprot-1</th></tr>
 *   <tr><th>  Content</th><td>TID</td><td>payload</td><td>stream status</td><td>protocol</td><td>reserved</td><td>prot status</td><td>txprot</td><td>data</td></tr>
 *   <tr><th>Parameter</th><td>   </td><td>       </td><td>             </td><td>protocol</td><td>        </td><td>     status</td><td>txSize</td><td>txData[0..txSize-1]</td></tr>
 * </table>
 * where
 * <ul>
 *   <li> \e TID : transaction ID.
 *   <li> \e payload: amount of data in report.
 *   <li> \e stream status : a non-zero value indicates that a previous command
 *      could not be processed
 *   <li> \e protocol : defines to which action the transmitted data belongs
 *   <li> \e prot status : a non-zero value indicates error condition of command
 *      (return value of function commands())
 *   <li> \e txprot: size of data in response
 *   <li> \e data: response data for command
 * </ul>
 * 
 * \subsubsection streamexample Example for the antenna power command, handled by callAntennaPower():
 * Request from host (Reader Suite):
 * <table>
 *   <tr><th>   Byte</th><th>   0</th><th>      1</th><th>       2</th><th>       3</th><th>  4..5</th><th>  6..7</th><th>   8</th></tr>
 *   <tr><th>Content</th><td> TID</td><td>payload</td><td>reserved</td><td>protocol</td><td>txprot</td><td>rxprot</td><td>data</td></tr>
 *   <tr><th>  Value</th><td>0x0F</td><td>   0x07</td><td>    0x00</td><td>    0x01</td><td>0x0002</td><td>0x0001</td><td>0xFF</td></tr>
 * </table>
 * where:
 * <ul>
 *   <li> 0x01: command ID for callAntennaPower(), is also used as index in call_fkt_() table.
 *   <li> 0xFF: parameter for callAntennaPower(). 0xFF means turn antenna on.
 * </ul>
 * The reader answer with this report:
 * <table>
 *   <tr><th>   Byte</th><th>   0</th><th>      1</th><th>            2</th><th>       3</th><th>       4</th><th>          5</th><th>  6..7</th><th>   8</th></tr>
 *   <tr><th>Content</th><td> TID</td><td>payload</td><td>stream status</td><td>protocol</td><td>reserved</td><td>prot status</td><td>txprot</td><td>data</td></tr>
 *   <tr><th>  Value</th><td>0xF0</td><td>   0x06</td><td>         0x00</td><td>    0x01</td><td>    0x00</td><td>       0x00</td><td>0x0001</td><td>0x00</td></tr>
 * </table>
 * <ul>
 *   <li> 0x00 (txData[0]): the data this command produced.
 *   <li> 0x00 (status): #ERR_NONE
 * </ul>
 *
 * \subsection streamprotocolUART Protocol for UART:
 * A simple UART stream packet is shown in
 * the following table, which maps the transmitted data to the function parameters
 * of commands().
 * <table>
 *   <tr><th>     Byte</th><th>  0</th><th>      1</th><th>       2..3</th><th>       4</th><th>  5..6</th><th>  7..8</th><th>9..9+txprot-1</th></tr>
 *   <tr><th>  Content</th><td>TID</td><td>Status</td><td>payload</td><td>protocol</td><td>txprot</td><td>rxprot</td><td>data</td></tr>
 *   <tr><th>Parameter</th><td>   </td><td>       </td><td>        </td><td>protocol</td><td>rxSize</td><td>txSize</td><td>rxData[0..rxSize-1]</td></tr>
 * </table>
 * where
 * <ul>
 *   <li> \e TID : transaction ID.
 *   <li> \e Status : Uart Status
 *   <li> \e payload: amount of data in report.
 *   <li> \e protocol: defines which action should be triggered. If commands() is
 *      is called it refers to a firmware application command (no generic stream command).
 *   <li> \e txprot: amount of data which has been received and should be forwarded
 *      to command function.
 *   <li> \e rxprot: This variable contains the expected size of data this command
 *      will return and will be set to the size of the actual data to be tx-ed back
 *      to host.
 *   <li> \e data: input data for command
 * </ul>
 * The response will look like this:
 * <table>
 *   <tr><th>     Byte</th><th>  0</th><th>      1</th><th>       2..3</th><th>       4</th><th>       5</th><th>          6</th><th>  7..8</th><th>9..9+txprot-1</th></tr>
 *   <tr><th>  Content</th><td>TID</td><td>Status</td><td>payload</td><td>protocol</td><td>reserved</td><td>prot status</td><td>txprot</td><td>data</td></tr>
 *   <tr><th>Parameter</th><td>   </td><td>       </td><td>             </td><td>protocol</td><td>        </td><td>     status</td><td>txSize</td><td>txData[0..txSize-1]</td></tr>
 * </table>
 * where
 * <ul>
 *   <li> \e TID : transaction ID.
 *   <li> \e Status : Uart Status
 *   <li> \e payload: amount of data in report.
 *   <li> \e protocol : defines to which action the transmitted data belongs
 *   <li> \e prot status : a non-zero value indicates error condition of command
 *      (return value of function commands())
 *   <li> \e txprot: size of data in response
 *   <li> \e data: response data for command
 * </ul>
 *
 * <b>UART Settings:</b>
 * <ul>
 *   <li> BAUD RATE: 115200
 *   <li> DATA BITS: 8
 *   <li> PARITY: NONE
 *   <li> STOP BITS: 1
 *   <li> FLOW CONTROL: NONE
 * </ul>
 *
 * \subsubsection streamexampleConfigTXRX Example for the config TX RX command, handled by callConfigTxRx():
 * Request from host (Reader Suite):
 * <table>
     <tr><th>     Byte</th><th>  0 </th><th>     1</th><th>   2..3</th><th>       4</th><th>   5..6</th><th>   7..8</th><th>9..12</th></tr>
 *   <tr><th>  Content</th><td>TID </td><td>Status</td><td>payload</td><td>protocol</td><td>txprot </td><td>rxprot </td><td>data</td></tr>
 *   <tr><th>Parameter</th><td>0x20</td><td>0x00  </td><td>0x00 09</td><td>0x04    </td><td>0x00 04</td><td>0x00 04</td><td>0x00 00 01 01</td></tr>
 * </table>
 * where:
 * <ul>
 *   <li> 0x04: command ID for callConfigTxRx(), is also used as index in call_fkt_() table.
 *   <li> 0x00 00 01 01 : parameter for callConfigTxRx() (set_sensitivity = false and sensitivity = 0; set_antenna = true and antenna id = 1)
 * </ul>
 * The reader answer with this report:
 * <table>
 *  <tr><th>     Byte</th><th>   0</th><th>      1</th><th>  2..3</th><th>       4</th><th>       5</th><th>          6</th><th>  7..8 </th><th>9..12</th></tr>
 *  <tr><th>  Content</th><td>TID </td><td>Status</td><td>payload</td><td>protocol</td><td>reserved</td><td>prot status</td><td>txprot </td><td>data</td></tr>
 *  <tr><th>Parameter</th><td>0x01</td><td>0x00  </td><td>0x00 09</td><td>0x04    </td><td>0x00    </td><td>0x00       </td><td>0x00 04</td><td>0x00 0B 00 01</td></tr>
 * </table>
 * <ul>
 *  <li> 0x00 0B 00 01: reserved bytes are 0x00, sensitivity is 0x0B and antenna id is 0x01
 * </ul>
 *
 * \subsubsection streamexampleInventoryGen2 Example for the Inventory Gen2 command, handled by callInventoryGen2():
 * Request from host (Reader Suite):
 * <table>
 *   <tr><th>     Byte</th><th>  0 </th><th>     1</th><th>   2..3</th><th>       4</th><th>   5..6</th><th>   7..8</th><th>9..12</th></tr>
 *   <tr><th>  Content</th><td>TID </td><td>Status</td><td>payload</td><td>protocol</td><td>txprot </td><td>rxprot </td><td>data</td></tr>
 *   <tr><th>Parameter</th><td>0x07</td><td>0x00  </td><td>0x00 08</td><td>0x85    </td><td>0x00 03</td><td>0x00 00</td><td>0x00 01 06</td></tr>
 * </table>
 * where:
 * <ul>
 *   <li> 0x85: command ID for callInventoryGen2(), is also used as index in call_fkt_() table.
 *   <li> 0x00 01 06 : parameter for callInventoryGen2()
 * </ul>
 * The reader answer with this report:
 * <table>
 *  <tr><th>     Byte</th><th>   0</th><th>      1</th><th>  2..3</th><th>       4</th><th>       5</th><th>          6</th><th>  7..8 </th><th>9..12</th></tr>
 *  <tr><th>  Content</th><td>TID </td><td>Status</td><td>payload</td><td>protocol</td><td>reserved</td><td>prot status</td><td>txprot </td><td>data</td></tr>
 *  <tr><th>Parameter</th><td>0x76</td><td>0x00  </td><td>0x00 14</td><td>0x05    </td><td>0x00    </td><td>0x00       </td><td>0x00 0F</td><td>00 00 01 8f ff ac 3c 0d 06 10 00 ba ba 77 77</td></tr>
 * </table>
 * <ul>
 *   <li> 0xba ba 77 77: epc of tag
 * </ul>
 *
 *  \subsubsection streamexampleReadFromTag Example for the Read from Tag command, handled by callReadFromTag():
 * Request from host (Reader Suite):
 * <table>
 *   <tr><th>     Byte</th><th>  0 </th><th>     1</th><th>   2..3</th><th>       4</th><th>   5..6</th><th>   7..8</th><th>9..17</th></tr>
 *   <tr><th>  Content</th><td>TID </td><td>Status</td><td>payload</td><td>protocol</td><td>txprot </td><td>rxprot </td><td>data</td></tr>
 *   <tr><th>Parameter</th><td>0x3E</td><td>0x00  </td><td>0x00 0e</td><td>0x08    </td><td>0x00 09</td><td>0x00 09</td><td>0x00 00 00 00 00 00 00 00 00</td></tr>
 * </table>
 * where:
 * <ul>
 *   <li> 0x08: command ID for callReadFromTag(), is also used as index in call_fkt_() table.
 *   <li> 0x00 00 00 00 00 00 00 00 00 : parameter for callReadFromTag()
 * </ul>
 * The reader answer with this report:
 * <table>
 *  <tr><th>     Byte</th><th>   0</th><th>      1</th><th>  2..3</th><th>       4</th><th>       5</th><th>          6</th><th>  7..8 </th><th>9..16</th></tr>
 *  <tr><th>  Content</th><td>TID </td><td>Status</td><td>payload</td><td>protocol</td><td>reserved</td><td>prot status</td><td>txprot </td><td>data</td></tr>
 *  <tr><th>Parameter</th><td>0xE7</td><td>0x00  </td><td>0x00 0D</td><td>0x08    </td><td>0x00    </td><td>0x00       </td><td>0x00 08</td><td>00 00 00 00 55 55 55 55</td></tr>
 * </table>
 * <ul>
 *   <li> 00 00 00 00 55 55 55 55: reserved memory of the tag
 * </ul>
 * \section applcmds Implemented commands
 * Functions which handle the commands from the host are located in appl_commands.c
 * <ul>
 *  <li>  callAntennaPower()
 *  <li>  callAntennaTuner()
 *  <li>  callAutoTuner()
 *  <li>  callChangeFreq()
 *  <li>  callConfigGen2()
 *  <li>  callConfigTxRx()
 *  <li>  callInventoryGen2()
 *  <li>  callInventory6B()
 *  <li>  callReadFromTag6B()
 *  <li>  callKillTag()
 *  <li>  callLockUnlockTag()
 *  <li>  callReadFromTag()
 *  <li>  callReaderConfig()
 *  <li>  callSelectTag()
 *  <li>  callStartStop()
 *  <li>  callTunerTable()
 *  <li>  callWriteToTag()
 *  <li>  callWrongCommand()
 * </ul>
 * The documentation of the various appl command functions will only discuss the
 * data received/sent back to host and will not include the header information
 * for every transmitted packet.
 *
 * For more details on the ams stream protocol have a look into ams_stream.h
 */
u8 commands( u8 protocol, u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData )
{
    APPLOG("%hhxI\n", protocol);
    //if (rxSize == 0) return ERR_REQUEST;
    cyclicInventory = 0;    //stop cyclic inventory when new command has been received.
    
    if (protocol >= CALL_FKT_SIZE)
    {
        APPLOG("invalid command:ERR_REQUEST\n");
        return ERR_REQUEST;
    }
 
    /* setup pointers etc for usb command funtions, which use local buffers. */
    cmdBuffer.rxData = rxData;
    cmdBuffer.rxSize = rxSize;
    cmdBuffer.txData = txData;
    cmdBuffer.txSize = *txSize;
    cmdBuffer.result = ERR_REQUEST;
    if (rxSize < call_fkt_[protocol].min_rx_size)
    {
        APPLOG("commands(%hhx):ERR_REQUEST %hx < %hx\n",protocol,rxSize,call_fkt_[protocol].min_rx_size);
        return ERR_REQUEST;
    }
    /*if (*txSize < call_fkt_[protocol].min_tx_size)
    {
        APPLOG("commands(%hhx):ERR_PARAM %hx < %hx\n",protocol,*txSize, call_fkt_[protocol].min_tx_size);
        return ERR_PARAM;
    }*/
    
    // call the function associated with the command
    call_fkt_[protocol].func();
    
    *txSize = cmdBuffer.txSize;
    APPLOG("commands execution finished len:%hhx, replyError:%hhx\n\n", *txSize, cmdBuffer.result);
    return cmdBuffer.result;
}


/**This function sends the tag data received in the last inventory round to the host.
 * The payload description is here: callInventoryGen2()\n
 * @param protocol Protocol byte for the stream packet
 * @param txData Buffer to use for tx data
 * @param txSize Number of bytes which have been copied into the buffer.
 * @param remainingSize Number of available bytes in the tx buffer.
 * @return Status of the command.
 * \n\n
 * The fixed size part of a tag data reply payload (as described in callInventoryGen2()) consists of:
 *  <table>
 *    <tr><th> number of bytes </th><th>    content    </th></tr>
 *    <tr><td>  1   </td><td>cyclic</td></tr>
 *    <tr><td>  1   </td><td>tags_left</td></tr>
 *    <tr><td>  1   </td><td>tags_in_reply</td></tr>
 *    <tr><td>  1   </td><td>AGC and status (reg 0x2A)</td></tr>
 *    <tr><td>  1   </td><td>RSSI_value (reg 0x2B)</td></tr>
 *    <tr><td>  3   </td><td>base_freq</td></tr>
 *    <tr><td>  1   </td><td>epclen+pclen</td></tr>
 *    <tr><td>  2   </td><td>pc</td></tr>
 *    <tr><td>sum : 10</td><td></td></tr>
 *  </table>
 * followed by an epc with variable length.\n
 * If the parameter #read_TID_MMS_VFC_inInventoryRound is set to 1,
 * one byte with the tidlen and the tid with variable length will follow.\n
 *
 */
u8 sendTagData( u8 * protocol, u16 * txSize, u8 * txData, u16 remainingSize )
{
    static u8 element = 0;
    u16 offset;

    *txSize = 0;
    if (!tagDataAvailable)
    {
        return ERR_NONE;
    }
    if (remainingSize < SENDTAGMINLEN)
    {   //not enough buffer to send next tag data, wait for buffer to be sent.
        //APPLOG("send tag data buffer full, len: %hhx\n", *txSize);
        //APPLOGDUMP(txData, *txSize);
        return ERR_NONE;
    }

    APPLOG("send tag data, numTags: %hhx , elem: %hhx\n", num_of_tags, element);
    *protocol = CMD_INVENTORY_GEN2;
    txData[0] = cyclicInventory;
    if (num_of_tags == 0)
    {
        txData[1] = 0;
        txData[2] = 0;
        txData[3] = inventoryResult;
        txData[4] = Frequencies.freq[currentFreqIdx] & 0xff;
        txData[5] = (Frequencies.freq[currentFreqIdx] >>  8) & 0xff;
        txData[6] = (Frequencies.freq[currentFreqIdx] >> 16) & 0xff;
        *txSize = SENDNOTAGINFO;
        tagDataAvailable = 0;
        element = 0;
        return ERR_NONE;
    }
    *txSize = SENDTAGHEADER;
    txData[2] = 0;          //number of tags in reply
    offset = SENDTAGHEADER; //index in buffer where data for next tag can be put
    while (element < num_of_tags)
    {
        if (remainingSize < (*txSize + SENDTAGMINLEN + tags_[element].epclen))
        {   //not enough buffer to send next tag data, wait for buffer to be sent.
            //APPLOG("send tag data buffer full2, elem: %hhx , txSize: %hhx , offset: %hhx , remain: %hhx\n", element, *txSize, offset, remainingSize);
            //APPLOGDUMP(txData, *txSize);
            return ERR_NONE;
        }
        txData[1] = num_of_tags - element - 1;    //set number of tags left
        txData[2]++;    //increase number of tags in reply
        *txSize += SENDTAGFIXDATALEN + tags_[element].epclen;
        txData[offset+0] = tags_[element].agc;
        txData[offset+1] = tags_[element].rssi;
        txData[offset+2] = Frequencies.freq[currentFreqIdx] & 0xff;
        txData[offset+3] = (Frequencies.freq[currentFreqIdx] >>  8) & 0xff;
        txData[offset+4] = (Frequencies.freq[currentFreqIdx] >> 16) & 0xff;
        txData[offset+5] = tags_[element].epclen + 2;
        txData[offset+6] = tags_[element].pc[0];
        txData[offset+7] = tags_[element].pc[1];
        memcpy(&txData[offset+8], tags_[element].epc, tags_[element].epclen);

        if(read_TID_CAL_inInventoryRound)
        {
            txData[offset+8+tags_[element].epclen] = tags_[element].tidlength;
            memcpy(&txData[offset+8+tags_[element].epclen+1], tags_[element].tid, tags_[element].tidlength);
            offset += tags_[element].tidlength+1;
            *txSize += tags_[element].tidlength+1;
        }
        offset += SENDTAGFIXDATALEN + tags_[element].epclen;
        element++;
    }
    APPLOG("all tags sent, len: %hhx , elem: %hhx , numTags: %hhx\n", *txSize, element, num_of_tags);
    APPLOGDUMP(txData, *txSize);
    // all tags have been sent
    tagDataAvailable = 0;
    element = 0;
    return ERR_NONE;
}


/** This function changes the PA HW configuration and is executed when a stream
 * packet with protocol = #CMD_CALL_PA_ID is received.\n
 * This function is currently only available for Newton readers.
 * The payload of the stream packet looks like this:
 * <table>
 *   <tr><th>   Byte</th><th>     0       </th><th>     1      </th></tr>
 *   <tr><th>Content</th><td>ReadWriteMode</td><td>Config Word </td></tr>
 * </table>
 * \n
 * ReadWriteMode:\n
 * 0=> Read, 1=>Write and Read\n
 * ConfigWord:\n
 * Bit 0 (LSB) : Value for G8\n
 * Bit 1 (2LSB): Value for G16\n
 * 1 => High\n
 * 0 => Low\n
 * The reply of this command looks like this:
 * <table>
 *   <tr><th>   Byte</th><th>     1      </th></tr>
 *   <tr><th>Content</th><td>Config Word </td></tr>
 * </table>
 * \n
 *
 * Status of the reply packet indicates success or error.
 */
void callConfigPA(void)
{

#if NEWTON
    int rwMode = cmdBuffer.rxData[0];
    APPLOG("Config PA %hhx\n", cmdBuffer.rxData[0]);

    if (rwMode == 1)
    {
        _LATB5 = (cmdBuffer.rxData[1] & 0x01); //G8
        _LATB2 = (cmdBuffer.rxData[1] & 0x02) >> 1; //G16
    }
    cmdBuffer.txData[0] = _LATB2 << 1;
    cmdBuffer.txData[0] |= _LATB5;
    cmdBuffer.txSize = 1;
    cmdBuffer.result = ERR_NONE;

#else
    APPLOG("not available\n");
    cmdBuffer.result = ERR_PARAM;
#endif
}

/**This function reads the TID Memory of a specific Tag.\n
  *
  * The maximum length is limited by #TIDLENGTH.
  * @attention This command works on the one tag which is currently in the open
  *            state
  *
  * @param *tag Pointer to the Tag structure.
  * @return The function returns an errorcode.
                  0x00 means no error occoured.
                  0xFF unknown error occoured.
                  Any other value is the backscattered error code from the tag.
 */
void gen2FollowRead(Tag *tag, s8 *readErr)
{
    u8 wordCount = 4;
    u32 wordPtr = 0;

    APPLOG("read TAG=%hhx\n",tag->epc[0]);
    tag->tidlength=0;
    readErr[0] = gen2ReadFromTag(tag, MEM_TID, wordPtr, wordCount, tag->tid);
    if (readErr[0] == ERR_NONE)
    {
        APPLOG("first 4 Words correctly read");
        tag->tidlength = wordCount * 2;

        wordPtr = 4;
        wordCount = 1;
        while (wordPtr < (TIDLENGTH/2))
        {
            readErr[0] = gen2ReadFromTag(tag, MEM_TID, wordPtr, wordCount, (tag->tid + (wordPtr*2)));
            if (readErr[0] != ERR_NONE) {break;}
            wordPtr++;
            tag->tidlength += wordCount * 2;
        }
        if ( (readErr[0] != ERR_NONE) && !((readErr[0] == ERR_CHIP_HEADER) && (tag->tid[wordPtr*2])== 0x03))
        {
            tag->tidlength = 0x1;
            tag->tid[0] = 0xFD;
        }
    }
    else if ( (readErr[0] == ERR_CHIP_HEADER) && (tag->tid[0] == 0x03) )// Memory Overrun? TID shorter than 4 Words
    {
        APPLOG("ERROR Headerbit=%hhx\n", tag->tid[0]);
        wordCount = 1;
        while (wordPtr < (TIDLENGTH/2))
        {
            readErr[0] = gen2ReadFromTag(tag, MEM_TID, wordPtr, wordCount, (tag->tid + (wordPtr*2)));
            if (readErr[0] != ERR_NONE) {break;}
            wordPtr++;
            tag->tidlength += wordCount * 2;
        }
        if ( (readErr[0] != ERR_NONE) && !((readErr[0] == ERR_CHIP_HEADER) && (tag->tid[wordPtr*2])== 0x03))
        {
            tag->tidlength = 0x1;
            tag->tid[0] = 0xFE;
        }
    }
    else
    {
        tag->tidlength = 0x1;
        tag->tid[0] = 0xFF;
    }
    
    tag->cal[0] = 0;
    tag->cal[1] = 0;
    tag->cal[2] = 0;
    tag->cal[3] = 0;
    tag->cal[4] = 0;
    tag->cal[5] = 0;
    tag->cal[6] = 0;
    tag->cal[7] = 0;
    tag->temp[0] = 0;
    tag->temp[1] = 0;
    tag->mms[0] = 0;
    tag->mms[1] = 0;
    tag->vfc[0] = 0;
    tag->vfc[1] = 0;
    
    if((tag->tid[0] == 0xE2) && (tag->tid[1] == 0x82))
    {
       if((tag->tid[3] & 0x30) == 0x30) //magnus 3 tag
       {
           if(read_TID_CAL_inInventoryRound)
           {
                //Read CAL
                 wordCount = 4;
                 wordPtr = 0x08;
                 readErr[1] = gen2ReadFromTag(tag, MEM_USER, wordPtr, wordCount, tag->cal);
                 if(readErr[1] != ERR_NONE)
                 {
                     tag->cal[0] = 0xFF;
                     tag->cal[1] = 0xFF;
                     tag->cal[2] = 0xFF;
                     tag->cal[3] = 0xFF;
                     tag->cal[4] = 0xFF;
                     tag->cal[5] = 0xFF;
                     tag->cal[6] = 0xFF;
                     tag->cal[7] = 0xFF;
                 } 
           }
           
           if(read_MMS_VFC_TEMP_inInventoryRound)
           {
                //Read TEMP
                 wordCount = 1;
                 wordPtr = 0x0E;
                 readErr[2] = gen2ReadFromTag(tag, MEM_RES, wordPtr, wordCount, tag->temp);
                 if(readErr[2] != ERR_NONE)
                 {
                     tag->temp[0] = 0xFF;
                     tag->temp[1] = 0xFF;
                 }

                 wordPtr = 0x0C;

                //Read MMS
                 wordCount = 1;
                 readErr[3] = gen2ReadFromTag(tag, MEM_RES, wordPtr, wordCount, tag->mms);
                 if(readErr[3] != ERR_NONE)
                 {
                     tag->mms[0] = 0xFF;
                     tag->mms[1] = 0xFF;
                 }

                 //Read VFC
                 wordPtr = 0x0D;
                 wordCount = 1;
                 readErr[4] = gen2ReadFromTag(tag, MEM_RES, wordPtr, wordCount, tag->vfc);
                 if(readErr[4] != ERR_NONE)
                 {
                     tag->vfc[0] = 0xFF;
                     tag->vfc[1] = 0xFF;
                 }
           }
       } 
       else if((tag->tid[3] & 0x30) == 0x20) //magnus 2 tag
       {
           if(read_TID_CAL_inInventoryRound)
           {
                //Read CAL for moisture -- possible future expansion
                 /*wordCount = 4;
                 wordPtr = 0x08;
                 tag->cal[0] = 0;
                 tag->cal[1] = 0;
                 tag->cal[2] = 0;
                 tag->cal[3] = 0;
                 tag->cal[4] = 0;
                 tag->cal[5] = 0;
                 tag->cal[6] = 0;
                 tag->cal[7] = 0;
                 readErr[1] = gen2ReadFromTag(tag, MEM_USER, wordPtr, wordCount, tag->cal);
                 if(readErr[1] != ERR_NONE)
                 {
                     tag->cal[0] = 0xFF;
                     tag->cal[1] = 0xFF;
                     tag->cal[2] = 0xFF;
                     tag->cal[3] = 0xFF;
                     tag->cal[4] = 0xFF;
                     tag->cal[5] = 0xFF;
                     tag->cal[6] = 0xFF;
                     tag->cal[7] = 0xFF;
                 }*/ 
           }
           
           if(read_MMS_VFC_TEMP_inInventoryRound)
           {
                 wordPtr = 0x0B;

                //Read MMS
                wordCount = 1;
                readErr[3] = gen2ReadFromTag(tag, MEM_RES, wordPtr, wordCount, tag->mms);
                if(readErr[3] != ERR_NONE)
                {
                    tag->mms[0] = 0xFF;
                    tag->mms[1] = 0xFF;
                }
                 
                //Read VFC
                wordPtr = 0x0D;
                wordCount = 1;
                readErr[4] = gen2ReadFromTag(tag, MEM_RES, wordPtr, wordCount, tag->vfc);
                if(readErr[4] != ERR_NONE)
                {
                    tag->vfc[0] = 0xFF;
                    tag->vfc[1] = 0xFF;
                }
           }
       } 
    }
    return;
}


