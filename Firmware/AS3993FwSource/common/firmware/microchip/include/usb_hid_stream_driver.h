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
 *      PROJECT:   USB PIC24F HID streaming firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author Wolfgang Reichart
 *
 *  \brief USB HID streaming driver declarations.
 *
 */

/*!
 *
 *
 */
#ifndef _USB_HID_STREAM_DRIVER_H
#define _USB_HID_STREAM_DRIVER_H

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "ams_types.h"
#include "ams_stream.h"
#include "Compiler.h"
#include "stream_driver.h"
#include "usb.h"
#include "usb_function_hid.h"

/*
 ******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************
 */


/*!
 *****************************************************************************
 *  \brief  initializes the USB HID Stream driver variables
 *
 *  This function takes care for proper initialisation of buffers, variables, etc.
 *
 *  \param rxBuf : buffer where received packets will be copied into
 *  \param txBuf : buffer where to be transmitted packets will be copied into
 *****************************************************************************
 */
void usbStreamInitialize (u8 * rxBuf, u8 * txBuf);

/*!
 *****************************************************************************
 *  \brief  initialises the usb endpoints and connects to host
 *****************************************************************************
 */
void usbStreamConnect();

/*!
 *****************************************************************************
 *  \brief  disconnects from usb host
 *****************************************************************************
 */
void usbStreamDisconnect();

/*!
 *****************************************************************************
 *  \brief  returns 1 if stream init is finished
 *
 *  \return 0=init has not finished yet, 1=stream has been initialized
 *****************************************************************************
 */
u8 usbStreamReady();

/*!
 *****************************************************************************
 *  \brief  tells the stream driver that the packet has been processed and can
 *   be moved from the rx buffer
 *
 *  \param rxed : number of bytes which have been processed
 *****************************************************************************
 */
void usbStreamPacketProcessed (u16 rxed);

/*!
 *****************************************************************************
 *  \brief  returns 1 if another packet is available in buffer
 *
 *  \return 0=no packet available in buffer, 1=another packet available
 *****************************************************************************
 */
s8 usbStreamHasAnotherPacket (void);

/*!
 *****************************************************************************
 *  \brief checks if there is data received on the HID device from the host
 *  and copies the received data into a local buffer
 *
 *  Checks if the usb HID device has data received from the host, copies this
 *  data into a local buffer. The data in the local buffer is than interpreted
 *  as a packet (with header, rx-length and tx-length). As soon as a full
 *  packet is received the function returns non-null.
 *
 *  \return 0 = nothing to process, >0 at least 1 packet to be processed
 *****************************************************************************
 */
u16 usbStreamReceive ();

/*!
 *****************************************************************************
 *  \brief checks if there is data to be transmitted from the HID device to
 *  the host.
 *
 *  Checks if there is data waiting to be transmitted to the host. Copies this
 *  data from a local buffer to the usb buffer and transmits this usb buffer.
 *  If more than 1 usb hid report is needed to transmit the data, the function
 *  waits until the first one is sent, and than refills the usb buffer with
 *  the next chunk of data and transmits the usb buffer again. And so on, until
 *  all data is sent.
 *
 *  \param [in] totalTxSize: the size of the data to be transmitted (the HID
 *  header is not included)
 *****************************************************************************
 */
void usbStreamTransmit( u16 totalTxSize );


//FIXME mar was ist das????
extern WEAK void applUSBCBHandler(USB_EVENT event, void *pdata, WORD size);

#endif // _USB_HID_STREAM_DRIVER_H

