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
 *      PROJECT:   AS1130 MCU board firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author M. Arpa
 *
 *  \brief primitive debugging service routines for PIC24FJ64
 *
 */
/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#define USE_AND_OR
#include <p24Fxxxx.h>
#include "debug.h"
/*
******************************************************************************
* LOCAL DEFINES
******************************************************************************
*/

#define UART_SYSCLK 16000000
#define UART_BAUDRATE 115200
#define UART_BRG           8 //((UART_SYSCLK) / (16* (UART_BAUDRATE) ) - 1 ) and round it

#define UART_TXFULL 0x0200
#define UART_TXENABLE 0x8000
#define UART_TXAUTO 0x0400

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
static u32 systemTickOverflows;

void systemTicksInitialize( )
{
    systemTickOverflows = 0;
    T4CONbits.T32 = 1; /* select 32-bit mode timer 4+5 */
    T4CONbits.TCKPS = 1; /* 1:8 prescaler */
    PR4 = 0xFFFF;
    PR5 = 0xFFFF; /* note an overflow */
    _T4IE = 0; /* no interrupts */
    _T5IE = 0;
    TMR4 = 0; /* start with counting at 0 */
    TMR5 = 0;
    T4CONbits.TON = 1; /* start timer */
}

u32 systemTicks( )
{
    u32 ticks = TMR5;
    ticks <<= 16;
    ticks |= TMR4;
    return ticks;
}

void debugInitialize( )
{
    systemTicksInitialize( );

    /* Disable UART for configuration */
    debugDeinitialize();

    /* Setup UART registers */
    /* equation according to the datasheet:
       (sysclk / (16 * baudrate)) - 1
     */
    U1BRG = UART_BRG;

    /* Enable UART */
    U1MODE = UART_TXENABLE;
    U1STA  |= UART_TXAUTO;
}

void debugDeinitialize()
{
    /* disable UART */
    U1MODE = 0x0;
    U1STA = 0x0;
}

void debugByte( u8 dat )
{
    while ( U1STA & UART_TXFULL ) ; /* wait until transmit buffer is no longer full */
    U1TXREG = dat;
}

#define UART_NIBBLE_TO_HEX(nibble) \
  ( (nibble) < 10 ? '0' + (nibble) : 'A' - 10 + (nibble) )

void debugByteAsNumber( u8 number )
{
    char num[ 3 ];

    num[2] = '\0';
    num[1] = UART_NIBBLE_TO_HEX( number & 0xF );
    num[0] = UART_NIBBLE_TO_HEX( (number >> 4 ) & 0xF );
    debugString( num );
}

void debugNumber( u16 number )
{
    char num[ 5 ];

    num[4] = '\0';
    num[3] = UART_NIBBLE_TO_HEX( number & 0xF );
    num[2] = UART_NIBBLE_TO_HEX( (number >> 4 ) & 0xF );
    num[1] = UART_NIBBLE_TO_HEX( (number >> 8 ) & 0xF );
    num[0] = UART_NIBBLE_TO_HEX( (number >> 12 ) & 0xF );
    debugString( num );
}


void debugString( const char * text )
{
    while ( *text != '\0' )
    {
        while ( U1STA & UART_TXFULL ) ; /* wait until transmit buffer is no longer full */
        U1TXREG = *text;
        ++text;
    }
}

void debugWithTime( const char * text )
{
    u32 ticks = systemTicks( );
    debugString( "\r\n[" );
    debugNumber( ticks >> 16 );
    debugNumber( ticks );
    debugString( "]" );
    debugString( text );
}
