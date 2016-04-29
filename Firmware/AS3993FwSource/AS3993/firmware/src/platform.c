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
/** @file
  * @brief Implementation of serial interface communication with AS3993.
  *
  * The file provides functions to initialize the uController and the board
  * for SPI communication with AS3993. The function writeReadAS3993() provides
  * communication functionality. The function writeReadAS3993Isr() reads 
  * interrupt status register and is only called from the ISR.
  * (no reentrant function necessary).
  * \n
  * For porting to another uController/board the functions in this file have to be
  * adapted.
  * \n\n
  *
  * @author Ulrich Herrmann
  * @author Bernhard Breinbauer
  */

#include "as3993_config.h"
#include "platform.h"
#include "global.h"
#include "logger.h"
#include "timer.h"
#include "spi_driver.h"
#include <PPS.h>


/*------------------------------------------------------------------------- */
/** This function initialising the external interrupt for comunication with the
  * AS3993. \n
  * This function does not take or return a parameter.
  */
static void initExtInterrupt(void )
{
    u8 a;
    // initialize IRQ interrupt
    // source for interrupt is set up in board-corresponding initXXX function

    _INT1EP  = 0;       // on positive edge
    _INT1IP  = 6;       // high priority
    _INT1IF  = 0;       // clear pending interrupts
    for (a = 100; a; a--);  // delay
    _INT1IE = 1;
}

#if EVALBOARD
static void initEvalBoard(void)
{
    TRISB = 0xFE9B;     // TX, MOSI, SCLK, NCS output
    TRISC = 0xFFFF;
    TRISD = 0x00BE;
    TRISE = 0xFFFC;     // IO1, IO0 output
    TRISF = 0xFFD5;     // LED4, LED1, LED2 output
#ifdef TP1
    _TRISB10 = 0;
    _LATB10 = 0;
#endif
    //configure SPI
    _LATB2 = 0;		// IO7(MOSI) low
    OUT_PIN_PPS_RP18 = OUT_FN_PPS_SCK1OUT;// assign RP18 (RB5) to SPI1 CLK output
    OUT_PIN_PPS_RP13 = OUT_FN_PPS_SDO1; // assign RP13 (RB2) to SPI1 data output (MOSI)
    IN_FN_PPS_SDI1 = IN_PIN_PPS_RP28;   // assign RP28 (RB4) to SPI1 data input (MISO)

    // setup external interrupt source
    IN_FN_PPS_INT1 = IN_PIN_PPS_RP20;   // RP20 for source INT1

    // setup UART pin for debugging
    OUT_PIN_PPS_RP8 = OUT_FN_PPS_U1TX;  // connect function U1TX to RP8
}
#endif

#if FERMI
static void initFermi(void)
{
    // set as output
    /*
     * set the following PORTA pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RA5 to RA15 are not routed to a pin.
     * RA4,    ,  9, OUT, GP2 (LED D2)
     * RA3,    ,  7, OUT, OSCO
     * RA2,    ,  6,  IN, OSCI
     * RA1, RP6, 28, OUT, MOSI
     * RA0, RP5, 27,  IN, MISO
     */
    TRISA = 0xFFE5;

    /*
     * set the following PORTB pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RB15, RP15, 23  OUT, NCS
     * RB14, RP14, 22,  IN, IRQ
     * RB13, RP13, 21, OUT, EN
     * RB12,     ,   ,  IN, n. c.
     * RB11, RP11, 19,  IN, USB D-
     * RB10, RP10, 18,  IN, USB D+
     * RB9 , RP9 , 15, OUT, MCULED
     * RB8 , RP8 , 14, OUT, UART1TX
     * RB7 , RP7 , 13,  IN, UART1RX
     * RB6 ,     ,   ,  IN, n. c.
     * RB5 ,     , 11, OUT, GP3 (LED D3)
     * RB4 , RP4 ,  8, OUT, GP1 (LED D1)
     * RB3 , RP3 ,  4, OUT, CLK
     * RB2 , RP2 ,  3,  IN, CLSYS
     * RB1 ,     ,  2,  IN, debugger
     * RB0 ,     ,  1,  IN, debugger
     */
    TRISB = 0x5CC7;

#ifdef TP1
    _TRISB4 = 0;
    _LATB4 = 0;
#endif
    //configure SPI
    _LATA1 = 0;		// IO7(MOSI) low
    OUT_PIN_PPS_RP3 = OUT_FN_PPS_SCK1OUT;// assign RP3 (RB3) to SPI1 CLK output
    OUT_PIN_PPS_RP6 = OUT_FN_PPS_SDO1;  // assign RP6 (RA1) to SPI1 data output (MOSI)
    IN_FN_PPS_SDI1 = IN_PIN_PPS_RP5; 	// assign RP5 (RA0) to SPI1 data input (MISO)

    // setup external interrupt source
    IN_FN_PPS_INT1 = IN_PIN_PPS_RP14;   // RP14 (RB14) for source INT1

    // setup UART pin for debugging
    OUT_PIN_PPS_RP8 = OUT_FN_PPS_U1TX;  // connect function U1TX to RP8
#if USE_UART_STREAM_DRIVER
    IN_FN_PPS_U1RX = IN_PIN_PPS_RP7;
#endif
}
#endif

#if FEMTO2
static void initFemto2(void)
{
    // set as output
    /*
     * set the following PORTA pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RA5 to RA15 are not routed to a pin.
     * RA4,    ,  9, OUT, GP2 (LED D2)
     * RA3,    ,  7, OUT, OSCO
     * RA2,    ,  6,  IN, OSCI
     * RA1, RP6, 28, OUT, MOSI
     * RA0, RP5, 27,  IN, MISO
     */
    TRISA = 0xFFE5;

    /*
     * set the following PORTB pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RB15, RP15, 23  OUT, NCS
     * RB14, RP14, 22,  IN, IRQ
     * RB13, RP13, 21, OUT, UART1TX
     * RB12,     ,   ,  IN, n. c.
     * RB11, RP11, 19,  IN, USB D-
     * RB10, RP10, 18,  IN, USB D+
     * RB9 , RP9 , 15, OUT, SEN_TUNE2
     * RB8 , RP8 , 14, OUT, EN
     * RB7 , RP7 , 13,  IN, UART1RX
     * RB6 ,     ,   ,  IN, n. c.
     * RB5 ,     , 11, OUT, SEN_TUNE1
     * RB4 , RP4 ,  8, OUT, GP1 (LED D1)
     * RB3 , RP3 ,  4, OUT, CLK
     * RB2 , RP2 ,  3, OUT, SW_ANT
     * RB1 ,     ,  2,  IN, debugger
     * RB0 ,     ,  1,  IN, debugger
     */
    TRISB = 0x5CC3;

#ifdef TP1
    _TRISB4 = 0;
    _LATB4 = 0;
#endif
    //configure SPI
    _LATA1 = 0;		// IO7(MOSI) low
    _LATB2 = 0;         // default: switch to ext antenna
    OUT_PIN_PPS_RP3 = OUT_FN_PPS_SCK1OUT;// assign RP3 (RB3) to SPI1 CLK output
    OUT_PIN_PPS_RP6 = OUT_FN_PPS_SDO1;  // assign RP6 (RA1) to SPI1 data output (MOSI)
    IN_FN_PPS_SDI1 = IN_PIN_PPS_RP5; 	// assign RP5 (RA0) to SPI1 data input (MISO)

    // setup external interrupt source
    IN_FN_PPS_INT1 = IN_PIN_PPS_RP14;   // RP14 (RB14) for source INT1

    // setup UART pin for debugging
    OUT_PIN_PPS_RP13 = OUT_FN_PPS_U1TX;  // connect function U1TX to RP13
#if USE_UART_STREAM_DRIVER
    IN_FN_PPS_U1RX = IN_PIN_PPS_RP7;
#endif
}
#endif

#if FEMTO2_1
static void initFemto2_1(void)
{
    // set as output
    /*
     * set the following PORTA pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RA5 to RA15 are not routed to a pin.
     * RA4,    ,  9, OUT, GP2 (LED D2)
     * RA3,    ,  7, OUT, OSCO
     * RA2,    ,  6,  IN, OSCI
     * RA1, RP6, 28, OUT, MOSI
     * RA0, RP5, 27,  IN, MISO
     */
    TRISA = 0xFFE5;

    /*
     * set the following PORTB pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RB15, RP15, 23  OUT, NCS
     * RB14, RP14, 22,  IN, IRQ
     * RB13, RP13, 21, OUT, UART1TX
     * RB12,     ,   ,  IN, n. c.
     * RB11, RP11, 19,  IN, USB D-
     * RB10, RP10, 18,  IN, USB D+
     * RB9 , RP9 , 15, OUT, SEN_TUNE2
     * RB8 , RP8 , 14, OUT, EN
     * RB7 , RP7 , 13,  IN, UART1RX
     * RB6 ,     ,   ,  IN, n. c.
     * RB5 ,     , 11, OUT, SEN_TUNE1
     * RB4 , RP4 ,  8, OUT, GP1 (LED D1)
     * RB3 , RP3 ,  4, OUT, CLK
     * RB2 , RP2 ,  3, OUT, SW_ANT
     * RB1 ,     ,  2,  IN, debugger
     * RB0 ,     ,  1,  IN, debugger
     */
    TRISB = 0x5CC0;

#ifdef TP1
    _TRISB4 = 0;
    _LATB4 = 0;
#endif
    //configure SPI
    _LATA1 = 0;		// IO7(MOSI) low
    _LATB2 = 0;         // default: switch to ext antenna
    OUT_PIN_PPS_RP3 = OUT_FN_PPS_SCK1OUT;// assign RP3 (RB3) to SPI1 CLK output
    OUT_PIN_PPS_RP6 = OUT_FN_PPS_SDO1;  // assign RP6 (RA1) to SPI1 data output (MOSI)
    IN_FN_PPS_SDI1 = IN_PIN_PPS_RP5; 	// assign RP5 (RA0) to SPI1 data input (MISO)

    // setup external interrupt source
    IN_FN_PPS_INT1 = IN_PIN_PPS_RP14;   // RP14 (RB14) for source INT1

    // setup UART pin for debugging
    OUT_PIN_PPS_RP13 = OUT_FN_PPS_U1TX;  // connect function U1TX to RP13
#if USE_UART_STREAM_DRIVER
    IN_FN_PPS_U1RX = IN_PIN_PPS_RP7;
#endif
}
#endif

#if MEGA
static void initMega(void)
{
    // set as output
    /*
     * set the following PORTA pin(s)as IN/OUT:
     * only used pins are shown
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RA15,   , 67, OUT, EN
     */
    TRISA = 0x7FFF;

    /*
     * set the following PORTB pin(s)as IN/OUT:
     * only used pins are shown.
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RB8 , RP8 , 32, OUT, GP2
     */
    TRISB = 0xFEFF;

    /*
     * no PORTC pins are used
     */
    TRISC = 0xFFFF;

    /*
     * set the following PORTD pin(s)as IN/OUT:
     * only used pins are shown
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RD15, RP5 , 48, OUT, GP3 - UART TX
     * RD11, RP12, 71, OUT, MOSI
     * RD10, RP3 , 70,  IN, MISO
     * RD9 , RP4 , 69, OUT, NCS
     * RD8 , RP2 , 68,  IN, IRQ
     * RD5 , RP20, 82, OUT, GP1
     * RD1 , RP24, 76, OUT, SCLK
     * RD0 , RP11, 72,  IN, CLSYS/GP
     */
    TRISD = 0x75DD;

    /*
     * set the following PORTF pin(s)as IN/OUT:
     * only used pins are shown
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RF5 , RP17, 50,  IN, GP4 - UART RX
     * RF1 ,     , 88, OUT, GP6
     */
    TRISF = 0xFFFD;

    /*
     * set the following PORTG pin(s)as IN/OUT:
     * only used pins are shown
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RG6 , RP21, 10, OUT, GP1
     */
    TRISG = 0xFFBF;

#ifdef TP1
    _TRISF1 = 0;
    _LATF1 = 0;
#endif
    //configure SPI
    OUT_PIN_PPS_RP24 = OUT_FN_PPS_SCK1OUT;// assign RP24 (RD1) to SPI1 CLK output
    OUT_PIN_PPS_RP12 = OUT_FN_PPS_SDO1;  // assign RP12 (RD11) to SPI1 data output (MOSI)
    IN_FN_PPS_SDI1 = IN_PIN_PPS_RP3; 	// assign RP3 (RD10) to SPI1 data input (MISO)

    // setup external interrupt source
    IN_FN_PPS_INT1 = IN_PIN_PPS_RP2;   // RP2 (RD8) for source INT1

    // setup UART pin for debugging
    OUT_PIN_PPS_RP5 = OUT_FN_PPS_U1TX;  // connect function U1TX to RP8
#if USE_UART_STREAM_DRIVER
    IN_FN_PPS_U1RX = IN_PIN_PPS_RP17;
#endif
}
#endif

#if RADON
static void initRadon(void)
{
    /*
     * set the following PORTA pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     *  RA0,    RP5,    19,   IN  , Button, CN2
     *  RA1,    RP6,    20,  OUT  , LED5(sch), LED1(sw)
     *  RA2,       ,    30,   IN  , OSCI
     *  RA3,       ,    31,   IN  , OSCO
     *  RA4,       ,    34,  OUT  , LED2(sch), LED4(sw)
     *  RA5,       ,    --,   IN  , n.c.
     *  RA6,       ,    --,   IN  , n.c.
     *  RA7,       ,    13,   IN  , gnd
     *  RA8,       ,    32,  OUT  , LED4(sch), LED2(sw)
     *  RA9,       ,    35,  OUT  , LED1(sch), LED5(sw)
     *  RA10,      ,    14,   IN  , gnd
     */
    TRISA = 0xfced;
    _CN2PUE = 1; /* Enable Pull up for button */

    /*
     * set the following PORTB pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     *  RB0,    RP0,    21,   IN  , PGED1
     *  RB1,    RP1,    22,   IN  , PGEC1
     *  RB2,    RP2,    23,  OUT  , EN_T_1
     *  RB3,    RP3,    24,  OUT  , EN_T_2
     *  RB4,    RP4,    33,  OUT  , LED3(sch), LED3(sw)
     *  RB5,       ,    11,   IN  , USBID
     *  RB6,       ,    --,   IN  , 
     *  RB7,    RP7,    43,   IN  , MISO
     *  RB8,    RP8,    44,  OUT  , MOSI
     *  RB9,    RP9,     1,  OUT  , CLK
     *  RB10,  RP10,     8,   IN  , D_P(USB)
     *  RB11,  RP11,     9,   IN  , D_M(USB)
     *  RB12,      ,    --,   IN  , n.c.
     *  RB13,  RP13,    11,   IN  , gnd
     *  RB14,  RP14,    14,   IN  , gnd
     *  RB15,  RP15,    15,   IN  , gnd
     */
    TRISB = 0xfee3;

    /*
     * set the following PORTB pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     *  RC0,   RP16,    25,  OUT  , ETC_1
     *  RC1,   RP17,    26,  OUT  , ETC_2
     *  RC2,   RP18,    27,  OUT  , ETC_3
     *  RC3,   RP19,    36,  OUT  , EN
     *  RC4,   RP20,    37,   IN  , IRQ
     *  RC5,   RP21,    38,  OUT  , NCS
     *  RC6,   RP22,     2,  OUT  , LED6(sch), LED6(sw)
     *  RC7,   RP23,     3,  OUT  , SW_ANT
     *  RC8,   RP24,     4,  OUT  , UART_TX 
     *  RC9,   RP25,     5,   IN  , UART_RX
     */
    TRISC = 0xfe10;

    //configure SPI
    OUT_PIN_PPS_RP9 = OUT_FN_PPS_SCK1OUT;// assign RP9 to SPI1 CLK output
    OUT_PIN_PPS_RP8 = OUT_FN_PPS_SDO1;  // assign RP8 to SPI1 data output (MOSI)
    IN_FN_PPS_SDI1 = IN_PIN_PPS_RP7; 	// assign RP7 to SPI1 data input (MISO)

    // setup external interrupt source
    IN_FN_PPS_INT1 = IN_PIN_PPS_RP20;   // RP20 for source INT1

    SWITCH_ANTENNA(2); /* Ant 2 should be default */

    // setup UART pin for debugging
    OUT_PIN_PPS_RP24 = OUT_FN_PPS_U1TX;  // connect function U1TX to RP24
#if USE_UART_STREAM_DRIVER
    IN_FN_PPS_U1RX = IN_PIN_PPS_RP25;
#endif
}
#endif

#if NEWTON
static void initNewton(void)
{
    // set as output
    /*
     * set the following PORTA pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RA5 to RA15 are not routed to a pin.
     * RA4,    ,  9, OUT, GP2 (LED D3)
     * RA3,    ,  7, OUT, OSCO
     * RA2,    ,  6,  IN, OSCI
     * RA1, RP6, 28, OUT, MOSI
     * RA0, RP5, 27,  IN, MISO
     */
    TRISA = 0xFFE5;

    /*
     * set the following PORTB pin(s)as IN/OUT:
     * PortPin, RPx, PinNr, IN/OUT, Description
     * ----------------------------------------
     * RB15, RP15, 23  OUT, NCS
     * RB14, RP14, 22,  IN, IRQ
     * RB13, RP13, 21, OUT, EN
     * RB12,     ,   ,  IN, n. c.
     * RB11, RP11, 19,  IN, USB D-
     * RB10, RP10, 18,  IN, USB D+
     * RB9 , RP9 , 15, OUT, SW_ANT
     * RB8 , RP8 , 14, OUT, UART1TX
     * RB7 , RP7 , 13,  IN, UART1RX
     * RB6 ,     ,   ,  IN, n. c.
     * RB5 ,     , 11, OUT, G8
     * RB4 , RP4 ,  8,  IN, BUTTON
     * RB3 , RP3 ,  4, OUT, CLK
     * RB2 , RP2 ,  3, OUT, G16
     * RB1 ,     ,  2,  IN, debugger PGED1(LED D1)
     * RB0 ,     ,  1,  IN, debugger PGEC1(LED D2)
     */
#if AS3993DEBUG
    TRISB = 0x5CD3;
#else
    TRISB = 0x5CD0;
#endif

    _CN1PUE = 1; /* Enable Pull up for button */
#ifdef TP1
#endif
    
    //configure SPI
    _LATA1 = 0;		// IO7(MOSI) low
    OUT_PIN_PPS_RP3 = OUT_FN_PPS_SCK1OUT;// assign RP3 (RB3) to SPI1 CLK output
    OUT_PIN_PPS_RP6 = OUT_FN_PPS_SDO1;  // assign RP6 (RA1) to SPI1 data output (MOSI)
    IN_FN_PPS_SDI1 = IN_PIN_PPS_RP5; 	// assign RP5 (RA0) to SPI1 data input (MISO)

    // setup external interrupt source
    IN_FN_PPS_INT1 = IN_PIN_PPS_RP14;   // RP14 (RB14) for source INT1

    //  G8 and G16 to high
    _LATB5 = 1; //G8
    _LATB2 = 1; //G16
    // SW ANT
    _LATB9 = 1;

    // setup UART pin for debugging
    OUT_PIN_PPS_RP8 = OUT_FN_PPS_U1TX;  // connect function U1TX to RP8
#if USE_UART_STREAM_DRIVER
    IN_FN_PPS_U1RX = IN_PIN_PPS_RP7;
#endif
}
#endif

/*------------------------------------------------------------------------- */
void platformInit(void)
{
    // configure ports
    AD1PCFG = 0xFFFF;   // all I/O digital
#if EVALBOARD
    initEvalBoard();
#elif FERMI
    initFermi();
#elif FEMTO2
    initFemto2();
#elif FEMTO2_1
    initFemto2_1();
#elif MEGA
    initMega();
#elif RADON
    initRadon();
#elif NEWTON
    initNewton();
#endif
    EN(LOW);
    NCS_DESELECT();

    initExtInterrupt();

    delay_us(100);
}

/*------------------------------------------------------------------------- */
void spiInit(void)
{
    spiConfig_t spiconf;

    // setup spi configuration
    spiconf.frequency = 4000000ULL;
    spiconf.instance = SPI1;
    spiconf.clockPhase = 1;
    spiconf.clockPolarity = 0;
    spiconf.deviceId = 0;

    spiInitialize(&spiconf, 0);
}

/*------------------------------------------------------------------------- */
void writeReadAS3993( const u8* wbuf, u8 wlen, u8* rbuf, u8 rlen, u8 stopMode, u8 doStart )
{
#if AS3993DEBUG
    if (!ENABLE)
        LOG("******* attempting SPI with EN low! ******\n");
#endif
    if (doStart) NCS_SELECT();

    spiTxRx(wbuf, 0, wlen);
    if (rlen)
        spiTxRx(0, rbuf, rlen);

    if (stopMode != STOP_NONE) NCS_DESELECT();
}

/*------------------------------------------------------------------------- */
void writeReadAS3993Isr( const u8* wbuf, u8 wlen, u8* rbuf, u8 rlen )
{
#if AS3993DEBUG
    if (!ENABLE)
        LOG("******* attempting SPI with EN low (ISR)! ******\n");
#endif

    NCS_SELECT();

    spiTxRx(wbuf, 0, wlen);
    spiTxRx(0, rbuf, rlen);

    NCS_DESELECT();
}


void setPortDirect()
{
    spiDeinitialize();
}

void setPortNormal()
{
    spiInit();
}
