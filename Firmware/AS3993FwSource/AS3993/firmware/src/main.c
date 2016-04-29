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
  * @brief System initialization and main loop.
  *
  * @author Ulrich Herrmann
  * @author Bernhard Breinbauer
  */

/*
 * TODO: FERMI: use external oscillator, check configuration
 * NOTE: pic30-gcc (Gcc: 4.0.3; pic: 3.30) produces non-working (no epc reads) binary for AS3980 with -O2, changed to -O1
 */

#include "as3993_config.h"
#include "platform.h"
#include "stream_dispatcher.h"
#include "usb_hid_stream_driver.h"
#include "logger.h"
#include "uart_driver.h"
#include "errno.h"
#include "as3993_public.h"
#include "as3993.h"
#include "gen2.h"
#include "global.h"
#include "timer.h"
#include "appl_commands.h"
#include "tuner.h"
#if ISO6B
#include "iso6b.h"
#endif

extern Freq Frequencies;
extern Tag tags_[MAXTAG];

#if (SYSCLK == SYSCLK_16MHZ)
#if (FEMTO2 || FEMTO2_1)
_CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
_CONFIG2(POSCMOD_HS & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_OFF & FCKSM_CSDCMD & FNOSC_PRIPLL & PLL96MHZ_ON & PLLDIV_DIV3 & IESO_ON)
_CONFIG3(WPFP_WPFP0 & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM & SOSCSEL_IO)      // SOSCSEL_IO: get RA4 and RB4 as digital I/O
_CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)

#elif FERMI || RADON || NEWTON
_CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
_CONFIG2(POSCMOD_NONE & I2C1SEL_PRI & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLLDIV_NODIV & IESO_OFF)
_CONFIG3(WPFP_WPFP0 & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM & SOSCSEL_IO)      // SOSCSEL_IO: get RA4 and RB4 as digital I/O
_CONFIG4(DSWDTPS_DSWDTPS3 & DSWDTOSC_LPRC & RTCOSC_SOSC & DSBOREN_OFF & DSWDTEN_OFF)

#elif (__PIC24FJ256GB110__) //MEGA
_CONFIG1(WDTPS_PS1 & FWPSA_PR32 & WINDIS_OFF & FWDTEN_OFF & ICS_PGx1 & COE_OFF & BKBUG_OFF & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
_CONFIG2(POSCMOD_NONE & DISUVREG_OFF & IOL1WAY_OFF & OSCIOFNC_ON & FCKSM_CSDCMD & FNOSC_FRCPLL & PLL_96MHZ_ON & PLLDIV_NODIV & IESO_OFF)
_CONFIG3(WPFP_WPFP0 & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPENDMEM)

#endif

#endif

extern u16 readerInitStatus;
/** FW information which will be reported to host. */
static const char gAS3993FwInfo[] = FIRMWARE_ID"||"HARDWARE_ID;
/** FW information which will be logged on startup. Version information is included in logger. */
static const char gLogStartup[] = FIRMWARE_ID" %hhx.%hhx.%hhx on "HARDWARE_ID"\n";
/** FW version which will be reported to host */
const u32 firmwareNumber = FIRMWARE_VERSION;

void systemInit(void);

#if 0
static void calcBestSlotSize( void )
{
    u8 round;
    u8 powSlot;
    u8 numTags;

    for ( powSlot = 0; powSlot < 15 ; powSlot ++)
    {
        u8 maxTags=0;
        u16 tagsFound=0;
        for ( round = 0; round < 255 ; round ++)
        {
            numTags = gen2SearchForTags(tags_,16,0,0,powSlot,continueAlways,0,1,NULL);
            tagsFound+= numTags;
            if ( numTags > maxTags) maxTags = numTags;
        }
        LOG("found with powSlot=%hhx %x tags, max %hhx\n",
                                      powSlot,        tagsFound,maxTags);
    }
}
#endif

/** morse codes for hex numbers. */
static const u8 codes[0x10] = 
{
    /* upper 3 bits length, lower 5 bits shorts/longs */
    0xbf, /* 0 ----- */
    0xaf, /* 1 .---  */
    0xa7, /* 2 ..--- */
    0xa3, /* 3 ...-- */
    0xa1, /* 4 ....- */
    0xa0, /* 5 ..... */
    0xb0, /* 6 -.... */
    0xb8, /* 7 --... */
    0xbc, /* 8 ---.. */
    0xbe, /* 9 ----. */
    0x41, /* a .-    */
    0x88, /* b -...  */
    0x8a, /* c -.-.  */
    0x64, /* d -..   */
    0x20, /* e .     */
    0x82, /* f ..-.  */
};

/** This function will morse an error code on either the #MCULED() or on #LED1()*/
static void morse_nibble( u8 nibble )
{
    u8 i, cod, code_len;

    if (nibble > 0xf) nibble = 0x89; /* -..- = "x" */

    cod = codes[nibble];
    code_len = cod >> 5;

    for ( i = code_len; i>0; i-- )
    {
        if (NUMLEDS == 0)       // if there are no status LEDs use the mcu led for morsing
            MCULED(LEDON);
        else
            LED1(LEDON);
        if ( cod & (1<<(i-1)) ) 
        {
            LOG("-");
            delay_ms(450);
        }
        else
        {
            LOG(".");
            delay_ms(150);
        }
        if (NUMLEDS == 0)
            MCULED(LEDOFF);
        else
            LED1(LEDOFF);
        delay_ms(150);
    }
    LOG(" %hhx\n", nibble);
    delay_ms(450);
}

/**
 * This function displays error codes. If the number of LEDs available on the
 * board is sufficient, those LEDs will be used to show the error code binary
 * coded. If there are too few LEDs on the board the error code will be morsed
 * via morse_nibble();
 * @param errorCode The error code to display.
 * @param blinkState Indicates if the LEDs should be turned on or off (for blinking).
 */
static void showError( u8 errorCode, u8 blinkState )
{
    if (errorCode == 0)
    {
        MCULED(blinkState);
        return;
    }

    if (NUMLEDS < 2)
    {   // if we do not have enough LEDs to show error codes, morse them on LED1
        morse_nibble(errorCode);
        delay_ms(1000);
    }
    else
    {
        /*MCULED(errorCode & 0x01 ? blinkState : LEDOFF);
        LED1(errorCode & 0x02 ? blinkState : LEDOFF);
        LED2(errorCode & 0x04 ? blinkState : LEDOFF);
        LED3(errorCode & 0x08 ? blinkState : LEDOFF);*/
    }
}

#ifdef TUNER
void mainSenCIN(int select)
{
    if (select) 
        SEN_TUNER_CIN(1);
    else
        SEN_TUNER_CIN(0);

}
void mainSenCLEN(int select)
{
    if (select) 
        SEN_TUNER_CLEN(1);
    else
        SEN_TUNER_CLEN(0);

}
void mainSenCOUT(int select)
{
    if (select) 
        SEN_TUNER_COUT(1);
    else
        SEN_TUNER_COUT(0);

}
#if RADON
void ant1SenCIN(int select)
{
    if (select) 
        SEN_ANT1_CINPIN(1);
    else
        SEN_ANT1_CINPIN(0);

}
void ant1SenCOUT(int select)
{
    if (select) 
        SEN_ANT1_COUTPIN(1);
    else
        SEN_ANT1_COUTPIN(0);

}
#endif

TunerConfiguration mainTuner = 
{
#if TUNER_CONFIG_CIN
    mainSenCIN,
#else
    NULL,
#endif
#if TUNER_CONFIG_CLEN
    mainSenCLEN,
#else
    NULL,
#endif
#if TUNER_CONFIG_COUT
    mainSenCOUT
#else
    NULL,
#endif
};

#if RADON
TunerConfiguration ant1Tuner = 
{
    ant1SenCIN,
    NULL,
    ant1SenCOUT
};
#endif
#endif

/** main function
 * Initializes board, cpu, reader, host communication, ...\n
 * After intialization a loop which waits for commands from host
 * and performs cyclic inventory rounds is entered.
 */
int main(void)
{
    u32 counter = 0;
    BOOL ledBlinkState = 0;
    u32 baudrate, realrate;

    //char tempBuffer[1];
    //int size = 1;
    
    systemInit();
    timerInit();
    platformInit();
    spiInit();

    MCULED(LEDOFF);
    LED1(LEDOFF);
    LED2(LEDOFF);
    LED3(LEDOFF);
    LED4(LEDOFF);

    baudrate = BAUDRATE;
#if USE_UART_STREAM_DRIVER
    uartInitialize(SYSCLK, baudrate, &realrate);
    
    /*while(1) 
    {
        while(uartRxNumBytesAvailable() == 0);
        uartRxNBytes( tempBuffer, &size );
        uartTxByte(tempBuffer[0]);
        //delay_ms(10);
    }*/
    
    /*while(1)
    {
        while(U1STAbits.URXDA)
            temp = U1RXREG;
        while(U1STAbits.URXDA == 0);
        while(U1STAbits.UTXBF); 
        U1TXREG = 0x41; //U1RXREG;                 
        LED2(LEDOFF);
        LED1(LEDON);
        uartTxByte (0x41);
         delay_ms(100);
        LED1(LEDOFF);
    }*/
   
#else
    uartTxInitialize(SYSCLK, baudrate, &realrate);
#endif
    LOG("\nHello World\n");
    LOG(gLogStartup, (u8)((firmwareNumber >> 16) & 0xFF), (u8)((firmwareNumber >> 8) & 0xFF), (u8)(firmwareNumber & 0xFF));

    delay_ms(1);
    /* Fill frequencies interval used in usb_commands.c with values */
    /* European channels since we are developping in Europe, values can be changed using GUI */
    Frequencies.freq[0]= 866900;
    Frequencies.freq[1]= 865700;
    Frequencies.freq[2]= 866300;
    Frequencies.freq[3]= 867500;
    Frequencies.numFreqs=4;
    
    // For Newton USA Profile is used. Initilize with USA Frequency
#if NEWTON
    Frequencies.freq[0]= 902700;
#endif

    delay_ms(1);
    readerInitStatus = as3993Initialize(Frequencies.freq[0]);

#ifdef BUTTON
    while (!BUTTON);
#endif


    initCommands(); /* USB report commands */
#ifdef TUNER
    tunerInit(&mainTuner);
#if RADON
    tunerInit(&ant1Tuner);
#endif
#endif
    
    showError(readerInitStatus, ledBlinkState);

    LOG("initialize state: %x\n", readerInitStatus);

#if ISO6B
    iso6bInitialize();
#endif

    StreamDispatcherInit(SYSCLK);
#if !USE_UART_STREAM_DRIVER
    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    USBDeviceAttach();
#endif

    while (1)
    {
        if (counter)
        {
            counter--;
        }
        else
        {
            counter = (10000ULL<<(readerInitStatus?3:0));
            ledBlinkState = (~ledBlinkState) & 0x01;
            showError(readerInitStatus, ledBlinkState);
            if (readerInitStatus) 
            { /* If MCU is powered before AS3993 has power it is best to try to init again */
                readerInitStatus = as3993Initialize(Frequencies.freq[0]);
            }
        }
        ProcessIO(); /* main trigger for operation commands. */

        //if (doCyclicInventory()) /* do cyclic inventory if necessary.*/
        //{ /* if it was performed, then update blink state */
          //  ledBlinkState = (~ledBlinkState) & 0x01;
          //  showError(readerInitStatus, ledBlinkState);
        //}
#if !USE_UART_STREAM_DRIVER
#ifdef BUTTON
        if (! BUTTON)
        { /* If button is pressed then reset */
            USBDeviceDetach();
            USBSoftDetach();
            delay_ms(150);
            return 0; /* Returning from main will reset */
        }
#endif
#endif
    }
}

/**
 * Basic system initialization.
 */
void systemInit(void)
{
#if RUN_ON_AS3994
    _RCDIV = 0;     // FIXME: why is our clock to slow without divider?
#endif
}


u8 applPeripheralReset(void)
{
    as3993Reset();
    return ERR_NONE;
}

u8 applProcessCmd( u8 protocol, u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData )
{
    /*LOG("applProcessCmds(ptcl=%hhx, tx=%hhx, rx=%hhx)\n",
            rxData[0], rxSize, txSize);*/
    return commands(protocol, rxSize, rxData, txSize, txData );
}

u8 applProcessCyclic( u8 * protocol, u16 * txSize, u8 * txData, u16 remainingSize )
{
    return sendTagData( protocol, txSize, txData, remainingSize );
}

const char * applFirmwareInformation()
{
    return gAS3993FwInfo;
}


/*!This function reads one or all registers from the AS3993. See also applReadReg().\n
  The format of the report from the host is as follows:
  <table>
    <tr><th>   Byte</th><th>       0</th><th>       1</th></tr>
    <tr><th>Content</th><td>  mode  </td><td>reg_addr</td></tr>
  </table>
  Where mode = 0 requests to read all registers and mode = 1 requests to read only
  the register with address reg_addr. If mode = 0 reg_addr is ignored. \n
  If mode = 0 the device sends back:
  <table>
    <tr>
        <th>Byte</th>
        <th>0</th>
        <th>..</th>
        <th>29</th>
        <th>30</th>
        <th>31</th>
        <th>..</th>
        <th>36</th>
        <th>37</th>
        <th>38</th>
        <th>48</th>
    </tr>
    <tr>
        <th>Content</th>
        <td>reg 0x00</td>
        <td>..</td>
        <td>reg 0x1D</td>
        <td>reg 0x22</td>
        <td>reg 0x29</td>
        <td>..</td>
        <td>reg 0x2E</td>
        <td>reg 0x33</td>
        <td>reg 0x35</td>
        <td>..</td>
        <td>reg 0x3F</td>
    </tr>
  </table>
  If mode = 1 the device sends back:
  <table>
    <tr><th>   Byte</th><th>        0</th></tr>
    <tr><th>Content</th><td>reg value</td></tr>
  </table>
 
  returns ERR_NONE if operation was successful, ERR_PARAM is invalid mode was set.
*/
u8 cmdReadReg(u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData)
{
    u8 status = ERR_PARAM;
    //LOG("cmdReadReg: cmd:%hhx, rxsize:%hhx, txsize:%hhx\n", rxData[0], rxSize, *txSize);
    if (rxSize < READ_REG_RX_SIZE)
        return status;
    switch (rxData[0])
    {
        case 0:     //read all regs
            status = readRegisters(txSize, txData);
            break;
        case 1:     // read one reg
            status = readRegister(rxData[1], txSize, txData);
            break;
        default:
            status = ERR_PARAM;
    }
    return status;
}

u8 applReadReg( u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData )
{
    return cmdReadReg(rxSize, rxData, txSize, txData);
}

/*!This function writes one register on the AS3993. See also applWriteReg(). \n
  The format of the report from the host is as follows:
  <table>
    <tr><th>   Byte</th><th>       0</th><th>    1</th></tr>
    <tr><th>Content</th><td>reg_addr</td><td>value</td></tr>
  </table>
  if reg_addr > 0x80, then an immediate command is executed.\n
  The device sends back:
  <table>
    <tr><th>   Byte</th><th>0</th></tr>
    <tr><th>Content</th><td>0</td></tr>
  </table>

  returns ERR_NONE if operation was successful.
 */
u8 cmdWriteReg (u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData)
{
    //LOG("applWriteReg: addr:%hhx, val:%hhx, rxsize:%hhx, txsize:%hhx\n", rxData[0], rxData[1], rxSize, *txSize);
    if (rxSize < WRITE_REG_RX_SIZE)
        return ERR_PARAM;

    return writeRegister(rxData[0], rxData[1], txSize, txData);
}

u8 applWriteReg( u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData )
{
    return cmdWriteReg(rxSize, rxData, txSize, txData);
}

#if RUN_ON_AS3994 || __PIC24FJ256GB110__    // check for CPU: fix nightly build
/* TODO: AS3994 does not support bootloader yet. */
void enableBootloader()
{

}
#endif
