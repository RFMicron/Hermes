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

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "system_clock.h"
#include <p24Fxxxx.h>
#ifndef __PIC24FJ128GB202__
#include <PwrMgnt.h>
#endif

#define FRC_NOMINAL       8000000ULL /* 8 MHz */
#define LPRC_NOMINAL        32768ULL /* 32.768 KHz */
#define PLL_OUT_NOMINAL  32000000ULL /* 32 MHz path of PLL is used as input for system clock */

#ifndef POSC_NOMINAL
#define POSC_NOMINAL            0ULL /* no primary oscillator */
#endif
#ifndef SOSC_NOMINAL
#define SOSC_NOMINAL            0ULL /* no secondary oscillator */
#endif

/*
******************************************************************************
* variables
******************************************************************************
*/

/* this is the instruction frequency */
u32 theSystemClock = 0;

/*
******************************************************************************
* FUNCTIONS
******************************************************************************
*/
static u32 systemClockFromPllPrescaler ( )
{
    u32 divider = 1 << CLKDIVbits.CPDIV;
    return ( PLL_OUT_NOMINAL / divider );
}

#if !defined(__PIC24FJ256GB110__) && !defined(__PIC24FJ128GB202__)  /* TODO: adapt this for GB110 (has no PLLEN but PLLDIS in config words) */
static void systemClockFindPllPrescaler ( u32 frequency )
{
    u32 border = PLL_OUT_NOMINAL / 2 ;
    u16 divider;
    if ( frequency >= PLL_OUT_NOMINAL )
    {
	frequency = PLL_OUT_NOMINAL;
    }
    for ( divider = 0; divider < 3; divider++ )
    {
        u32 offset = border / 2;
        if ( frequency > border + offset ) 
        {
	  break; /* found it */
	}
	border /= 2;
    }
    /* either found it or stopped by 3 (which is smallest possible divider) */
    CLKDIVbits.CPDIV = divider;
#ifndef __PIC24FJ256GB106__
    CLKDIVbits.PLLEN = 1; /* enable pll */
#endif
    /* wait for lock of pll  */
    while ( ! OSCCONbits.LOCK );
}
#endif

static u32 systemClockFromFrcPostscaler ( )
{
    u32 divider = 1 << CLKDIVbits.RCDIV;
    return ( FRC_NOMINAL / divider );
}
#ifndef __PIC24FJ128GB202__
static void systemClockFindFrcPostscaler ( u32 frequency )
{
    u32 border = FRC_NOMINAL / 2 ;
    u16 divider;
    if ( frequency >= FRC_NOMINAL )
    {
	frequency = FRC_NOMINAL;
    }
    for ( divider = 0; divider < 7; divider++ )
    {
        u32 offset = border / 2;
        if ( frequency > border + offset ) 
        {
	  break; /* found it */
	}
	border /= 2;
    }
    /* either found it or stopped by 7 (which is smallest possible divider) */
    CLKDIVbits.RCDIV = divider;
}
#endif
void systemClockConfigure( s8 poscEnableDuringSleep, s8 soscEnable )
{
    //OSCCON = OSCCON | 0x4;
    OSCCONbits.POSCEN = 1; // ( poscEnableDuringSleep ? 1 : 0 );
    OSCCONbits.SOSCEN = 1; //( soscEnable ? 1 : 0 );
}

u32 systemClockInitialise ( )
{ 
    /* read out configuration from HW */  
    return systemClockGetFcy( );
}

u32 systemClockGetFcy ( )
{
    u32 frequency = 0;
    switch ( OSCCONbits.COSC )
    {

        case CLK_SRC_LPRC:
	    frequency = LPRC_NOMINAL;
	    break;

        case CLK_SRC_SOSC:
	    frequency = SOSC_NOMINAL;
	    break;

        case CLK_SRC_POSC_PLL:
	    if ( POSC_NOMINAL )
	    {
	        frequency = systemClockFromPllPrescaler( );
	    }
	    break;

	case CLK_SRC_POSC:
	    frequency = POSC_NOMINAL;
	    break;

        case CLK_SRC_FRCDIV:
	    frequency = systemClockFromFrcPostscaler( );
	    break;

        case CLK_SRC_FRCPLL:
	  frequency = systemClockFromPllPrescaler( );
	  break;

        case CLK_SRC_FRC:
	  frequency = FRC_NOMINAL;
	  break;
    }
    theSystemClock = frequency / 2; /* the theSystemClock is always half the system clock frequency */ 
    return theSystemClock;
}

u8 systemClockGetClockSource ( )
{
    return OSCCONbits.COSC;
}


#if !defined(__PIC24FJ256GB110__) && !defined(__PIC24FJ128GB202__)   /* TODO: adapt this for GB110 (has no PLLEN but PLLDIS in config words) */
u32 systemClockChangeClockSource ( u16 source, u32 newFcy )
{
    u32 frequency = newFcy * 2;

    PwrMgnt_OscSel( source );

#if 0
    /* if necessary change the source */
    if ( OSCCONbits.COSC != source )
    { /* change source */
        OSCCONBITS newOSCCONbits = OSCCONbits;
        WORD_VAL newOSCCON;

        /* NOTE: the Config Word2 must have either FCKSM_CSECME or FCKSM_CSECMD
           This means that clock-switching is enabled. We could read out the
           config-word2 and check that the corresponding bit is cleared.
           */

        newOSCCONbits.NOSC = source;
        newOSCCONbits.LOCK = 0;  /* read-only - so don't care */
        newOSCCONbits.OSWEN = 1; /* switch clock */

        /* convert from struct to word - need to go via the "void*" as we
           compile with strict-aliasing */
	newOSCCON.Val = *(WORD*)((void*)(&newOSCCONbits));

        __builtin_write_OSCCONH( newOSCCON.byte.HB );
        __builtin_write_OSCCONL( newOSCCON.byte.LB );

        while ( OSCCONbits.OSWEN ); /* wait for switch */


    }
#endif

    /* now depending on clock source we can trim the frequency or not */
    switch ( OSCCONbits.COSC )
    {
        case CLK_SRC_LPRC:
	    frequency = LPRC_NOMINAL;
            #ifndef __PIC24FJ256GB106__
            CLKDIVbits.PLLEN = 0; /* disable pll */
            #endif
	    break;

        case CLK_SRC_SOSC:
	    frequency = SOSC_NOMINAL;
            #ifndef __PIC24FJ256GB106__
            CLKDIVbits.PLLEN = 0; /* disable pll */
            #endif
	    break;

        case CLK_SRC_POSC_PLL:
	    if ( POSC_NOMINAL )
	    {
	      /* find desired fcy in either 4, 8, 16, or 32 MHz */
	        systemClockFindPllPrescaler( frequency );
		frequency = systemClockFromPllPrescaler( );
	    }
	    break;

	case CLK_SRC_POSC:
	    frequency = POSC_NOMINAL;
            #ifndef __PIC24FJ256GB106__
            CLKDIVbits.PLLEN = 0; /* disable pll */
            #endif
	    break;

        case CLK_SRC_FRCDIV:
	    /* find desired fcy in either 8, 4, 2, .... 32.25kHz */
	    systemClockFindFrcPostscaler( frequency );
	    frequency = systemClockFromFrcPostscaler( );
            #ifndef __PIC24FJ256GB106__
            CLKDIVbits.PLLEN = 0; /* disable pll */
            #endif
	    break;

        case CLK_SRC_FRCPLL:
	    /* find desired fcy in either 4, 8, 16, or 32 MHz */
	    systemClockFindPllPrescaler( frequency );
	    frequency = systemClockFromPllPrescaler( );
	    break;

        case CLK_SRC_FRC:
	    frequency = FRC_NOMINAL;
            #ifndef __PIC24FJ256GB106__
            CLKDIVbits.PLLEN = 0; /* disable pll */
            #endif
	    break;
    }
    theSystemClock = frequency / 2; /* the theSystemClock is always half the system clock frequency */ 
    return theSystemClock;
}
#endif

