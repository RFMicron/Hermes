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
 *      PROJECT:   PIC24F streaming firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author Bernhard Breinbauer
 *
 *  \brief Streaming driver interface declarations.
 *  The defines allow switching between different stream drivers,
 *  currently implemented are:
 *  - USB   
 *  - UART  
 *
 */

/*!
 *
 *
 */
#ifndef STREAM_DRIVER_H
#define STREAM_DRIVER_H
/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "ams_types.h"
#include "Compiler.h"
#include "ams_stream.h"  /* stream protocol definitions */

/*
 ******************************************************************************
 * DEFINES
 ******************************************************************************
 */

/* set this flag to log all packets as hexdumps */
#define STREAM_DEBUG  0


/* redirect according to underlying communication protocol being usb-hid, uart */
#if USE_UART_STREAM_DRIVER

#define StreamInitialize       uartStreamInitialize
#define StreamConnect          uartStreamConnect
#define StreamDisconnect       uartStreamDisconnect
#define StreamReady            uartReady
#define UARTReady              uartReady
#define StreamHasAnotherPacket uartStreamHasAnotherPacket
#define StreamPacketProcessed  uartStreamPacketProcessed
#define StreamReceive          uartPacketReceive
#define packetReceive         uartPacketReceive
#define StreamTransmit         uartStreamTransmit


#else /* USE_USB_STREAM_DRIVER */

#define StreamInitialize       usbStreamInitialize
#define StreamConnect          usbStreamConnect
#define StreamDisconnect       usbStreamDisconnect
#define StreamReady            usbStreamReady
#define StreamHasAnotherPacket usbStreamHasAnotherPacket
#define StreamPacketProcessed  usbStreamPacketProcessed
#define StreamReceive          usbStreamReceive
#define StreamTransmit         usbStreamTransmit

#endif



/* ------------ functions ---------------------------------------- */


/*!
 *****************************************************************************
 *  \brief  Sets and clears an activity LED if function is implemented
 *
 *  This function is implemented as a weak function that does nothing. If you 
 *  want to drive a LED implment them as normal function (i.e. without
 *  the attribute WEAK). The weak implementation can be found in 
 *  file weak_functions.c.
 *****************************************************************************
 */
extern void ioLedOn( );
extern void ioLedOff( );



#endif /* STREAM_DRIVER_H */

