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
 *          M. Arpa
 *
 *  \brief USB HID streaming driver implementation.
 *
 */

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include "ams_stream.h"
#include "GenericTypeDefs.h"
#include "errno.h"
#include "stream_driver.h"
#include "stream_dispatcher.h"
#include "usb_hid_stream_driver.h"
#include "usb.h"
#include "usb_function_hid.h"
#include "ams_types.h"
#include "logger.h"
#include <stdio.h>

#ifdef NO_INITIAL_LOAD
// no initial loading of variable reduces start-up time
#define NOLOAD __attribute__ ((noload));
#else
#define NOLOAD ;
#endif
/*
 ******************************************************************************
 * LOCAL VARIABLES
 ******************************************************************************
 */
#pragma udata
static USB_HANDLE USBOutHandle = 0;
static USB_HANDLE USBInHandle  = 0;

static u8 usbRxBuffer[USB_HID_REPORT_SIZE] NOLOAD;     //buffer for usb stack
static u8 usbTxBuffer[USB_HID_REPORT_SIZE] NOLOAD;     //buffer for usb stack

static u8 * rxBuffer;   //buffer location is set in StreamInitialize
static u8 * txBuffer;   //buffer location is set in StreamInitialize

static u8 txTid;
static u8 rxTid;
static u16 rxSize;
static u8 * rxEnd; /* pointer to next position where to copy the received data */



/*
 ******************************************************************************
 * GLOBAL FUNCTIONS
 ******************************************************************************
 */

void usbStreamInitialize (u8 * rxBuf, u8 * txBuf)
{
    // initialize the variable holding the handle for the last
    // transmission
    USBOutHandle    = 0;
    USBInHandle     = 0;
    /* setup pointers to buffers */
    rxBuffer = rxBuf;
    txBuffer = txBuf;
    /* so far we have received nothing */
    txTid = 0;
    rxTid = 0;
    rxSize = 0;
    rxEnd = rxBuffer;
}

void usbStreamConnect (void)
{
    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    USBDeviceAttach();
}

void usbStreamDisconnect (void)
{
    USBDeviceDetach();
}

u8 usbStreamReady (void)
{
    if((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1))
        return 0;

    return 1;
}

void usbStreamPacketProcessed ( u16 rxed )
{
#if STREAM_DEBUG
    USB_LOG( "usbStreamPacketProcessed: rxed=%u, rxSize=%u\n", rxed, rxSize );
#endif
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

#if STREAM_DEBUG
    USB_LOG( "usbStreamPacketProcessed-Leave: rxed=%u, rxSize=%u\n", rxed, rxSize );
#endif
}

s8 usbStreamHasAnotherPacket ( )
{
    return (  rxSize >= AMS_STREAM_HEADER_SIZE
           && rxSize >= ( AMS_STREAM_DR_GET_RX_LENGTH( rxBuffer ) + AMS_STREAM_HEADER_SIZE )
           );
}

u16 usbStreamOldFormatRequest ( )
{
    u8 protocol = usbRxBuffer[ 2 ];
    u8 toTx = usbRxBuffer[ 3 ];

    ioLedOn();

    USB_LOG( "OldFormatRequest\n" );

    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket( HID_EP, (BYTE*)usbRxBuffer, USB_HID_REPORT_SIZE );

    /* send back the special answer */
    if ( toTx > 0 || ( protocol & 0x40 ) ) /* response was required */
    {
        USB_LOG( "OldFormatRequest-tx\n" );
        /* wait here (and before copying the IN-buffer) until the USBInHandle is free again */
        while ( HIDTxHandleBusy( USBInHandle ) );

        usbTxBuffer[ 0 ] = AMS_STREAM_COMPATIBILITY_TID;
        usbTxBuffer[ 1 ] = 0x03; /* payload */
        usbTxBuffer[ 2 ] = protocol; 
	usbTxBuffer[ 3 ] = 0xFF; /* status = failed -> wrong protocol version */
	usbTxBuffer[ 4 ] = 0x00; /* no data will be sent back */

        /* initiate transfer now */
        USBInHandle = HIDTxPacket( HID_EP, (BYTE*)usbTxBuffer, USB_HID_REPORT_SIZE );
    }

    return 0;
}

u16 usbStreamReceive ( )
{
    if( !HIDRxHandleBusy(USBOutHandle) )				//Check if data was received from the host.
    {
        // if data was received, it is written to the usbBuffer

        /*
         * Read the usb hid buffer into the local buffer.
         * When the RX-Length within the first streaming packet is
         * longer than the HID payload, we have to concatenate several
         * packets (up to max. 256 bytes)
         */

        u16 packetSize;
        u8 payload  = USB_HID_PAYLOAD_SIZE( usbRxBuffer );

	if ( USB_HID_STATUS( usbRxBuffer ) != 0 ) /* this is a request in the old format */
	{ /* in the old format at this position we had the protocol id - which was never 0 
             in the new format here this byte is resered and 0 when sent from host to device */
	    return usbStreamOldFormatRequest( );
	}

        ioLedOn();

        rxTid = USB_HID_TID( usbRxBuffer );
        /* adjust number of totally received bytes */
        rxSize += payload;

        USB_LOG("HID-rx: tid=%hhx payload=%hhx, receivedDataSize=%u\n", rxTid, payload, rxSize );
#if STREAM_DEBUG
        USB_LOGDUMP( usbRxBuffer, USB_HID_REPORT_SIZE );
#endif

	/* add the new data at the end of the data in the rxBuffer (i.e. where rxEnd points to) */
	memcpy( rxEnd, USB_HID_PAYLOAD( usbRxBuffer ), payload );
	rxEnd += payload;
	packetSize = AMS_STREAM_DR_GET_RX_LENGTH( rxBuffer ) + AMS_STREAM_HEADER_SIZE;

#if STREAM_DEBUG
	memset( (void*)usbRxBuffer, 0xab, sizeof( usbRxBuffer ) );
#endif

        //Re-arm the OUT endpoint for the next packet
        USBOutHandle = HIDRxPacket( HID_EP, (BYTE*)usbRxBuffer, USB_HID_REPORT_SIZE );

        ioLedOff();

        if ( packetSize > rxSize )
        {
            /* indicate that we must continue receiving */
            return 0;
        }
        rxEnd = rxBuffer;   //next time we receive new data, we start over at buffer start
        return rxSize;
    }
    /* indicate that we did not receive anything - try next time again */
    return 0;
}

void usbStreamTransmit ( u16 totalTxSize )
{
    u16 offset = 0;

    while ( totalTxSize > 0 )
    {
        u8 payload = ( totalTxSize > USB_HID_MAX_PAYLOAD_SIZE ? USB_HID_MAX_PAYLOAD_SIZE : totalTxSize );
        ioLedOn( );

        /* wait here (and before copying the IN-buffer) until the USBInHandle is free again */
        while ( HIDTxHandleBusy( USBInHandle ) );

#if STREAM_DEBUG
        memset( usbTxBuffer, 0xCC, USB_HID_REPORT_SIZE );
#endif

        /* generate a new tid for tx */
	USB_HID_GENERATE_TID_FOR_TX( rxTid, txTid );

        /* TX-packet setup */
        USB_HID_TID( usbTxBuffer )          = txTid;
        USB_HID_PAYLOAD_SIZE( usbTxBuffer ) = payload;
	USB_HID_STATUS( usbTxBuffer )       = StreamDispatcherGetLastError();

        /* copy data to usb buffer */
        memcpy( USB_HID_PAYLOAD( usbTxBuffer ), txBuffer + offset, payload );

        USB_LOG("HID-tx: tid=%hhx payload=%hhx, (totalSize=%u)\n", txTid, payload, totalTxSize );
        USB_LOGDUMP( usbTxBuffer, USB_HID_REPORT_SIZE );

        totalTxSize -= payload;
        offset += payload;

        /* initiate transfer now */
        USBInHandle = HIDTxPacket( HID_EP, (BYTE*)usbTxBuffer, USB_HID_REPORT_SIZE );

        ioLedOff( );
    }
}

// ******************************************************************************************************
// ************** USB Callback Functions ****************************************************************
// ******************************************************************************************************
// The USB firmware stack will call the callback functions USBCBxxx() in response to certain USB related
// events.  For example, if the host PC is powering down, it will stop sending out Start of Frame (SOF)
// packets to your device.  In response to this, all USB devices are supposed to decrease their power
// consumption from the USB Vbus to <2.5mA each.  The USB module detects this condition (which according
// to the USB specifications is 3+ms of no bus activity/SOF packets) and then calls the USBCBSuspend()
// function.  You should modify these callback functions to take appropriate actions for each of these
// conditions.  For example, in the USBCBSuspend(), you may wish to add code that will decrease power
// consumption from Vbus to <2.5mA (such as by clock switching, turning off LEDs, putting the
// microcontroller to sleep, etc.).  Then, in the USBCBWakeFromSuspend() function, you may then wish to
// add code that undoes the power saving things done in the USBCBSuspend() function.

// The USBCBSendResume() function is special, in that the USB stack will not automatically call this
// function.  This function is meant to be called from the application firmware instead.  See the
// additional comments near the function.

/******************************************************************************
 * Function:        void USBCBSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Call back that is invoked when a USB suspend is detected
 *
 * Note:            None
 *****************************************************************************/
void USBCBSuspend(void)
{
    //Example power saving code.  Insert appropriate code here for the desired
    //application behavior.  If the microcontroller will be put to sleep, a
    //process similar to that shown below may be used:

    //ConfigureIOPinsForLowPower();
    //SaveStateOfAllInterruptEnableBits();
    //DisableAllInterruptEnableBits();
    //EnableOnlyTheInterruptsWhichWillBeUsedToWakeTheMicro();	//should enable at least USBActivityIF as a wake source
    //Sleep();
    //RestoreStateOfAllPreviouslySavedInterruptEnableBits();	//Preferably, this should be done in the USBCBWakeFromSuspend() function instead.
    //RestoreIOPinsToNormal();									//Preferably, this should be done in the USBCBWakeFromSuspend() function instead.

    //IMPORTANT NOTE: Do not clear the USBActivityIF (ACTVIF) bit here.  This bit is
    //cleared inside the usb_device.c file.  Clearing USBActivityIF here will cause
    //things to not work as intended.

    USBSleepOnSuspend();
}

/******************************************************************************
 * Function:        void USBCBWakeFromSuspend(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The host may put USB peripheral devices in low power
 *					suspend mode (by "sending" 3+ms of idle).  Once in suspend
 *					mode, the host may wake the device back up by sending non-
 *					idle state signaling.
 *
 *					This call back is invoked when a wakeup from USB suspend
 *					is detected.
 *
 * Note:            None
 *****************************************************************************/
void USBCBWakeFromSuspend(void)
{
    // If clock switching or other power savings measures were taken when
    // executing the USBCBSuspend() function, now would be a good time to
    // switch back to normal full power run mode conditions.  The host allows
    // a few milliseconds of wakeup time, after which the device must be
    // fully back to normal, and capable of receiving and processing USB
    // packets.  In order to do this, the USB module must receive proper
    // clocking (IE: 48MHz clock must be available to SIE for full speed USB
    // operation).
}

/********************************************************************
 * Function:        void USBCB_SOF_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB host sends out a SOF packet to full-speed
 *                  devices every 1 ms. This interrupt may be useful
 *                  for isochronous pipes. End designers should
 *                  implement callback routine as necessary.
 *
 * Note:            None
 *******************************************************************/
void USBCB_SOF_Handler(void)
{
    // No need to clear UIRbits.SOFIF to 0 here.
    // Callback caller is already doing that.
}

/*******************************************************************
 * Function:        void USBCBErrorHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The purpose of this callback is mainly for
 *                  debugging during development. Check UEIR to see
 *                  which error causes the interrupt.
 *
 * Note:            None
 *******************************************************************/
void USBCBErrorHandler(void)
{
    // No need to clear UEIR to 0 here.
    // Callback caller is already doing that.

    // Typically, user firmware does not need to do anything special
    // if a USB error occurs.  For example, if the host sends an OUT
    // packet to your device, but the packet gets corrupted (ex:
    // because of a bad connection, or the user unplugs the
    // USB cable during the transmission) this will typically set
    // one or more USB error interrupt flags.  Nothing specific
    // needs to be done however, since the SIE will automatically
    // send a "NAK" packet to the host.  In response to this, the
    // host will normally retry to send the packet again, and no
    // data loss occurs.  The system will typically recover
    // automatically, without the need for application firmware
    // intervention.

    // Nevertheless, this callback function is provided, such as
    // for debugging purposes.
}


/*******************************************************************
 * Function:        void USBCBCheckOtherReq(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        When SETUP packets arrive from the host, some
 * 					firmware must process the request and respond
 *					appropriately to fulfill the request.  Some of
 *					the SETUP packets will be for standard
 *					USB "chapter 9" (as in, fulfilling chapter 9 of
 *					the official USB specifications) requests, while
 *					others may be specific to the USB device class
 *					that is being implemented.  For example, a HID
 *					class device needs to be able to respond to
 *					"GET REPORT" type of requests.  This
 *					is not a standard USB chapter 9 request, and
 *					therefore not handled by usb_device.c.  Instead
 *					this request should be handled by class specific
 *					firmware, such as that contained in usb_function_hid.c.
 *
 * Note:            None
 *******************************************************************/
void USBCBCheckOtherReq(void)
{
    USBCheckHIDRequest();
}//end


/*******************************************************************
 * Function:        void USBCBStdSetDscHandler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USBCBStdSetDscHandler() callback function is
 *					called when a SETUP, bRequest: SET_DESCRIPTOR request
 *					arrives.  Typically SET_DESCRIPTOR requests are
 *					not used in most applications, and it is
 *					optional to support this type of request.
 *
 * Note:            None
 *******************************************************************/
void USBCBStdSetDscHandler(void)
{
    // Must claim session ownership if supporting this request
}//end


/*******************************************************************
 * Function:        void USBCBInitEP(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when the device becomes
 *                  initialized, which occurs after the host sends a
 * 					SET_CONFIGURATION (wValue not = 0) request.  This
 *					callback function should initialize the endpoints
 *					for the device's usage according to the current
 *					configuration.
 *
 * Note:            None
 *******************************************************************/
void USBCBInitEP(void)
{
    //enable the HID endpoint
    USBEnableEndpoint(HID_EP,USB_IN_ENABLED|USB_OUT_ENABLED|USB_HANDSHAKE_ENABLED|USB_DISALLOW_SETUP);
    //Re-arm the OUT endpoint for the next packet
    USBOutHandle = HIDRxPacket(HID_EP, (BYTE*)&usbRxBuffer, USB_HID_REPORT_SIZE);
}

/********************************************************************
 * Function:        void USBCBSendResume(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        The USB specifications allow some types of USB
 * 					peripheral devices to wake up a host PC (such
 *					as if it is in a low power suspend to RAM state).
 *					This can be a very useful feature in some
 *					USB applications, such as an Infrared remote
 *					control	receiver.  If a user presses the "power"
 *					button on a remote control, it is nice that the
 *					IR receiver can detect this signalling, and then
 *					send a USB "command" to the PC to wake up.
 *
 *					The USBCBSendResume() "callback" function is used
 *					to send this special USB signalling which wakes
 *					up the PC.  This function may be called by
 *					application firmware to wake up the PC.  This
 *					function should only be called when:
 *
 *					1.  The USB driver used on the host PC supports
 *						the remote wakeup capability.
 *					2.  The USB configuration descriptor indicates
 *						the device is remote wakeup capable in the
 *						bmAttributes field.
 *					3.  The USB host PC is currently sleeping,
 *						and has previously sent your device a SET
 *						FEATURE setup packet which "armed" the
 *						remote wakeup capability.
 *
 *					This callback should send a RESUME signal that
 *                  has the period of 1-15ms.
 *
 * Note:            Interrupt vs. Polling
 *                  -Primary clock
 *                  -Secondary clock ***** MAKE NOTES ABOUT THIS *******
 *                   > Can switch to primary first by calling USBCBWakeFromSuspend()

 *                  The modifiable section in this routine should be changed
 *                  to meet the application needs. Current implementation
 *                  temporary blocks other functions from executing for a
 *                  period of 1-13 ms depending on the core frequency.
 *
 *                  According to USB 2.0 specification section 7.1.7.7,
 *                  "The remote wakeup device must hold the resume signaling
 *                  for at least 1 ms but for no more than 15 ms."
 *                  The idea here is to use a delay counter loop, using a
 *                  common value that would work over a wide range of core
 *                  frequencies.
 *                  That value selected is 1800. See table below:
 *                  ==========================================================
 *                  Core Freq(MHz)      MIP         RESUME Signal Period (ms)
 *                  ==========================================================
 *                      48              12          1.05
 *                       4              1           12.6
 *                  ==========================================================
 *                  * These timing could be incorrect when using code
 *                    optimization or extended instruction mode,
 *                    or when having other interrupts enabled.
 *                    Make sure to verify using the MPLAB SIM's Stopwatch
 *                    and verify the actual signal on an oscilloscope.
 *******************************************************************/
void USBCBSendResume(void)
{
    static WORD delay_count;

    USBResumeControl = 1;                // Start RESUME signaling

    delay_count = 1800U;                // Set RESUME line for 1-13 ms
    do
    {
        delay_count--;
    }
    while(delay_count);
    USBResumeControl = 0;
}


/*******************************************************************
 * Function:        BOOL USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, WORD size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  WORD size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
BOOL USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, WORD size)
{
    if(applUSBCBHandler)
        applUSBCBHandler(event, pdata, size);

    switch(event)
    {
    case EVENT_CONFIGURED:
        //USB_LOG("EVENT_CONFIGURED\n");
        USBCBInitEP();
        break;
    case EVENT_SET_DESCRIPTOR:
        //USB_LOG("EVENT_SET_DESCRIPTOR\n");
        USBCBStdSetDscHandler();
        break;
    case EVENT_EP0_REQUEST:
        //USB_LOG("EVENT_EP0_REQUEST\n");
        USBCBCheckOtherReq();
        break;
    case EVENT_SOF:
        //USB_LOG("EVENT_SOF\n");
        USBCB_SOF_Handler();
        break;
    case EVENT_SUSPEND:
        //USB_LOG("EVENT_SUSPEND\n");
        USBCBSuspend();
        break;
    case EVENT_RESUME:
        //USB_LOG("EVENT_RESUME\n");
        USBCBWakeFromSuspend();
        break;
    case EVENT_BUS_ERROR:
        //USB_LOG("EVENT_BUS_ERROR\n");
        USBCBErrorHandler();
        break;
    case EVENT_TRANSFER:
        //USB_LOG("EVENT_TRANSFER\n");
        Nop();
        break;
    default:
        //USB_LOG("EVENT_UNKNOWN\n");
        break;
    }
    return TRUE;
}
