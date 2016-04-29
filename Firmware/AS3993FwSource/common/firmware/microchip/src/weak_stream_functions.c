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
 *      PROJECT:   ASxxxx common firmware
 *      $Revision: $
 *      LANGUAGE:  Microchip C
 */

/*! \file
 *
 *  \author M. Arpa
 *
 *  \brief A dummy implementation of the WEAK functions needed by the
 *  stream_dispatcher.c
 *
 * to override any of the functions implemented here, just implement your 
 * own version without the attribute WEAK.
 */


/* --------- includes ------------------------------------------------------- */

#include "ams_types.h"
#include "ams_stream.h"
#include "logger.h"
#include "i2c_driver.h"
#include "spi_driver.h"
#include "stream_driver.h"
#include "stream_dispatcher.h"
#include "platform.h"

static u32 counter = 0;

/* --------- weak functions-------------------------------------------------- */

void WEAK i2cDeserialiseConfig ( i2cConfig_t * config, const u8 * data )
{
    INFO_LOG( "i2cDeserialiseConfig N/A\n" );
}

s8 WEAK i2cInitialize ( u32 sysClk, const i2cConfig_t* cfgDataIn, i2cConfig_t *cfgDataOut )
{
    INFO_LOG( "i2cInitialize N/A\n" );
    return AMS_STREAM_UNHANDLED_PROTOCOL;
}

s8 WEAK i2cRxTx ( u16 numberOfBytesToTx, const u8 * txData, u16 numberOfBytesToRx, u8 * rxData, BOOL sendStartCond, BOOL sendStopCond )
{
    INFO_LOG( "i2cRxTx N/A\n" );
    return AMS_STREAM_UNHANDLED_PROTOCOL;
}

void WEAK spiDeserialiseConfig ( spiConfig_t * config, const u8 * data )
{
    INFO_LOG( "spiDeserialiseConfig N/A\n" );
}

s8 WEAK spiInitialize ( const spiConfig_t* spiConfigIn, spiConfig_t* spiConfigOut )
{
    INFO_LOG( "spiInitialise N/A\n" );
    return AMS_STREAM_UNHANDLED_PROTOCOL;
}

s8 WEAK spiTxRx ( const u8 * txData, u8 * rxData, u16 numberOfBytes )
{
    INFO_LOG( "spiTxRx N/A\n" );
    return AMS_STREAM_UNHANDLED_PROTOCOL;
}

u8 WEAK applPeripheralReset ( )
{
    INFO_LOG( "applPeripheralReset N/A\n" );
    return AMS_STREAM_UNHANDLED_PROTOCOL;
}

const char * WEAK applFirmwareInformation ( )
{
    INFO_LOG( "applFirmwareInformation N/A\n" );
    return "applFirmwareInformation not supported";
}

u8 WEAK applProcessCmd ( u8 protocol, u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData )
{
    INFO_LOG( "applProcessCmd N/A\n" );
    return AMS_STREAM_UNHANDLED_PROTOCOL;
}

u8 WEAK applProcessCyclic ( u8 * protocol, u16 * txSize, u8 * txData, u16 remainingSize )
{
    if ( counter == 0 )
    { /* do not log this every time : is called cyclic */
	INFO_LOG( "applProcessCyclic N/A\n" );
    }
    counter++;
    *txSize = 0;
    return AMS_STREAM_NO_ERROR; /* cyclic is always called, so it is no error
                                   if there is no function */
}

u8 WEAK applReadReg ( u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData )
{
    INFO_LOG( "applReadReg N/A\n" );
    return AMS_STREAM_UNHANDLED_PROTOCOL;
}    

u8 WEAK applWriteReg ( u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData )
{
    INFO_LOG( "applWriteReg N/A\n" );
    return AMS_STREAM_UNHANDLED_PROTOCOL;
}    

void WEAK 	applSpiActivateSEN( u8 spiDeviceId )
{
    INFO_LOG( "applSpiActivateSEN N/A\n" );
}

void WEAK applSpiDeactivateSEN( u8 spiDeviceId )
{
    INFO_LOG( "applSpiDeactivateSEN N/A\n" );
}

void WEAK spiActivateSEN()
{
    INFO_LOG( "spiActivateSEN N/A\n" );
}

void WEAK spiDeactivateSEN()
{
    INFO_LOG( "spiActivateSEN N/A\n" );
}

void WEAK ioLedOn ( )
{
  /* implement here to switch a led on if you want to see it on whenever the PIC is communicating with
     the PC through the steaming interface */
    LED1(LEDON);
}

void WEAK ioLedOff ( )
{
  /* implement here to switch a led off if you want to see it on whenever the PIC is communicating with
     the PC through the steaming interface */
    LED1(LEDOFF);
}

