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
 *  \brief UART streaming driver implementation.
 *
 */

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "ams_types.h"
#include "errno.h"
#include "ams_stream.h"
#include "GenericTypeDefs.h"
#include "stream_driver.h"
#include "uart_stream_driver.h"
#include "stream_dispatcher.h"
#include "logger.h"
#include <stdio.h>
#include "crc16.h"


/*
 ******************************************************************************
 * VARIABLES
 ******************************************************************************
 */
static u8 * rxBuffer;   /* hook to rx buffer provided by uartStreamInitialize */
static u8 * txBuffer;   /* hook to tx buffer provided by uartStreamInitialize */

static u8 txTid;   /* tid for transmit */
static u8 rxTid;   /* tid received */

static u8 rxMsgType;   /* message type received */

static u16 rxPayloadSize; /* the payload size read out from the rx- uart header */
static u16 rxMsgLength; /* the message length read out from the rx- uart header */
//static u16 rxMsgCheckSum; /* the message checksum read out from the rx- uart header */
static u16 rxSize;          /* so far received payload bytes */
static u8 * rxEnd; /* pointer to next position where to copy the received data */
static u8 rxUartHeaderSize;    /* number of so far received bytes of the uart header */

static u8 oldRequestFormat; /* check if host is using stream v1 version */

//u8 rxUartHeaderBuffer[ UART_HEADER_SIZE ]; /* small buffer to hold the rxed uart header */
static u8 txUartHeaderBuffer[ UART_HEADER_SIZE ]; /* small buffer to hold the transmit uart header */


/*
 ******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************
 */

void uartStreamInitialize ( u8 * rxBuf, u8 * txBuf )
{
    /* setup pointers to buffers */
    rxBuffer = rxBuf;
    txBuffer = txBuf;
    /* so far we have received nothing */
    rxUartHeaderSize = 0; /* no bytes of header received */
    rxPayloadSize = 0; /* no payload received */
    txTid = 0;
    rxTid = 0;
    rxSize = 0; /* no data so far */
    rxEnd = rxBuffer;
}

void uartStreamConnect (void)
{
}

void uartStreamDisconnect (void)
{
}

u8 uartStreamReady (void)
{
    return 1;
}

u8 uartReady (void)
{
    return 1;
}

void uartStreamPacketProcessed ( u8 rxed )
{
    /*
     * FIXME: knowing that a memmove() is not the most performant variant,
     * it is used here to keep the code simpler.
     * If we run into performance issues on the PIC, this can be replaced
     * by pointer operations.
     */
    rxed += AMS_STREAM_HEADER_SIZE;
    /* decrease remaining data length by length of consumed packet */
    rxSize -= rxed;

    memmove( rxBuffer, rxBuffer + rxed, rxSize );
    /* correct end pointer to the new end of the buffer */
    rxEnd = rxBuffer + rxSize;
}

s8 uartStreamHasAnotherPacket ( )
{
    return (  rxSize >= AMS_STREAM_HEADER_SIZE
           && rxSize >= ( AMS_STREAM_DR_GET_RX_LENGTH( rxBuffer ) + AMS_STREAM_HEADER_SIZE )
           );
}

u16 uartStreamOldFormatRequest ( )
{
    u8 protocol = rxBuffer[ 0 ];
    u8 toTx = rxBuffer[ 1 ];
    
    /* send back the special answer */
    if ( toTx > 0 || ( protocol & 0x40 ) ) /* response was required */
    {
        txUartHeaderBuffer[ 0 ] = AMS_STREAM_COMPATIBILITY_TID;
        txUartHeaderBuffer[ 1 ] = 0x03; /* payload */
        uartTxNBytes( txUartHeaderBuffer, 2 );
	txUartHeaderBuffer[ 0 ] = protocol; 
	txUartHeaderBuffer[ 1 ] = 0xFF; /* status = failed -> wrong protocol version */
	txUartHeaderBuffer[ 2 ] = 0x00; /* no data will be sent back */
        uartTxNBytes( txUartHeaderBuffer, 3 );
    }

    oldRequestFormat = 0;
    rxPayloadSize = 0;
    rxSize        = 0; /* no payload received so far */
    rxEnd         = rxBuffer; /* we start a new uart packet */
    return 0;
}

  

u16 uartPacketReceive ( )
{
    if ( uartRxNumBytesAvailable() > 0 )	
    { /* data received from host */
            
        u16 uartRxBytes;
	u16 missingHeaderBytes = UART_HEADER_SIZE - rxUartHeaderSize;

	if ( missingHeaderBytes > 0 ) /* we still need to receive the header */
        {
	    ioLedOn();
	    uartRxNBytes( rxBuffer + rxUartHeaderSize, &missingHeaderBytes );
	    rxUartHeaderSize += missingHeaderBytes;
	    ioLedOff( );
	    if ( rxUartHeaderSize < UART_HEADER_SIZE )
	    {
		return 0; /* continue to wait for data for header */
	    }

	    /* if we get here the uart header is totally received -> interpret it */

	    //if ( UART_STATUS( rxUartHeaderBuffer ) != 0 ) /* this is a request in the old format */
	    //{ /* in the old format at this position we had the payload size - which was never 0 
        //         in the new format here this byte is resered and 0 when sent from host to device */
 	    //    oldRequestFormat = 1;
	    //    rxPayloadSize = UART_STATUS( rxUartHeaderBuffer );
        //        /* The old format had only a 2 bytes header, copy the additional 2 bytes
        //           into the payload buffer and set up rxSize. */
        //        memcpy(rxBuffer, rxUartHeaderBuffer + 2, UART_HEADER_SIZE - 2);
        //        rxEnd = rxBuffer + 2;
        //        rxSize = UART_HEADER_SIZE - 2;
	    //}
	    //else
	    //{
  	    oldRequestFormat = 0;
        rxMsgLength = UART_GET_MESSAGE_LENGTH( rxBuffer );
                rxSize        = 0; /* no payload received so far */
                rxEnd         = &rxBuffer[6]; /* start receiving payload */
	    //}
        rxMsgType         = UART_GET_MESSAGE_TYPE( rxBuffer );
	}

	/* if we get here we receive payload (header totally received) */
        ioLedOn();
	uartRxBytes = rxMsgLength - UART_HEADER_SIZE - rxSize;
	/* add the new data at the end of the data in the rxBuffer (i.e. where rxEnd points to) */
        uartRxNBytes( rxEnd, &uartRxBytes );
        rxSize += uartRxBytes;      /* adjust number of totally received bytes by number of actually received bytes*/
	rxEnd  += uartRxBytes;

	ioLedOff( );

    rxPayloadSize = rxMsgLength - UART_HEADER_SIZE;
    
	if ( rxSize >= rxPayloadSize || rxPayloadSize == 0)
    { /* full uart packet received */
    	    rxUartHeaderSize = 0; /* new packet to receive */
	    if ( oldRequestFormat )
	    { /* old format request totally received -> now handle it */
            return uartStreamOldFormatRequest( );
	    }
	    return rxMsgLength;
	}
    }
    return 0; /* continue to receive */
}

void uartStreamTransmit ( u16 totalTxSize )
{
    if ( totalTxSize > 0 )
    {
        ioLedOn( );

        /* generate a new tid for tx */
        UART_GENERATE_TID_FOR_TX( rxTid, txTid );

        /* TX-packet setup */
        UART_TID( txUartHeaderBuffer )    = txTid;
	UART_STATUS( txUartHeaderBuffer ) = StreamDispatcherGetLastError();
        UART_SET_PAYLOAD_SIZE( txUartHeaderBuffer, totalTxSize );

        uartTxNBytes( txUartHeaderBuffer, UART_HEADER_SIZE );
        uartTxNBytes( txBuffer, totalTxSize );

        ioLedOff();
    }
}
u8 checkMsgCRC ( u16 msgLength, u16 msgCRC, u8 *msgBuffer )
{
    u16 calculatedCRC;
    calculatedCRC = calcCrc16(msgBuffer, msgLength);
    if(msgCRC != calculatedCRC)
        return -1;
    return 0;
}

//u16 uartStreamReceive ( )
//{
//    if ( uartRxNumBytesAvailable() > 0 )	
//    { /* data received from host */
            
//        u16 uartRxBytes;
//	u16 missingHeaderBytes = UART_HEADER_SIZE - rxUartHeaderSize;

//	if ( missingHeaderBytes > 0 ) /* we still need to receive the header */
//       {
//	    ioLedOn();
//	    uartRxNBytes( rxUartHeaderBuffer + rxUartHeaderSize, &missingHeaderBytes );
//	    rxUartHeaderSize += missingHeaderBytes;
//	    ioLedOff( );
//	    if ( rxUartHeaderSize < UART_HEADER_SIZE )
//	    {
//		return 0; /* continue to wait for data for header */
//	    }

	    /* if we get here the uart header is totally received -> interpret it */

//	    if ( UART_STATUS( rxUartHeaderBuffer ) != 0 ) /* this is a request in the old format */
//	    { /* in the old format at this position we had the payload size - which was never 0 
//                 in the new format here this byte is resered and 0 when sent from host to device */
// 	        oldRequestFormat = 1;
//	        rxPayloadSize = UART_STATUS( rxUartHeaderBuffer );
                /* The old format had only a 2 bytes header, copy the additional 2 bytes
                   into the payload buffer and set up rxSize. */
//                memcpy(rxBuffer, rxUartHeaderBuffer + 2, UART_HEADER_SIZE - 2);
//                rxEnd = rxBuffer + 2;
//                rxSize = UART_HEADER_SIZE - 2;
//	    }
//	    else
//	    {
//  	        oldRequestFormat = 0;
//		rxPayloadSize = UART_GET_PAYLOAD_SIZE( rxUartHeaderBuffer );
//                rxSize        = 0; /* no payload received so far */
//                rxEnd         = rxBuffer; /* we start a new uart packet */
//	    }
//	    rxTid         = UART_TID( rxUartHeaderBuffer );
//	}

	/* if we get here we receive payload (header totally received) */
//       ioLedOn();
//	uartRxBytes = rxPayloadSize - rxSize;
	/* add the new data at the end of the data in the rxBuffer (i.e. where rxEnd points to) */
//        uartRxNBytes( rxEnd, &uartRxBytes );
//        rxSize += uartRxBytes;      /* adjust number of totally received bytes by number of actually received bytes*/
//	rxEnd  += uartRxBytes;

//	ioLedOff( );

//	if ( rxSize >= rxPayloadSize )
//        { /* full uart packet received */
//    	    rxUartHeaderSize = 0; /* new packet to receive */
//	    if ( oldRequestFormat )
//	    { /* old format request totally received -> now handle it */
//		return uartStreamOldFormatRequest( );
//	    }
//	    return rxPayloadSize;
//	}
//    }
//    return 0; /* continue to receive */
//}

