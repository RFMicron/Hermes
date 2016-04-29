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
 *      PROJECT:   AMS USB encapsulation
 *      $Revision: $
 *      LANGUAGE: C++
 */

/*! \file 
 *
 *  \author M. Arpa
 *
 *  \brief Encapsulating of some parts of USB handling.
 *         If used, this allows to wakeup from USB even when sleeping,        
 *         without a seperate pin for this purpose.
 */

#include "ams_usb.h"
#include "usb.h"
#include "usb_device.h"


/* ------------- functions ------------------------------------------------- */

void amsUsbInitialise ( )
{
    asm("disi #0x3FFF"); /* disable interrupts while handling flags */
    USBDeviceInit();	/* Initializes USB module SFRs and firmware */
#ifdef AMS_USB_WAKEUP_FROM_SLEEP
    IFS5bits.USB1IF = 0;
    /* clear flag, as bus device is not attached */
    USBClearInterruptFlag(USBActivityIFReg,USBActivityIFBitNum);
    U1OTGIEbits.ACTVIE = 1;
    IEC5bits.USB1IE = 1;     /* Enable the interrupt */
#endif
    USBDeviceAttach();  /* device is ready to be detected on bus now */
    asm( "disi #0" ); /* allow interrupt handling now */
}

s8 amsUsbIsCableConnected ( )
{
#ifdef AMS_USB_WAKEUP_FROM_SLEEP
    return ( U1OTGSTATbits.VBUSVD );
#else
    return amsUsbIsReady( );
#endif
}

s8 amsUsbIsReady ( )
{
    
#ifdef AMS_USB_WAKEUP_FROM_SLEEP
    if ( amsUsbIsCableConnected() )
#endif
    {
        return ( ( USBGetDeviceState() == CONFIGURED_STATE ) && ! USBSuspendControl );
    }
#ifdef AMS_USB_WAKEUP_FROM_SLEEP
    else
    {
        return false;
    }
#endif
    
}

void amsUsbIsNotReady ( )
{
}
