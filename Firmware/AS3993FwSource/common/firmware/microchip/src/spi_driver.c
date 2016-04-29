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
/*
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author Wolfgang Reichart
 *
 *  \brief SPI driver for PIC24F.
 *
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#define USE_AND_OR
#include <p24Fxxxx.h>
#ifndef __PIC24FJ128GB202__
#include <spi.h>
#endif
#include <string.h>
#include "ams_types.h"
#include "GenericTypeDefs.h"
#include "errno.h"
#include "spi_driver.h"
#include "logger.h"
#include "system_clock.h"

/*
******************************************************************************
* LOCAL MACROS
******************************************************************************
*/
#define SPI_FIFO_DEPTH                  8

#define SPI_MAX_FREQ_CONF_TABLE_ENTRIES 5

#define spiReadStat() (*hw->stat)
#define spiReadCon1() (*hw->con1)
#define spiReadCon2() (*hw->con2)
#define spiReadBuf()  (*hw->buf)

#define spiWriteStat(A) *hw->stat = (A)
#define spiWriteCon1(A) *hw->con1 = (A)
#define spiWriteCon2(A) *hw->con2 = (A)
#define spiWriteBuf(A)  *hw->buf  = (A)

/*
******************************************************************************
* LOCAL DATATYPES
******************************************************************************
*/
typedef struct
{
    u32 systemFrequency;
    u32 frequency;
    u32 actFrequency;
#ifdef __PIC24FJ128GB202__
    u16 brgl;
#else
    u8 priPreScalerReg;
    u16 secPreScaler;
#endif
} spiFrequencyConfiguration_t;

struct spiHw
{
    volatile unsigned int  *stat;
    volatile unsigned int  *con1;
    volatile unsigned int  *con2;
    volatile unsigned int  *buf ;
};
/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
static umword spiTableIndex = 0;
static spiFrequencyConfiguration_t spiFreqConfTable[SPI_MAX_FREQ_CONF_TABLE_ENTRIES];

static const struct spiHw spiController[2] =
{
#ifdef __PIC24FJ128GB202__
    {
        .stat = &SPI1STATL,
        .con1 = &SPI1CON1,
        .con2 = &SPI1CON2,
        .buf  = &SPI1BUFL
    },
    {
        .stat = &SPI2STATL,
        .con1 = &SPI2CON1L,
        .con2 = &SPI2CON2L,
        .buf  = &SPI2BUFL
    }
#else
    {
        .stat = &SPI1STAT,
        .con1 = &SPI1CON1,
        .con2 = &SPI1CON2,
        .buf  = &SPI1BUF
    },
    {
        .stat = &SPI2STAT,
        .con1 = &SPI2CON1,
        .con2 = &SPI2CON2,
        .buf  = &SPI2BUF
    }
#endif
};
/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
static spiConfig_t myCfgData;
/* PIC24FJ64GB004 Family Silicon Errata: point 7 */
static unsigned char spienbits[2];

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/
/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/



void spiSerialiseConfig( const spiConfig_t * config, u8 * data )
{
    data[0] = config->frequency >> 24;
    data[1] = config->frequency >> 16;
    data[2] = config->frequency >> 8;
    data[3] = config->frequency;
    data[4] = config->instance;
    data[5] = config->deviceId;
    data[6] = config->clockPhase;
    data[7] = config->clockPolarity;
}

void spiDeserialiseConfig( spiConfig_t * config, const u8 * data )
{
    config->frequency = (((u32) data[0]) << 24) | (((u32) data[1]) << 16) | (((u32) data[2]) << 8) | (((u32) data[3]));
    config->instance = data[4];
    config->deviceId = data[5];
    config->clockPhase = data[6];
    config->clockPolarity = data[7];
}

s8 spiInitialize( const spiConfig_t* const spiConfigIn, spiConfig_t *spiConfigOut)
{
    u32 systemClock = systemClockGetFcy(); /* mar 2013/12/04: we wanted the signature of the initialise
                              function to be changed, so I removed the clock as an
                              input parameter */
#ifdef __PIC24FJ128GB202__
    u32 brgl;
#else
    u16 priprescaler;
    u8 priprescalerreg;
    u16 secprescaler;
#endif
    u32 actFrequency;
    spiFrequencyConfiguration_t* tblptr;
    umword i;
    BOOL configFound = FALSE;
    const struct spiHw *hw;

    if ( spiConfigIn == NULL || spiConfigIn->instance >= SPI_MAX_INSTANCES )
    {
        /* avoid severe pointer-arithmetik errors */
        return ERR_PARAM;
    }

    if (spiConfigOut != NULL)
    {
        spiConfigOut->clockPhase    = myCfgData.clockPhase;
        spiConfigOut->clockPolarity = myCfgData.clockPolarity;
        spiConfigOut->deviceId      = myCfgData.deviceId;
        spiConfigOut->frequency     = myCfgData.frequency;
        spiConfigOut->instance      = myCfgData.instance;
    }
    myCfgData.clockPhase    = spiConfigIn->clockPhase;
    myCfgData.clockPolarity = spiConfigIn->clockPolarity;
    myCfgData.deviceId      = spiConfigIn->deviceId;
    myCfgData.frequency     = spiConfigIn->frequency;
    myCfgData.instance      = spiConfigIn->instance;
    hw = spiController + myCfgData.instance;

    /* disable SPI for configuration */
    spiWriteStat(0);

    /* first check the cache table for already calculated frequencies */
    for (i = 0; i < SPI_MAX_FREQ_CONF_TABLE_ENTRIES; i++)
    {
        if ((spiFreqConfTable[i].frequency == myCfgData.frequency) && (spiFreqConfTable[i].systemFrequency == systemClock))
        {
            /* found a previously configured setting */
            configFound = TRUE;
            tblptr = &spiFreqConfTable[i];
            break;
        }
    }

    /* configure SPI frequency */
#ifdef __PIC24FJ128GB202__
    if (!configFound)
    {
        /* equation according to the datasheet for calculating spi clock speed:
         * fsck = FPB / (2 * (BRG + 1))
         */
        brgl = ((systemClock*5)/myCfgData.frequency)-10;
        /* round to next integer if >= 0.5*/
        brgl = (brgl%10>=5) ? ((brgl/10)+1) : (brgl/10);
        /* mind brgl limits*/
        brgl = (brgl > 0x1FFF) ? 0x1FFF : brgl;

        actFrequency = systemClock / (2*(brgl+1));

        /* save this new configuration in the cache table */
        if (spiTableIndex >= SPI_MAX_FREQ_CONF_TABLE_ENTRIES)
        {
            spiTableIndex = 0;
        }
        spiFreqConfTable[spiTableIndex].actFrequency = actFrequency;
        spiFreqConfTable[spiTableIndex].frequency = myCfgData.frequency;
        spiFreqConfTable[spiTableIndex].brgl = brgl;
        spiFreqConfTable[spiTableIndex].systemFrequency = systemClock;
        spiTableIndex++;
    }
    else
    {
        actFrequency = tblptr->actFrequency;
        myCfgData.frequency = tblptr->frequency;
        brgl = tblptr->brgl;
    }

    if(myCfgData.instance == SPI1)
        SPI1BRGL = brgl;
    else
        SPI2BRGL = brgl;
#else
    if (!configFound)
    {
        /* equation according to the datasheet for calculating spi clock speed:
         * fsck = SYSCLK / (pri_prescaler * sec_prescaler)
         */
        priprescaler = 1;
        priprescalerreg = 3;
        do
        {
            secprescaler = systemClock / (myCfgData.frequency * priprescaler);
            if (secprescaler > 8)
            {
                if (priprescaler >= 64)
                {
                    secprescaler = 8;
                    break;
                }
                else
                {
                    priprescaler <<= 2;
                    priprescalerreg--;
                    continue;
                }
            }
            else
            {
                break;
            }

        }
        while(1);

        secprescaler = (secprescaler == 0) ? 1 : secprescaler;
        actFrequency = systemClock / (priprescaler * secprescaler);

        /* save this new configuration in the cache table */
        if (spiTableIndex >= SPI_MAX_FREQ_CONF_TABLE_ENTRIES)
        {
            spiTableIndex = 0;
        }
        spiFreqConfTable[spiTableIndex].actFrequency = actFrequency;
        spiFreqConfTable[spiTableIndex].frequency = myCfgData.frequency;
        spiFreqConfTable[spiTableIndex].priPreScalerReg = priprescalerreg;
        spiFreqConfTable[spiTableIndex].secPreScaler = secprescaler;
        spiFreqConfTable[spiTableIndex].systemFrequency = systemClock;
        spiTableIndex++;
    }
    else
    {
        actFrequency = tblptr->actFrequency;
        myCfgData.frequency = tblptr->frequency;
        priprescalerreg = tblptr->priPreScalerReg;
        secprescaler = tblptr->secPreScaler;
    }

    /* set pre-scaler */
    spiWriteCon1(priprescalerreg | ((8 - secprescaler) << 2));
#endif

    /* set phase and polarity: */
    /* mar 2013/11/29:
     * PIC has not phase and polarity but idle-state and transmit on transition
     * from idle to active or transmit on transition from active to idle.
     * So the following tables are valid:
     *
   polarity | phase                                                        idle-state-is     | tx-active-to-idle
   ---------+------                                                     ---------------------+------------------
      0     | 0     = capture on rising edge, provide on falling edge =          0 (low)     |     1             = tx- active-to-idle
      0     | 1     = capture on falling edge, provide on rising edge =          0 (low)     |     0             = tx- idle-to-active
      1     | 0     = capture on falling edge, provide on rising edge =          1 (high)    |     1             = tx- active-to-idle
      1     | 1     = capture on rising edge, provide on falling edge =          1 (high)    |     0             = tx- idle-to-active

     *  So idle-state is the same as polarity, but phase is the opposite of CKE (clock edge select bit) which is
     *  basically transmit-on-transition from active to idle.
     */
    spiWriteCon1(spiReadCon1() |
                 ((myCfgData.clockPhase ? 0 : 1) << 8) |
                 (myCfgData.clockPolarity << 6));

    /* set enhanced buffer master mode and enable SPI */
#ifdef __PIC24FJ128GB202__
    spiWriteCon1(spiReadCon1() | (1 << 15) | (1 << 5) | 1);
#else
    spiWriteCon2(1);
    spiWriteCon1(spiReadCon1() | (1 << 5));
    spiWriteStat((1 << 15));
#endif

    SPI_LOG( "\nSPI M=%hhx, des.f=%u%u, act.f=%u%u, clkPhase=%hhx, clkPol=%hhx, ss=%hhx\n"
             , myCfgData.instance
             , (u16)(myCfgData.frequency >> 16)
             , (u16)(myCfgData.frequency)
             , (u16)(actFrequency >> 16)
             , (u16)(actFrequency)
             , myCfgData.clockPhase
             , myCfgData.clockPolarity
             , myCfgData.deviceId
           );

    return ERR_NONE;
}

s8 spiDeinitialize()
{
    const struct spiHw *hw = spiController + myCfgData.instance;
    u8 tmp;

    /* clear receive buffer */
    if (spiReadStat() & 1)
    {
        tmp = spiReadBuf();
    }
    /* clear RX FIFO when SRXMPT bit is set (only valid in enhancled buffer mode) */
    while (!(spiReadStat() & (1 << 5)))
    {
        tmp = spiReadBuf();
    }
    /* clear receive overflow flag as it might be set */
    spiWriteStat(spiReadStat() & ~((u16)(1 << 6)));
    spiWriteStat(0);
    spiWriteCon1(0);
    spiWriteCon2(0);

    return ERR_NONE;
}

s8 spiTxRx(const u8* txData, u8* rxData, u16 length)
{
    const struct spiHw *hw = spiController + myCfgData.instance;
    u16 i = 0;

    if (length == 0) return 0;

#ifdef __PIC24FJ128GB202__
    // Wait if buffer is full
    //while(SPI1STATLbits.SPITBF);
#endif
    SPI_LOG("SPI Write:");

    /* Write one data before proceeding, this is possible because we have a FIFO */
    /* This could also be changed to a more efficient scheme always looking
       at water levels. But this only makes sense at very high SPI speeds,
       improvement for future */
    if (txData != NULL)
    {
        SPI_LOG(" %hhx", txData[i]);
        spiWriteBuf(txData[i]);
    }
    else
    {
        SPI_LOG(" %hhx", 0);
        spiWriteBuf(0);
    }
    i++;

    /* Write the next byte, then receive the previous byte */
    while (i < length)
    {
#ifdef __PIC24FJ128GB202__
        // Wait if buffer is full
        //while(SPI1STATLbits.SPITBF);
#endif
        if (txData != NULL)
        {
            SPI_LOG(" %hhx", txData[i]);
            spiWriteBuf(txData[i]);
        }
        else
        {
            SPI_LOG(" %hhx", 0);
            spiWriteBuf(0);
        }
        i++;

#ifdef __PIC24FJ128GB202__
        // Wait if receive buffer is empty
        while(SPI1STATLbits.SPIRBE);
#else
        while (spiReadStat() & 0x20);
#endif

        if ((rxData != NULL))
        {
            rxData[i - 2] = spiReadBuf();
        }
        else
        {
            spiReadBuf();
        }
    }

#ifdef __PIC24FJ128GB202__
    // Wait if receive buffer is empty
    while(SPI1STATLbits.SPIRBE);
#else
    while (spiReadStat() & 0x20);
#endif

    /* Receive the last byte, FIFO should be empty for both directions */
    if ((rxData != NULL))
    {
        rxData[i - 1] = spiReadBuf();
    }
    else
    {
        spiReadBuf();
    }

#if USE_LOGGER
    if (rxData)
    {
        SPI_LOG(" ->");

        for (i = 0; i < length; i++)
        {
            SPI_LOG(" %hhx",rxData[i]);
        }
    }

    SPI_LOG("\n");
#endif
//    if (spiReadCon2() & 0x01)
//    {
//        // Enhanced Buffer Mode -> wait for pending transfers
//        while(SPI1STATbits.SPIBEC);
//    }
    return ERR_NONE;
}

void WEAK spiActivateSEN()
{
    applSpiActivateSEN(myCfgData.deviceId);
}

void WEAK spiDeactivateSEN()
{
    applSpiDeactivateSEN(myCfgData.deviceId);
}

void spiDisableBeforeSleep()
{
    // wait for pending transfers to be finished
#ifdef __PIC24FJ128GB202__
    while(SPI1STATHbits.RXELM);
    while(SPI1STATHbits.TXELM);
    while(SPI2STATHbits.RXELM);
    while(SPI2STATHbits.TXELM);

    // backup SPIEN flags for SPI1 and SPI2
    spienbits[0] = SPI1CON1Lbits.SPIEN;
    if(SPI2CON2L&0x8000)
        spienbits[1]=1;
    else
        spienbits[1]=0;
    // Disable both SPI blocks
    SPI1CON1Lbits.SPIEN = 0;
    SPI2CON1L = (SPI2CON1L&0x7FFF);
#else
    while(SPI1STATbits.SPIBEC);
    while(SPI2STATbits.SPIBEC);
    // backup SPIEN flags for SPI1 and SPI2
    spienbits[0] = SPI1STATbits.SPIEN;
    spienbits[1] = SPI2STATbits.SPIEN;
    // Disable both SPI blocks
    SPI1STATbits.SPIEN = 0;
    SPI2STATbits.SPIEN = 0;
#endif
}

void spiEnableAfterSleep()
{
    // Restore SPIEN flags for both blocks
#ifdef __PIC24FJ128GB202__
    SPI1CON1Lbits.SPIEN = 1;//spienbits[0];
    SPI2CON1L = ((SPI2CON1L&0x7FFF) | (spienbits[1]<<15));
#else
    SPI1STATbits.SPIEN = spienbits[0];
    SPI2STATbits.SPIEN = spienbits[1];
#endif
}
