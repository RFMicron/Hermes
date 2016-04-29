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
 *      PROJECT:   PIC24FJxxGBxxx 
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author M. Arpa
 *          
 *  \brief routines for system clock control
 *
 *  
 *
 */


#ifndef SYSTEM_CLOCK_H
#define SYSTEM_CLOCK_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

/* FCY must be defined before including libpic30.h */
#ifdef __PIC24FJ128GB202__
// Otherwise warning
#ifdef FCY
#undef FCY
#endif

#endif
#define FCY    theSystemClock

#include <libpic30.h>
#include "ams_types.h"

/* NOTE: the Config Word2 must have either FCKSM_CSECME or FCKSM_CSECMD
   This means that clock-switching is enabled. 

   If you want to use the primary oscillator you must define:
   #define POSC_NOMINAL    xxxxULL 
   where xxxs is the frequency in hertz of the oscillator. 
   Also you must configure the divider in the Config Word2 to the 
   appropriate value so that the PLL block has as input frequency 4 MHz.
   E.g. if your oscillator has 16MHz output you must use PLLDIV_4

   You should also consider using IESO_ON which allows the system to
   start from the FRC and switch to the primary oscillator when this
   is stable.
 */


/*
******************************************************************************
* DEFINES
******************************************************************************
*/

#define CLK_SRC_FRCDIV   0x7          /* internal oscillator for slow speed */
#define CLK_SRC_LPRC     0x5          /* watchdog etc. */  
#define CLK_SRC_SOSC     0x4          /* 2nd oscillator if available */
#define CLK_SRC_POSC_PLL 0x3          /* 1st oscillator (if available) for usb */
#define CLK_SRC_POSC     0x2          /* 1st oscillator if available */
#define CLK_SRC_FRCPLL   0x1          /* internal oscillator for usb */
#define CLK_SRC_FRC      0x0          /* internal oscillator */     

/*
******************************************************************************
* DECLARATIONS
******************************************************************************
*/
extern u32 theSystemClock; 

/*
******************************************************************************
* FUNCTIONS
******************************************************************************
*/

/*!
 *****************************************************************************
 *  \brief must call this function before calling any delay, clock functions 
 *         etc.
 *
 *  \returns the FCY (the instruction cycle frequency) in hertz
 *****************************************************************************
 */
u32 systemClockInitialise( );

/*!
 *****************************************************************************
 *  \brief configure if posc and sosc are enabled
 *****************************************************************************
 */
void systemClockConfigure( s8 poscEnableDuringSleep, s8 soscEnable );

/*!
 *****************************************************************************
 *  \brief reads out the HW configuration and calculates the FCY in hertz
 *
 *  \returns the FCY (the instruction cycle frequency) in hertz
 *****************************************************************************
 */
u32 systemClockGetFcy( );

/*!
 *****************************************************************************
 *  \brief returns the currently selected clock source
 *****************************************************************************
 */
u8 systemClockGetClockSource( );

/*!
 *****************************************************************************
 *  \brief change clock source
 *
 *  Function to change the clock source and frequency. Uses a frequency that is 
 * closest to the deried one (if possible )
 *
 *  \returns the new FCY (the instruction cycle frequency) in hertz
 *****************************************************************************
 */
u32 systemClockChangeClockSource( u16 source, u32 newFcy );


#endif /* SYSTEM_CLOCK_H */
