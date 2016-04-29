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
 *  \brief spi driver declaration file
 *
 */
/*!
 *
 * The spi driver provides basic functionality for sending and receiving
 * data via SPI interface. All four common SPI modes are supported.
 * Moreover, only master mode and 8 bytes blocks are supported.
 *
 * API:
 * - Initialize SPI driver: #spiInitialize
 * - Deinitialize SPI driver: #spiDeinitialize
 * - Transmit data: #spiTxRx
 */

#ifndef SPI_H
#define SPI_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "Compiler.h"
#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/
#define SPI_MAX_INSTANCES 2
#define SPI1    0
#define SPI2    1

/*
******************************************************************************
* GLOBAL DATATYPES
******************************************************************************
*/
/*!
 * structure used to configure spi
 */
typedef struct
{
    u32 frequency;      /*!< desired spi frequency in Hz */
    u8 instance;        /*!< desired spi instance */
    u8 deviceId;        /*!< desired spi device id (selected with cs) */
    u8 clockPhase;      /*!< 0 or 1 - data centered on first or second edge of SCK period */
    u8 clockPolarity;   /*!< 0 or 1 - SCK low or high when idle */
} spiConfig_t;

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

/*!
 *****************************************************************************
 *  \brief  activate SEN for configured SPI device
 *
 *  Activate the SEN for the configured SPI device. Used by stream_disptcher.h.
 *
 *****************************************************************************
 */
extern void spiActivateSEN( );

/*!
 *****************************************************************************
 *  \brief  deactivate SEN for configured SPI device
 *
 *  Deactivate the SEN for the configured SPI device. Used stream_disptcher.h.
 *
 *****************************************************************************
 */
extern void spiDeactivateSEN( );

/*!
 *****************************************************************************
 *  \brief  activate SEN for desired SPI device
 *
 *  Function which has to be implemented by the application to
 *  activate the SEN for the desired SPI device when used by stream_disptcher.h.
 *
 *  \param[in] spiDeviceId: SPI device ID (given by USB application)
 *****************************************************************************
 */
extern void applSpiActivateSEN(u8 spiDeviceId);

/*!
 *****************************************************************************
 *  \brief  deactivate SEN for desired SPI device
 *
 *  Function which has to be implemented by the application to
 *  deactivate the SEN for the desired SPI device when used by stream_disptcher.h.
 *
 *  \param[in] spiDeviceId: SPI device ID (given by USB application)
 *****************************************************************************
 */
extern void applSpiDeactivateSEN(u8 spiDeviceId);

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

/*!
 *****************************************************************************
 *  \brief Read configuration in from a byte-stream (the byte stream must
 * have been written with spiSerialiseConfig. Used by stream_disptcher.h.
 *
 *  \param[out] config : structure to be filled with the data read in
 *  \param[in]  data : byte stream containing the data to be converted to the
 *  config struct
 *****************************************************************************
 */
void spiDeserialiseConfig( spiConfig_t * config, const u8 * data );

/*!
 *****************************************************************************
 *  \brief Write configuration to a byte-stream (the byte stream must
 * be read in with spiDeserialiseConfig. Used by stream_disptcher.h.
 *
 *  \param[in] config : structure to filled with the data to be written
 *  \param[out] data : byte stream to be filled with the data from the
 *  config struct
 *****************************************************************************
 */
void spiSerialiseConfig( const spiConfig_t * config, u8 * data );

/*!
 *****************************************************************************
 *  \brief  Initialize SPI interface
 *
 *  This function initializes the SPI interface, i.e. sets the requested
 *  frequency and SPI mode.
 *
 *  \param[in]  spiConfigIn  : structure holding configuration parameter (see #spiConfig_t)
 *  \param[out] spiConfigOut : structure returning previous configuration parameter
 *
 *  \return ERR_NONE : No error, SPI initialized.
 *
 *****************************************************************************
 */
extern s8 spiInitialize( const spiConfig_t* spiConfigIn, spiConfig_t* spiConfigOut);

/*!
 *****************************************************************************
 *  \brief  Deinitialize SPI interface
 *
 *  Calling this function deinitializes the SPI interface.
 *  SPI transfers are then not possible any more.
 *
 *  \return ERR_NONE : No error, SPI deinitialized.
 *
 *****************************************************************************
 */
extern s8 spiDeinitialize();

/*!
 *****************************************************************************
 *  \brief  Transfer a buffer of given length
 *
 *  This function is used to shift out \a length bytes of \a outbuf and
 *  write latched in data to \a inbuf.
 *  \note Due to the nature of SPI \a inbuf has to have the same size as
 *  \a outbuf, i.e. the number of bytes shifted out equals the number of
 *  bytes latched in again.
 *
 *  \param[in] txData: Buffer of size \a length to be transmitted.
 *  \param[out] rxData: Buffer of size \a length where received data is
 *              written to OR NULL in order to perform a write-only operation.
 *  \param[in] length: Number of bytes to be transfered.
 *
 *  \return ERR_IO : Error during SPI communication, returned data may be invalid.
 *  \return ERR_NONE : No error, all \a length bytes transmitted/received.
 *
 *****************************************************************************
 */
extern s8 spiTxRx(const u8* txData, u8* rxData, u16 length);

/*!
 *****************************************************************************
 *  \brief  Disables SPI interfaces
 *
 *  Calling this function before Sleep() backups the SPIEN bits of both SPI blocks
 *  and sets the SPIEN to 0.
 *
 *  Caution: Must be called before spiEnableAfterSleep() can be called successfully!!!
 *
 *  \return: n/a
 *
 *****************************************************************************
 */
extern void spiDisableBeforeSleep();

/*!
 *****************************************************************************
 *  \brief  Enables SPI interfaces
 *
 *  Calling this function after Sleep() restires the SPIEN bits of both SPI blocks
 *  which have been backuped by spiDisableBeforeSleep().
 *
 *  Caution: Must not be called before spiDisableBeforeSleep() has been called!!!
 *
 *  \return: n/a
 *
 *****************************************************************************
 */
extern void spiEnableAfterSleep();

#endif /* SPI_H */
