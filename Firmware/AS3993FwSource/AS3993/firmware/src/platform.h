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
/*! \file
 *
 *  \author Bernhard Breinbauer
 *  \author Rene Eggerstorfer
 *
 *   \brief This file provides platform (board) specific macros and declarations
 *
 *   Contains configuration macros which are used throughout the code base,
 *   to enable/disable board specific features.
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include <p24Fxxxx.h>
#include "Compiler.h"
#include "global.h"
#include "as3993_config.h"


/*
******************************************************************************
* DEFINES
******************************************************************************
*/
/** define for stopMode parameter of writeReadAS3993() */
#define STOP_NONE               0
/** define for stopMode parameter of writeReadAS3993() */
#define STOP_SGL                1
/** define for stopMode parameter of writeReadAS3993() */
#define STOP_CONT               2

/** map as3993Isr to _INT1Interrupt */
#define as3993Isr _INT1Interrupt

/*! map timer2Isr to _T3Interrupt */
#define timer3Isr _T3Interrupt

/*! map timer4Isr to _T4Interrupt */
#define timer4Isr _T4Interrupt

/** Macro for enable external IRQ */
#define ENEXTIRQ()                _INT1IE = 1;

/** Macro for disable external IRQ */
#define DISEXTIRQ()               _INT1IE = 0

/** Macro for clearing external IRQ flag*/
#define CLREXTIRQ()               _INT1IF = 0



/** Macro for setting enable pin */
#define EN(x)                     ENABLE=(x)

/** Macro for setting PA on/off */
#define EN_PA(x)                  EN_PAPIN(x)

/** Macro for setting NCS pin, serial enable line */
#define NCS(x)                    NCSPIN=(x)
/** Macro for activating AS3993 for SPI communication */
#define NCS_SELECT()              NCS(0)
/** Macro for deactivating AS3993 for SPI communication */
#define NCS_DESELECT()            NCS(1)

#ifdef TUNER
#define SEN_TUNER_CIN(x)          SEN_CINPIN(x)
#define SEN_TUNER_CLEN(x)         SEN_CLENPIN(x)
#define SEN_TUNER_COUT(x)         SEN_COUTPIN(x)
#endif

/** Definition for the MCU Status LED */
#define MCULED(x)                 MCULEDPIN(x)

/** Definition of the general status LEDs */
#define NUMLEDS                   BOARD_NUMLEDS
#define LED1(x)                   LED1PIN(x)
#define LED2(x)                   LED2PIN(x)
#define LED3(x)                   LED3PIN(x)
#define LED4(x)                   LED4PIN(x)

#define LEDON                     0
#define LEDOFF                    1

// Select RX or TX in Direct Mode by setting a dedicated Pin to High or Low
#define DIRECT_MODE_ENABLE_SENDER()      DM_SW_RX_TX(LOW)
#define DIRECT_MODE_ENABLE_RECEIVER()    DM_SW_RX_TX(HIGH)


//definition of test points for debugging
#if EVALBOARD
#define TP1(x)  _LATB10=(x);
#elif FERMI
#define TP1(x)  _LATB4=(x);
#elif  FEMTO2 || FEMTO2_1
#define TP1(x)  _LATB4=(x);
#elif MEGA
#define TP1(x)  _LATF1=(x);
#elif RADON
/* not available :#define TP1(x)  _LATF1=(x); */
#elif NEWTON
/* not available; */
#endif

//definition of irq pins
#if EVALBOARD
#define AS3993_PORT_INT 0 //TODO: has do be defined for eval board
#elif FERMI
#define AS3993_PORT_INT  _RB14
#elif  FEMTO2 || FEMTO2_1
#define AS3993_PORT_INT  _RB14
#elif MEGA
#define AS3993_PORT_INT  _RD8
#elif RADON
#define AS3993_PORT_INT  _RC4
#elif NEWTON
#define AS3993_PORT_INT  _RB14
#endif

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
/*------------------------------------------------------------------------- */
/** This function initializes uController I/O ports for the configured board
  *
  * This function does not take or return a parameter.
  */
void platformInit(void);

/*------------------------------------------------------------------------- */
/** This function initializes SPI communication.
  *
  * This function does not take or return a parameter.
  */
void spiInit(void);

/*------------------------------------------------------------------------- */
/** This function talks with the AS3993 chip.
  */
void writeReadAS3993( const u8* wbuf, u8 wlen, u8* rbuf, u8 rlen, u8 stopMode, u8 doStart );

/** This function talks with the AS3993 chip from ISR.
  */
void writeReadAS3993Isr( const u8 *wbuf, u8 wlen, u8* rbuf, u8 rlen );

/*------------------------------------------------------------------------- */
/** This function sets the interface to the AS3993 for accessing it via
  * direct mode. 
  */
void setPortDirect();

/*------------------------------------------------------------------------- */
/** This function sets the interface to the AS3993 for accessing it via
  * normal SPI mode.
  */
void setPortNormal();



/*
******************************************************************************
* INTERNAL DEFINES
* Not to be used directly, use the macros at the top of this file
******************************************************************************
*/
#if EVALBOARD
/** Definition for the serial enable pin */
#define NCSPIN                    _LATB6
/** Definition for the Direct data mode Pin*/
#define IO3PIN                    _LATE3
/** Definition for the Direct data mode Pin*/
#define IO2PIN                    _LATE2
/** Definition for the Direct mode data bitclock Pin*/
#define IO5PIN                    _LATE5
/** Definition for the Direct mode data Pin*/
#define IO6PIN                    _LATE6

/** Definition for the enable pin */
#define ENABLE                    _LATD6
/** Definition for the 8 bit data outport */
#define DATAOUTPORT               LATE
/** Definition for the PA enable Pin */
#define EN_PAPIN(x)                  _LATG2=(x)

#define LED2PIN                   _LATF3
#define LED3PIN                   _LATD0
#define PA_ON                     HIGH
#define PA_OFF                    LOW


#elif FERMI
/** Definition for the serial enable pin */
#define NCSPIN                    _LATB15
/** Definition for the Direct data mode Pin*/

/** Definition for the enable pin */
#define ENABLE                    _LATB13
/** Definition for the PA enable Pin, this is a dummy pin as the
 * fermi reader pa is always enabled  */
#define EN_PAPIN(x)                  _LATB10=(x)

#define MCULEDPIN(x)              _LATB9=(x)

#define BOARD_NUMLEDS             3
#define LED1PIN(x)                _LATB4=(x)
#define LED2PIN(x)                _LATA4=(x)
#define LED3PIN(x)                _LATB5=(x)
#define LED4PIN(x)

#define PA_ON                     HIGH
#define PA_OFF                    LOW

/* Definition for the Direct Mode Pins */
#define DM_TX              _LATA1       // MOSI PIN
#define DM_RX(x)           _LATA0=(x)   // MISO PIN
#define DM_SW_RX_TX(x)     _LATB3=(x)   // SCLK PIN
#define DM_RX_CLK(x)       _LATB14=(x)  // IRQ PIN

#elif FEMTO2 
/** Definition for the serial enable pin */
#define NCSPIN                    _LATB15

/** Definition for the enable pin */
#define ENABLE                    _LATB8

#define MCULEDPIN(x)              _LATB4=(x)

#define BOARD_NUMLEDS             2
#define LED1PIN(x)                _LATA4=(x)
#define LED2PIN(x)                _LATA2=(x)
#define LED3PIN(x)
#define LED4PIN(x)

/** Antenna switch is used to toggle antenna port 1 and 2 */
#define SWITCH_ANTENNA(x)       _LATB2 = ((x)==2?SWITCH_ANT_P2:SWITCH_ANT_P1)
#define SWITCH_ANT_P1           0x0
#define SWITCH_ANT_P2           0x1

#define SEN_CINPIN(x)           _LATB5=(x)
#define SEN_CLENPIN(x)
#define SEN_COUTPIN(x)          _LATB9=(x)

#define PA_ON                     HIGH
#define PA_OFF                    LOW

/* Definition for the Direct Mode Pins */
#define DM_TX              _LATA1       // MOSI PIN
#define DM_RX(x)           _LATA0=(x)   // MISO PIN
#define DM_SW_RX_TX(x)     _LATB3=(x)   // SCLK PIN
#define DM_RX_CLK(x)       _LATB14=(x)  // IRQ PIN


#elif FEMTO2_1
/** Definition for the serial enable pin */
#define NCSPIN                    _LATB15

/** Definition for the enable pin */
#define ENABLE                    _LATB8

#define MCULEDPIN(x)              _LATB1=(x) //changed from V2.0 to V2.1

#define BOARD_NUMLEDS             2
#define LED1PIN(x)                _LATB0=(x) //changed from V2.0 to V2.1
#define LED2PIN(x)                _LATA2=(x)
#define LED3PIN(x)
#define LED4PIN(x)

/** Antenna switch is used to toggle antenna port 1 and 2 */
#define SWITCH_ANTENNA(x)       _LATB2 = ((x)==2?SWITCH_ANT_P2:SWITCH_ANT_P1)
#define SWITCH_ANT_P1           0x0
#define SWITCH_ANT_P2           0x1

#define SEN_CINPIN(x)           _LATB5=(x)
#define SEN_CLENPIN(x)
#define SEN_COUTPIN(x)          _LATB9=(x)

#define PA_ON                     HIGH
#define PA_OFF                    LOW

/* Definition for the Direct Mode Pins */
#define DM_TX              _LATA1       // MOSI PIN
#define DM_RX(x)           _LATA0=(x)   // MISO PIN
#define DM_SW_RX_TX(x)     _LATB3=(x)   // SCLK PIN
#define DM_RX_CLK(x)       _LATB14=(x)  // IRQ PIN

#elif MEGA
/** Definition for the serial enable pin */
#define NCSPIN                    _LATD9
/** Definition for the Direct data mode Pin*/

/** Definition for the enable pin */
#define ENABLE                    _LATA15
/** Definition for the PA enable Pin, this is a dummy pin as the
 * fermi reader pa is always enabled  */
#define EN_PAPIN(x)                  _LATA0=(x)

#define MCULEDPIN(x)              _LATG6=(x)

#define BOARD_NUMLEDS             3
#define LED1PIN(x)                _LATB8=(x)
#define LED2PIN(x)                _LATD15=(x)
#define LED3PIN(x)                _LATF5=(x)
#define LED4PIN(x)

#define PA_ON                     HIGH
#define PA_OFF                    LOW

/* Definition for the Direct Mode Pins */
#define DM_TX              _LATD11       // MOSI PIN
#define DM_RX(x)           _LATD10=(x)   // MISO PIN
#define DM_SW_RX_TX(x)     _LATD1=(x)   // SCLK PIN
#define DM_RX_CLK(x)       _LATD8=(x)  // IRQ PIN

#elif RADON
/** Definition for the serial enable pin */
#define NCSPIN                    _LATC5
/** Definition for the Direct data mode Pin*/

/** Definition for the enable pin */
#define ENABLE                    _LATC3
/** Definition for the PA enable Pin, this is a dummy pin as the
 * Radon reader pa is always enabled, use LED3 which this way serves as 
 * RF Power on LED  */
#define EN_PAPIN(x)               do{u8 r;LED3PIN(x);r = as3993SingleRead(AS3993_REG_RFOUTPUTCONTROL); r = ((x==PA_ON)?(r|0x20):(r&~0x20)); as3993SingleWrite(AS3993_REG_RFOUTPUTCONTROL,r);}while(0)

#define MCULEDPIN(x)              LED1PIN(x)
#define ANT1LEDPIN(x)             LED6PIN(x)
#define ANT2LEDPIN(x)             LED5PIN(x)
#define RFDATARECEIVELED(x)       LED4PIN(x)
#define REFLPOWERTOOHIGHLED(x)    LED2PIN(x)

#define BOARD_NUMLEDS             7
/* A1 is LED5(schematic) aka D13 */
#define LED1PIN(x)                _LATA1=(x)
/* A8 is LED4(schematic) aka D8 */
#define LED2PIN(x)                _LATA8=(x)
/* B4 is LED3(schematic) aka D9 */
#define LED3PIN(x)                _LATB4=(x)
/* A4 is LED2(schematic) aka D10 */
#define LED4PIN(x)                _LATA4=(x)
/* A9 is LED1(schematic) aka D7 */
#define LED5PIN(x)                _LATA9=(x)
/* C6 is LED6(schematic) aka D11 */
#define LED6PIN(x)                _LATC6=(x)

/** Antenna switch is used to toggle antenna port 1 and 2 */
#define SWITCH_ANTENNA(x)       (_LATC7 = ((x)==2?SWITCH_ANT_P2:SWITCH_ANT_P1), ANT2LEDPIN((x)==2), ANT1LEDPIN((x)!=2))
#define SWITCH_ANT_P1           0x0
#define SWITCH_ANT_P2           0x1

#define SEN_CINPIN(x)           _LATC0=(x)
#define SEN_CLENPIN(x)          _LATC1=(x)
#define SEN_COUTPIN(x)          _LATC2=(x)

#define SEN_ANT1_CINPIN(x)      _LATB2=(x)
#define SEN_ANT1_COUTPIN(x)     _LATB3=(x)
#define BUTTON                  _RA0


#define PA_ON                     LOW
#define PA_OFF                    HIGH

/* Definition for the Direct Mode Pins */
#define DM_TX              _LATB8       // MOSI PIN
#define DM_RX(x)           _LATB7=(x)   // MISO PIN
#define DM_SW_RX_TX(x)     _LATB9=(x)   // SCLK PIN
#define DM_RX_CLK(x)       _LATC4=(x)  // IRQ PIN


#elif NEWTON
/** Definition for the serial enable pin */
#define NCSPIN                    _LATB15
/** Definition for the Direct data mode Pin*/

/** Definition for the enable pin */
#define ENABLE                    _LATB13
/** Definition for the PA enable Pin, this is a dummy pin as the
 * fermi reader pa is always enabled  */
#define EN_PAPIN(x)                  _LATB10=(x)

#define BUTTON                    _RB4

#if AS3993DEBUG
#define BOARD_NUMLEDS             1
#define MCULEDPIN(x)              LED1PIN(x)
#define LED1PIN(x)               _LATA4=(x)
#define LED2PIN(x)               
#define LED3PIN(x)               
#define LED4PIN(x)         
#else
#define BOARD_NUMLEDS             3
#define MCULEDPIN(x)              LED1PIN(x)
#define LED1PIN(x)               _LATB0=(x)
#define LED2PIN(x)               _LATB1=(x)
#define LED3PIN(x)               _LATA4=(x)
#define LED4PIN(x)    
#endif
        
#define PA_ON                     HIGH
#define PA_OFF                    LOW

/* Definition for the Direct Mode Pins */
#define DM_TX              _LATA1   // MOSI PIN
#define DM_RX(x)           _LATA0=(x)   // MISO PIN
#define DM_SW_RX_TX(x)     _LATB3=(x)   // SCLK PIN
#define DM_RX_CLK(x)       _LATB14=(x)  // IRQ PIN

#define SWITCH_ANT_A1           0x0 // Internal Antenna
#define SWITCH_ANT_P1           0x1 // External Antenna
#define SWITCH_ANTENNA(x)       _LATB9 = ((x)==2?SWITCH_ANT_A1:SWITCH_ANT_P1)

#endif

#endif /* PLATFORM_H_ */
