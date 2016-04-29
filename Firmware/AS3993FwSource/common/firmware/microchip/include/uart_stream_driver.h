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
 *      PROJECT:   UART PIC24F streaming firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author Bernhard Breinbauer
 *
 *  \brief UART streaming driver declarations.
 *
 *  UART has to be set up by the application. Use uartTxInitialize() and
 *  uartRxInitialize() from uart_driver.[hc]
 *
 */

/*!
 *
 *
 */
#ifndef _UART_STREAM_DRIVER_H
#define _UART_STREAM_DRIVER_H
/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "ams_types.h"
#include "ams_stream.h"
#include "Compiler.h"
//#include "stream_driver.h"
#include "uart_driver.h"

/*
 ******************************************************************************
 * GLOBAL PROTCOL I/O FUNCTIONS
 ******************************************************************************
 */

/*
 ******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************
 */


/*!
 *****************************************************************************
 *  \brief  initializes the UART Stream driver variables
 *
 *  This function takes care for proper initialisation of buffers, variables, etc.
 *
 *  \param rxBuf : buffer where received packets will be copied into
 *  \param txBuf : buffer where to be transmitted packets will be copied into
 *  \return n/a
 *****************************************************************************
 */
void uartStreamInitialize (u8 * rxBuf, u8 * txBuf);

/*!
 *****************************************************************************
 *  \brief  n/a in UART mode
 *
 *  \return n/a
 *****************************************************************************
 */
void uartStreamConnect (void);

/*!
 *****************************************************************************
 *  \brief  n/a in UART mode
 *
 *  \return n/a
 *****************************************************************************
 */
void uartStreamDisconnect (void);

/*!
 *****************************************************************************
 *  \brief  returns 1 if stream init is finished
 *
 *  \return 0=init has not finished yet, 1=stream has been initialized
 *****************************************************************************
 */
u8 uartStreamReady (void);

/*!
 *****************************************************************************
 *  \brief  returns 1 if stream init is finished
 *
 *  \return 0=init has not finished yet, 1=stream has been initialized
 *****************************************************************************
 */
u8 uartReady (void);

/*!
 *****************************************************************************
 *  \brief  tells the stream driver that the packet has been processed and can
 *   be moved from the rx buffer
 *
 *  \param rxed : number of bytes which have been processed
 *  \return n/a
 *****************************************************************************
 */
void uartStreamPacketProcessed (u8 rxed);

/*!
 *****************************************************************************
 *  \brief  returns 1 if another packet is available in buffer
 *
 *  \return 0=no packet available in buffer, 1=another packet available
 *****************************************************************************
 */
s8 uartStreamHasAnotherPacket (void);

/*!
 *****************************************************************************
 *  \brief checks if there is data received on UART from the host
 *  and copies the received data into a local buffer
 *
 *  Checks if the UART has data received from the host, copies this
 *  data into a local buffer. The data in the local buffer is than interpreted
 *  as a packet (with header, rx-length and tx-length). As soon as a full
 *  packet is received the function returns non-null.
 *
 *  \return 0 = nothing to process, >0 at least 1 packet to be processed
 *****************************************************************************
 */
u16 uartPacketReceive ();

/*!
 *****************************************************************************
 *  \brief checks if there is data to be transmitted from the device to
 *  the host.
 *
 *  Checks if there is data waiting to be transmitted to the host. Copies this
 *  data from a local buffer to the UART buffer and transmits this UART buffer.
 *
 *  \param [in] totalTxSize: the size of the data to be transmitted (the Stream
 *  driver header is not included)
 *****************************************************************************
 */
void uartStreamTransmit ( u16 totalTxSize );


u8 checkMsgCRC ( u16 msgLength, u16 msgCRC, u8 *msgBuffer );
 
#endif // _UART_STREAM_DRIVER_H

