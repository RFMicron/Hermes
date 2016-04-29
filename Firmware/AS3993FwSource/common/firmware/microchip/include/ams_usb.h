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

#ifndef AMS_USB_H
#define AMS_USB_H

#include "ams_types.h"

/* ------------- functions ------------------------------------------------- */

/*!
 *****************************************************************************
 *  \brief Function that configures the USB module to operate as device
 *
 * The USB module is configured, powered, the interrupt on bus
 * activity is enabled and the device is attached (=prepared to work when
 * usb is connected). 
 *
 * Note that USB interrupts are enabled after this function is called.
 *
 *****************************************************************************
 */
void amsUsbInitialise( );

/*!
 *****************************************************************************
 *  \brief Function returns true if a USB cable is connected
 *
 *  \returns <>0 when a USB cable is connected (do not go to sleep while true)
 *  \returns ==0 when no USB cable is connected
 *****************************************************************************
 */
s8 amsUsbIsCableConnected( );

/*!
 *****************************************************************************
 *  \brief Function checks if USB is connected and ready for use
 *
 *  \returns <>0 when USB is connected and ready for use
 *  \returns ==0 when USB is not ready for use 
 *****************************************************************************
 */
s8 amsUsbIsReady( );

/*!
 *****************************************************************************
 *  \brief dummy function
 *****************************************************************************
 */
void amsUsbIsNotReady( );

#endif /* AMS_USB_H */
