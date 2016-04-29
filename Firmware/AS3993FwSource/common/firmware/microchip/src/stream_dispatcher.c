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
 *      PROJECT:   ASxxxx firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author M. Arpa originated by W. Reichart
 *
 *  \brief main dispatcher for streaming protocol uses a stream_driver 
 *   for the io.
 *
 */


/* --------- includes ------------------------------------------------------- */

#include "system_clock.h"
#include "ams_stream.h"
#include "stream_dispatcher.h"
#include "stream_driver.h"
#include "usb_hid_stream_driver.h"
#include "uart_stream_driver.h"
#include "bootloadable.h"
#include "ams_types.h"
#include "i2c_driver.h"
#include "spi_driver.h"
#include "logger.h"
#include "appl_commands.h"
#include "platform.h"
#include "timer.h"
#include "crc16.h"

/* FCY needed by libpic30.h, pessimistically assume the maximum, needed for __delay functions */
#ifndef FCY
#define FCY 16000000ULL
#endif
#include <libpic30.h>

#ifdef NO_INITIAL_LOAD
// no initial loading of variable reduces start-up time
#define NOLOAD __attribute__ ((noload));
#else
#define NOLOAD ;
#endif

/* ------------- local variables -------------------------------------------- */
static u8 rxBuffer[ COM_BUFFER_MAX_SIZE ] NOLOAD; /*! buffer to store protocol packets received from the Host */
static u8 txBuffer[ COM_BUFFER_MAX_SIZE ] NOLOAD; /*! buffer to store protocol packets which are transmitted to the Host */

static u32 systemClock;
static u8 lastError; /* flag inicating different types of errors that cannot be reported in the protocol status field */


/* ------------- local functions --------------------------------------------- */
//static u8 handleConfig ( u16 rxed, u8 * rxData, u16 * toTx, u8 * txData )
//{
//    u16 address;
//    u16 mask;
//    u16 data;
//    u16 * ptr;
//    u16 temp;
//    u16 txBack = *toTx;
//    u8 wordSize;
//    u8 status = AMS_STREAM_PROTOCOL_FAILED;
//    *toTx = 0; /* make sure we do not send something back in error cases */
//
//    if ( rxed > AMS_CONFIG_WORD_SIZE_OFFSET )
//    {
//        wordSize = rxData[ AMS_CONFIG_WORD_SIZE_OFFSET ];
//        if ( wordSize == AMS_CONFIG_PIC_WORD_SIZE )
//        {
//            if ( rxed > AMS_CONFIG_READ_REQUEST_LENGTH(wordSize) )
//            {
//                status = AMS_STREAM_NO_ERROR;
//                rxData++;
//                address = AMS_GET_16BIT( rxData );
//                rxData += wordSize;
//                /* make out of address an pointer */
//
//              ptr = ( u16*) address;
//                if ( rxed > AMS_CONFIG_WRITE_REQUEST_LENGTH(wordSize) )
//                { /* this is a write */
//
//                    mask = AMS_GET_16BIT( rxData );
//                    rxData += wordSize;
//                    data = AMS_GET_16BIT( rxData );
//                    /* write to specified address -
//                     * FIXME : make sure no alignment error occurs */
//
//                    temp = *ptr; /* read current content */
//                    temp = temp & ~mask; /* clear all bits that can be written */
//                    temp = temp | ( data & mask ); /* set all bits that can be written */
//                    INFO_LOG( "CONF: addr=%u, mask=%u, data=%u, new=%u\n", address, mask, data, temp );
//                    *ptr = temp; /* write back */
//                }
//                if ( txBack >= AMS_CONFIG_READ_RESPONSE_LENGTH(wordSize) )
//                {
//                    temp = *ptr; /* read current content */
//                    INFO_LOG( "CONF: addr=%u, data=%u\n", address, temp );
//                    AMS_SET_16BIT( temp, txData );
//                    *toTx = AMS_CONFIG_READ_RESPONSE_LENGTH(wordSize);
//                }
//            }
//        }
//    }
//    return status;
//}

/*static u8 handleI2c ( u16 rxed, u8 * rxData, u16 * toTx, u8 * txData )
{
    return i2cRxTx( rxed, rxData, *toTx, txData, TRUE, TRUE );
}*/

//static u8 handleI2cConfig ( u16 rxed, u8 * rxData )
//{ 
    //i2cConfig_t config;
    //i2cDeserialiseConfig( &config, rxData );
    //return i2cInitialize( systemClock, &config, 0 /* oldConfig */ );
//}

//static u8 handleSpi ( u16 rxed, u8 * rxData, u16 * toTx, u8 * txData )
//{
    //u8 status;
    // find maximum between send and receive - and hand this in to
    //   the spi driver 
    //u16 maxSize = ( rxed > *toTx ? rxed : *toTx );
    //spiActivateSEN();
    //status = spiTxRx( rxData, txData, maxSize );
    //spiDeactivateSEN();
    //return status;
//}

/*static u8 handleSpiConfig ( u16 rxed, u8 * rxData )
{    
    spiConfig_t config;
    spiDeserialiseConfig( &config, rxData );
    return spiInitialize( &config, 0 );
}*/

static u8 handleReset ( u16 rxed, u8 * rxData )
{
    u8 status = AMS_STREAM_NO_ERROR;

    DBG_ASSERT( rxed == 1); /* a reset command consists of 2 bytes, command byte and flags  */
    /* at position 0 after the reset command we find the device selection flag */
    switch( *rxData )
    {
    case AMS_COM_RESET_MCU: /* reset the PIC */
        INFO_LOG( "Reset MCU\n" );
#ifdef __DEBUG
        DBG_ASSERT( 0 );
#endif
        StreamDisconnect();
        __delay_ms( 100 ); /* to give host time to recognize detach event */
        Reset();
        break;
    case AMS_COM_RESET_PERIPHERAL: /* reset the peripheral(s) */
        INFO_LOG( "Reset Peripherals\n" );
        status = applPeripheralReset( );
        break;
    default:
        INFO_LOG( "Reset N/A\n");
        status = AMS_STREAM_UNHANDLED_PROTOCOL;
        break;
    }
    return status;
}

static u8 handleFirmwareInformation ( u16 * toTx, u8 * txData )
{
    u16 size = strlen( applFirmwareInformation( ) );
    INFO_LOG( "FirmwareInfo\n" );
    /*if ( size >= *toTx - 1 )
    {
	size = *toTx - 1;
    }*/
    if ( size >= AMS_STREAM_SHORT_STRING - 1 )
    {
	size = AMS_STREAM_SHORT_STRING - 1;
    }
    *toTx = size + 1; /* for zero terminator */
    memcpy( txData , applFirmwareInformation(), size + 1 );
    return AMS_STREAM_NO_ERROR;
}

static u8 handleFirmwareNumber ( u16 * toTx, u8 * txData )
{
    INFO_LOG( "FirmwareNumber\n" );
    *toTx = 3; /* 1-byte major, 1-byte minor, 1-byte release marker */
    txData[0] = ( firmwareNumber >> 16 ) & 0xFF;
    txData[1] = ( firmwareNumber >>  8 ) & 0xFF;
    txData[2] =   firmwareNumber         & 0xFF;
    return AMS_STREAM_NO_ERROR;
}

static u8 handleEnterBootloader ( )
{
    INFO_LOG( "Enable Bootloader\n" );
    enableBootloader(); /* never returns */
    return AMS_STREAM_NO_ERROR;
}

static s8 sendResponse ( u8 msgType, u8 status, u8 *txBuf, u16 toTx )
{
    u16 messageLength;
    u16 crc;
    
    switch( msgType )
    {
        case CMD_READER_CONFIG:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_READER_CONFIG_RESP );      
            break;
        case CMD_ANTENNA_POWER:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_ANTENNA_POWER_RESP );      
            break;
        case CMD_CHANGE_FREQ:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_CHANGE_FREQ_RESP );      
            break;
        case CMD_GEN2_SETTINGS:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_GEN2_SETTINGS_RESP );      
            break;      
        case CMD_CONFIG_TX_RX:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_CONFIG_TX_RX_RESP );      
            break;
        case CMD_INVENTORY_GEN2:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_INVENTORY_GEN2_RESP );      
            break;
        case CMD_SELECT_TAG:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_SELECT_TAG_RESP );      
            break;
        case CMD_WRITE_TO_TAG:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_WRITE_TO_TAG_RESP );      
            break;
        case CMD_READ_FROM_TAG:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_READ_FROM_TAG_RESP );      
            break;
        case CMD_LOCK_UNLOCK_TAG:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_LOCK_UNLOCK_TAG_RESP );      
            break;
        case CMD_KILL_TAG:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_KILL_TAG_RESP );      
            break;
        case CMD_START_STOP:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_START_STOP_RESP );      
            break;
        case CMD_TUNER_TABLE:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_TUNER_TABLE_RESP );      
            break;
        case CMD_AUTO_TUNER:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_AUTO_TUNER_RESP );      
            break;
        case CMD_ANTENNA_TUNER:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_ANTENNA_TUNER_RESP );      
            break;
        case CMD_INVENTORY_6B:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_INVENTORY_6B_RESP );      
            break;
        case CMD_READ_FROM_TAG_6B:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_READ_FROM_TAG_6B_RESP );      
            break;
        case CMD_WRITE_TO_TAG_6B:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_WRITE_TO_TAG_6B_RESP );      
            break;
        case CMD_GENERIC_CMD_ID:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_GENERIC_CMD_ID_RESP );      
            break;
        case CMD_RSSI_MEAS_CMD_ID:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_RSSI_MEAS_CMD_ID_RESP );      
            break;
        case AMS_COM_CTRL_CMD_FW_NUMBER:
                UART_SET_MESSAGE_TYPE( txBuf, COM_CTRL_CMD_FW_NUMBER_RESP ); 
            break;
        case AMS_COM_CTRL_CMD_RESET:
                UART_SET_MESSAGE_TYPE( txBuf, COM_CTRL_CMD_RESET_RESP ); 
            break; 
        case AMS_COM_CTRL_CMD_FW_INFORMATION:
                UART_SET_MESSAGE_TYPE( txBuf, COM_CTRL_CMD_FW_INFORMATION_RESP ); 
            break; 
        case AMS_COM_CTRL_CMD_ENTER_BOOTLOADER:
                UART_SET_MESSAGE_TYPE( txBuf, COM_CTRL_CMD_ENTER_BOOTLOADER_RESP ); 
            break; 
        case AMS_COM_WRITE_REG:
                UART_SET_MESSAGE_TYPE( txBuf, COM_WRITE_REG_RESP ); 
            break; 
        case AMS_COM_READ_REG:
                UART_SET_MESSAGE_TYPE( txBuf, COM_READ_REG_RESP ); 
            break;
        case CMD_GET_TAG_DATA:
                UART_SET_MESSAGE_TYPE( txBuf, CMD_GET_TAG_DATA_RESP ); 
        default:
            break;
    }
    
    messageLength = toTx + UART_HEADER_SIZE;
    UART_SET_MESSAGE_LENGTH( txBuf, messageLength );    
    UART_SET_MESSAGE_CRC( txBuf, 0 );  
    UART_SET_MESSAGE_STATUS( txBuf, status );
    crc = calcCrc16(txBuf, messageLength);
    UART_SET_MESSAGE_CRC( txBuf, crc ); 
    return uartTxNBytes( txBuf, messageLength );
}


static void processReceivedPackets ( )
{
    /* every time we enter this function, the last txBuffer was already sent.
       So we fill a new transfer buffer */
    //u8 *txEnd = &txBuffer[6]; /* the txEnd always points to the end of header and start of payload */
    //u16 txSize = 0;

    INFO_LOG( "ProcessReceivedPackets\n" );

    //while ( StreamHasAnotherPacket( ) )
    //{
    /* read out protocol header data */
    u8  msgType    = UART_GET_MESSAGE_TYPE( rxBuffer );
    u16 msgLength  = UART_GET_MESSAGE_LENGTH( rxBuffer );
    u16 msgCRC  = UART_GET_MESSAGE_CRC( rxBuffer );
    u16 rxed       = msgLength - UART_HEADER_SIZE;
    u16 toTx = 0;
    u8  *rxData    = UART_GET_MESSAGE_PAYLOAD( rxBuffer );
    
    /* set up tx pointer for any data to be transmitted back to the host */
    u8  *txBuf    = txBuffer;
    u8  status     = NO_ERROR;
	
    //s8 isReadCommand = ! ( protocol & AMS_COM_WRITE_READ_NOT );
	//protocol &= ~AMS_COM_WRITE_READ_NOT; /* remove direction flag */
    
    /* decide which protocol to execute */
    INFO_LOG( "RX-Packet: Prot=%hhx, rxed=%u, toTx=%u\n", msgType, rxed, toTx );
#if STREAM_DEBUG
    LOGDUMP( rxBuffer, UART_HEADER_SIZE + rxed );
#endif

   UART_SET_MESSAGE_CRC( rxBuffer, 0);
    if( checkMsgCRC(msgLength, msgCRC, rxBuffer) != 0 )
    {
        sendResponse( msgType, CRC_ERROR, txBuf, 0 );
        return;
    }
    
    if( rxed > PAYLOAD_MAX_SIZE )
    { 
        sendResponse( msgType, SIZE_ERROR, txBuf, 0 );
        return;
    }
    
    switch ( msgType )
    {
            /*case AMS_COM_CONFIG:
                status = handleConfig( rxed, rxData, &toTx, &txBuf[6] );
                sendResponse( msgType, status, txBuf, toTx );
            break;
            case AMS_COM_I2C:
            status = handleI2c( rxed, rxData, &toTx, txData );
                break;
            case AMS_COM_I2C_CONFIG:
            status = handleI2cConfig( rxed, rxData );
                break;
            case AMS_COM_SPI:
            status = handleSpi( rxed, rxData, &toTx, txData );
                break;
            case AMS_COM_SPI_CONFIG:
            status = handleSpiConfig( rxed, rxData );
                break;*/
        case AMS_COM_CTRL_CMD_RESET:
            if(*rxData == AMS_COM_RESET_MCU || *rxData == AMS_COM_RESET_PERIPHERAL)
            {
                sendResponse( msgType, status, txBuf, 0 );
                status = handleReset( rxed, rxData );
            }
            else
            {
                status = handleReset( rxed, rxData );
                sendResponse( msgType, status, txBuf, 0 );
            }
            break;
        case AMS_COM_CTRL_CMD_FW_INFORMATION:
                status = handleFirmwareInformation( &toTx, &txBuf[6] );
                sendResponse( msgType, status, txBuf, toTx );
            break;
        case AMS_COM_CTRL_CMD_FW_NUMBER:
                status = handleFirmwareNumber( &toTx, &txBuf[6] );
                sendResponse( msgType, status, txBuf, toTx );
            break;
        case AMS_COM_CTRL_CMD_ENTER_BOOTLOADER:
                sendResponse( msgType, NO_ERROR, txBuf, 0 );  
                status = handleEnterBootloader( );  //never returns from call above
            break;
        case AMS_COM_WRITE_REG:
                status = applWriteReg( rxed, rxData, &toTx, &txBuf[6] );
                sendResponse( msgType, status, txBuf, toTx );
            break;
        case AMS_COM_READ_REG:
                status = applReadReg( rxed, rxData, &toTx, &txBuf[6] );
                sendResponse( msgType, status, txBuf, toTx );
            break;
        default:
                if ( msgType > CMD_RSSI_MEAS_CMD_ID )
                { /* reserved protocol value and not handled so far */
                    status = AMS_STREAM_UNHANDLED_PROTOCOL;
                    sendResponse( msgType, status, txBuf, 0 );
                }
                else /* application protocol */
                {
                    if( msgType == CMD_READ_FROM_TAG )
                        toTx = rxData[9];
                    if( msgType == CMD_GENERIC_CMD_ID )
                        toTx = rxData[10];
                    if( msgType == CMD_CHANGE_FREQ && rxData[0] == 16 )
                    {
                        sendResponse( msgType, NO_ERROR, txBuf, 0 );
                        status = applProcessCmd( msgType, rxed, rxData, &toTx, &txBuf[6] );
                    }
                    else
                    {
                        status = applProcessCmd( msgType, rxed, rxData, &toTx, &txBuf[6] );
                        sendResponse( msgType, status, txBuf, toTx );
                    }
                }
            break;
    }
	
    return;
}

//static u16 processCyclic ( )
//{
//    /* every time we enter this function, the last txBuffer was already sent.
//       So we fill a new transfer buffer */
//    u8 * txEnd = txBuffer; /* the txEnd always points to the next position to be filled with data */
//    u16 txSize = 0;
//    u16 toTx;
//    u8 protocol;
//    u8 status = AMS_STREAM_NO_ERROR;
//
//    do
//    {
//        /* set up tx pointer for any data to be transmitted back to the host */
//        u8 * txData = UART_GET_MESSAGE_PAYLOAD( txEnd );
//        toTx = 0;
//        status = applProcessCyclic( &protocol, &toTx, txData, AMS_STREAM_MAX_DATA_SIZE - txSize );
//
//        /* fill out transmit packet header if any data was produced */
//        if ( toTx > 0 )
//        {
//            INFO_LOG( "ProcessCyclic\n" );
//
//            AMS_STREAM_DT_SET_PROTOCOL( txEnd, protocol );
//            AMS_STREAM_DT_SET_STATUS( txEnd, status );
//            AMS_STREAM_DT_SET_TX_LENGTH( txEnd, toTx );
//
//            INFO_LOG( "TX-Packet: protocol=%hhx status=%hhx toTx=%u\n", protocol, status, toTx );
//#if STREAM_DEBUG
//	    LOGDUMP( txEnd, toTx + AMS_STREAM_HEADER_SIZE );
//#endif
//            /* adjust pointer to the enter next data, and total size */
//            txEnd += ( toTx + AMS_STREAM_HEADER_SIZE );
//            txSize += (toTx + AMS_STREAM_HEADER_SIZE );
//        }
//        else if ( status != AMS_STREAM_NO_ERROR ) /* protocol failed, we indicate an error if command itself does not send back */
//        {
//            lastError = status;
//        }
//    }
//    while ( toTx > 0 );
//
//    return txSize;
//}


/* --------- global functions ------------------------------------------------------------- */

void StreamDispatcherInitAndConnect ( u32 sysClk )
{
    StreamDispatcherInit( sysClk );
    StreamConnect();
}

void StreamDispatcherInit ( u32 sysClk )
{
    StreamDispatcherGetLastError();
    systemClock = sysClk;
    StreamInitialize( rxBuffer, txBuffer );
}

u8 StreamDispatcherGetLastError( )
{
    u8 temp = lastError;
    lastError = AMS_STREAM_NO_ERROR;
    return temp;
}

void ProcessIO(void)
{
    //u16 txSize;

    if ( UARTReady() )
    {
        /* read out data from stream driver, and move it to module-local buffer */
        if ( packetReceive() > 0 )
        {
            /* if we have at least one fully received packet, we start execution */
            
            /* interpret one (or more) packets in the module-local buffer */
            processReceivedPackets( );
            
            /* transmit any data waiting in the module-local buffer */
            //StreamTransmit( txSize );
        }
        
        /* we need to call the processCyclic function for all applications that
           have any data to send (without receiving a hid packet). The data to
           be sent is written into the module-local buffer */
        //txSize = processCyclic( );
        
        /* transmit any data waiting in the module-local buffer */
        //StreamTransmit( txSize );
    }
}

