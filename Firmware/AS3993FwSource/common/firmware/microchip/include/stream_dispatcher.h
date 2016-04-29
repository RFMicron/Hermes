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
 *      PROJECT:   common firmware
 *      $Revision: $
 *      LANGUAGE:  C
 */

/*! \file
 *
 *  \author M. Arpa
 *
 *  \brief Interface for stream packet handling.
 *
 */


#ifndef STREAM_DISPATCHER_H
#define STREAM_DISPATCHER_H

/* ------------ includes ----------------------------------------- */

#include "ams_types.h"
#include "stream_driver.h"


/* ------------ constants ---------------------------------------- */

/*
 * The application should define its own firmware version. Please write
 * in your application file:
 *
 * E.g. for major 0x01, minor 0x03, releaseMarker 0x07
 * const u32 firmwareNumber = 0x010307UL;
 *
 * E.g. for major 0x02, minor 0x00, releaseMarker 0x18
 * const u32 firmwareNumber = 0x020018UL;
 */
extern const u32 firmwareNumber;



/* ------------ application functions ---------------------------- */

/*!
 *****************************************************************************
 *  \brief  reset peripherals of board
 *
 *  function which can be implemented by the application to reset
 *  the board peripherals.
 *  \return AMS_STREAM_UNHANDLED_PROTOCOL if function was not overloaded by application
 *****************************************************************************
 */
extern u8 applPeripheralReset( );

/*!
 *****************************************************************************
 *  \brief  get firmware information (zero-terminated) string
 *
 *  function which can be implemented by the application to return the
 *  firmware inforamtion string (zero-terminated). E.g. information about
 *  the chip and board.
 *  \return the a pointer to the firmware information
 *****************************************************************************
 */
extern const char * applFirmwareInformation( );

/*!
 *****************************************************************************
 *  \brief  Command dispatcher for application-specific commands
 *
 *  function which can be implemented by the application process application-
 *  specific commands for I/O. If data should be returned, the txData buffer
 *  can be filled with data which should be sent to the PC. In argument txSize,
 *  the size is returned.
 *  \param[in] protocol : the protocol byte which needs to be processed
 *  \param[in] rxData : pointer to payload for appl commands (in stream protocol buffer).
 *  \param[in] rxSize : size of rxData
 *  \param[out] txData : pointer to buffer to store returned data (payload only)
 *  \param[out] txSize : size of returned data
 *  \return the status byte to be interpreted by the stream layer on the host
 *****************************************************************************
 */
extern u8 applProcessCmd( u8 protocol, u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData );

/*!
 *****************************************************************************
 *  \brief  Called cyclic (even when no full usb packet was received). Use
 *          this is you need to send several packets (time delayed) in
 *          response to one usb request.
 *
 *  function which can be implemented by the application process
 *  If data should be returned, the txData buffer must be filled with the data to
 *  be sent to the PC. In argument txSize, the size is returned. The function
 *  also must fill in the protocol byte (this is the protocol value that is
 *  filled in in the protocol header.
 *  \param[out] protocol : protocol byte to be used for the packet header
 *  \param[out] txData : pointer to buffer to store returned data (payload only)
 *  \param[out] txSize : size of returned data
 *  \param[in]  remainingSize : how many bytes are free in the buffer txData
 *  \return the status byte to be interpreted by the stream layer on the host
 *****************************************************************************
 */
extern u8 applProcessCyclic( u8 * protocol, u16 * txSize, u8 * txData, u16 remainingSize );

/*!
 *****************************************************************************
 *  \brief  Generic function to read one or more registers
 *
 *  Function which can be implemented by the application to read registers.
 *  If data should be returned, the txData buffer must be filled with the data to
 *  be sent to the PC. In argument txSize, the size is returned. The function
 *  also must fill in the protocol byte (this is the protocol value that is
 *  filled in in the protocol header.
 *  \param[in] rxData : pointer to payload for appl commands (in stream protocol buffer).
 *  \param[in] rxSize : size of rxData
 *  \param[out] txData : pointer to buffer to store returned data (payload only)
 *  \param[out] txSize : size of returned data
 *  \return the status byte to be interpreted by the stream layer on the host
 *****************************************************************************
 */
extern u8 applReadReg( u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData );

/*!
 *****************************************************************************
 *  \brief  Generic function to write one or more registers
 *
 *  Function which can be implemented by the application to write registers.
 *  If data should be returned, the txData buffer must be filled with the data to
 *  be sent to the PC. In argument txSize, the size is returned. The function
 *  also must fill in the protocol byte (this is the protocol value that is
 *  filled in in the protocol header.
 *  \param[in] rxData : pointer to payload for appl commands (in stream protocol buffer).
 *  \param[in] rxSize : size of rxData
 *  \param[out] txData : pointer to buffer to store returned data (payload only)
 *  \param[out] txSize : size of returned data
 *  \return the status byte to be interpreted by the stream layer on the host
 *****************************************************************************
 */
extern u8 applWriteReg( u16 rxSize, const u8 * rxData, u16 * txSize, u8 * txData );


/* ------------ functions ---------------------------------------- */


/********************************************************************
 *  \brief returns the last error that occured and clears the error
 *  *******************************************************************/
u8 StreamDispatcherGetLastError( );

/********************************************************************
 *  \brief  initialization of stream dispatcher and connect to the
 *  communication stream (e.g. USB HID).
 *
 *  This function does all the necessary initialization for the Stream dispatcher.
 *  It also connects to the IO.
 *
 *  \param[in] sysClk : configured sysclk.
 *   Required to set correct transfer rates.
 *  *******************************************************************/
void StreamDispatcherInitAndConnect( u32 sysClk );

/********************************************************************
 *  \brief  initialisation of the stream dispatcher (no connect)
 *
 *  \param[in] sysClk : configured sysclk. Required to set correct spi/i2c data rates.
 *  *******************************************************************/
void StreamDispatcherInit( u32 sysClk );

/********************************************************************
 *  \brief  Main entry point into the stream dispatcher must be called cyclic.
 *
 *  This function checks the stream driver for received data. If new data is
 *  available it is processed and forwarded to the application
 *  functions.
 *  *******************************************************************/
void ProcessIO( );

#endif /* STREAM_DISPATCHER */
