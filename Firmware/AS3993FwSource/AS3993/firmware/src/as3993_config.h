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
 *  \author U.Herrmann ( based on work by E.Grubmueller )
 *  \author T. Luecker (Substitute)
 *  \author B. Bernhard
 *
 *   \brief Configuration file for all AS99x firmware
 *
 *   Do configuration of the driver in here.
 */

#ifndef _AS3993_CONFIG_H
#define _AS3993_CONFIG_H

/***************************************************************************/
/********************** configuration area *********************************/
/***************************************************************************/

/** Define this to 1 if you have a FERMI board */
#define FERMI 0

/** Define this to 1 if you have an (ams internal) eval board */
#define EVALBOARD 0

/** Define this to 1 if you have a FEMTO v2 board */
#define FEMTO2 0

/** Define this to 1 if you have a FEMTO v2.1 board */
#define FEMTO2_1 0

/** Define this to 1 if you have a MEGA board */
#define MEGA 0

/** Define this to 1 if you have a RADON board */
#define RADON 1

/** Define this to 1 if you have a NEWTON board */
#define NEWTON 0

/** Define this to 1 to enable AS3993 support. */
#define RUN_ON_AS3993 1

/** Define this to 1 to enable AS3994 support. */
#define RUN_ON_AS3994 0

/** Define this to 1 to enable AS3980 support. */
#define RUN_ON_AS3980 0

/** Define this to 1 if as3993Initialize() should perform a proper selftest,
  testing connection AS3993, crystal, pll, antenna */
#define AS3993_DO_SELFTEST 1

/** Define this to 1 if you want debug messages in as3993.c or want to have
 bebug output on OAD pins*/
#define AS3993DEBUG 0

/** If set to 1 reader intialization will be more verbose. */
#define VERBOSE_INIT 0

/** Set this to 1 to enable iso6b support. */
#if RUN_ON_AS3993
#define ISO6B 1
#else
#define ISO6B 0
#endif

#if ISO6B
#define CRC16_ENABLE 1
#endif

/** Baudrate used if uart communication is enabled at compile time. */
#define BAUDRATE 115200UL

/***************************************************************************/
/******************** private definitions, not to be changed ***************/
/***************************************************************************/
#define EVAL_HW_ID          1
#define FERMI_HW_ID         2
#define FERMI_BIG_HW_ID     3
#define FEMTO_1V0_HW_ID     4
#define FEMTO_1V1_HW_ID     5
#define FEMTO_2V0_HW_ID     6
#define MEGA_HW_ID          7
#define RADON_HW_ID         8
#define FEMTO_2V1_HW_ID     9
#define NEWTON_HW_ID        10
#if RUN_ON_AS3993
#define CHIP                "AS3993"
#elif RUN_ON_AS3994
#define CHIP                "AS3994"
#elif RUN_ON_AS3980
#define CHIP                "AS3980"
#endif

#define FIRMWARE_ID         CHIP" Reader Firmware"
#define FIRMWARE_VERSION    0x020400UL;

#if FERMI
#define HARDWARE_ID         CHIP" FERMI Reader Hardware 1.0"
#define HARDWARE_ID_NUM     FERMI_HW_ID
#endif

#if EVALBOARD
#define HARDWARE_ID         CHIP" EVAL Reader Hardware 1.0"
#define HARDWARE_ID_NUM     EVAL_HW_ID
#endif

#if FEMTO2
#define HARDWARE_ID         CHIP" FEMTO Reader Hardware 2.0"
#define HARDWARE_ID_NUM     FEMTO_2V0_HW_ID
#endif

#if FEMTO2_1
#define HARDWARE_ID         CHIP" FEMTO Reader Hardware 2.1"
#define HARDWARE_ID_NUM     FEMTO_2V1_HW_ID
#endif

#if MEGA
#define HARDWARE_ID         CHIP" MEGA Reader Hardware 1.0"
#define HARDWARE_ID_NUM     MEGA_HW_ID
#endif

#if RADON
#define HARDWARE_ID         CHIP" RADON Reader Hardware 1.0"
#define HARDWARE_ID_NUM     RADON_HW_ID
#endif

#if NEWTON
#define HARDWARE_ID         CHIP" NEWTON Reader Hardware 1.0"
#define HARDWARE_ID_NUM     NEWTON_HW_ID
#endif

#define WRONG_CHIP_ID       "caution wrong chip"

/** define to identify Cin cap of tuner. */
#define TUNER_CIN       0x01
/** define to identify Clen cap of tuner. */
#define TUNER_CLEN      0x02
/** define to identify Cout cap of tuner. */
#define TUNER_COUT      0x04

/* Set the following configuration switches depending on the board setup:
 * INTVCO or EXTVCO
 * INTPA or EXTPA
 * SINGLEINP or BALANCEDINP
 * ANTENNA_SWITCH if 2 antenna ports are available
 * TUNER if antenna tuning is available
 * TUNER_CONFIG configures which tuner caps are available (bitmask)
*/
#if FERMI
#define INTVCO
#define EXTPA
#define BALANCEDINP
#elif EVALBOARD
#define INTVCO
#define EXTPA
#define BALANCEDINP
#elif FEMTO2 || FEMTO2_1
#define INTVCO
#define INTPA
#define SINGLEINP
#define ANTENNA_SWITCH
#define TUNER
#define TUNER_CONFIG (TUNER_CIN | TUNER_COUT)
#elif MEGA
#define INTVCO
#define EXTPA
#define SINGLEINP
#elif RADON
#define INTVCO
#define EXTPA
#define BALANCEDINP
#define ANTENNA_SWITCH
#define TUNER
#define TUNER_CONFIG (TUNER_CIN | TUNER_CLEN | TUNER_COUT)
#elif NEWTON
#define INTVCO
#define EXTPA
#define SINGLEINP
#define ANTENNA_SWITCH
#endif

/***************************************************************************/
/******************** check for configuration errors ***********************/
/***************************************************************************/
#if (FERMI + EVALBOARD + FEMTO2 + FEMTO2_1 + MEGA + RADON + NEWTON) > 1
#error "FW is configured for more than 1 board at the same time"
#endif
#if !FERMI && !EVALBOARD && !FEMTO2 && !FEMTO2_1 && !MEGA && !RADON && !NEWTON
#error unknown board
#endif

#if (RUN_ON_AS3993 + RUN_ON_AS3980 + RUN_ON_AS3994) > 1
#error "FW is configured for more than 1 chip at the same time"
#endif
#if !RUN_ON_AS3993 && !RUN_ON_AS3980 && !RUN_ON_AS3994
#error "unknown device"
#endif

/*!
  \mainpage
  \section Layering
  This software is layered as follows:

  <table cellpadding=20 cellspacing=10 rules="none">
  <tr>
      <td align="center" colspan=3 bgcolor="#FCCE9C">\link appl_commands.c application code\endlink</td>
      <td>Upper layer in which the application run. In reference implementation it is
      \link appl_commands.c appl_commands.c\endlink which handles communication with PC.
            All commands available are detailed in there.</td>
  </tr>
  <tr>
      <td align="center" bgcolor="#04FEFC">\link gen2.h gen2\endlink</td>
      <td align="center" bgcolor="89C2FC">\link iso6b.h iso6b\endlink</td>
      <td align="center" bgcolor="CCFECC">\link as3993_public.h as3993_public\endlink</td>
      <td>Protocol layer in which the protocol handling is performed</td>
  </tr>
  <tr>
      <td align="center" colspan=3 bgcolor="CCFECC">\link as3993.h as3993\endlink</td>
      <td>Device specific procedures of UHF Reader</td>
  </tr>
  <tr>
      <td  align="center" bgcolor="FCFE9C" colspan=3>\link platform.h platform\endlink</td>
      <td>Platform layer for communication with UHF Reader, handling CPU specifics, ...</td>
  </tr>
  </table>

  The files gen2.h and iso6b.h contain the interface provided to the
  user for the two supported protocols. They rely on the interface provide
  by as3993.h. This in turn relies on the actual hardware communication
  interface provided by platform.h, spi_driver.h, timer.h, ...

  \section Applicaton
  The application layer in this reference implementation is implemented as
  a dispatcher which waits for commands from the host (either via USB or UART)
  and processes commands accordingly. Documentation of the protocol is available
  in appl_commands.c.

  \section Configuration
  Configuration of this software is done in as3993_config.h and by defines
  in the build environment. The currenlty supported defines in the build
  environment are:
   - USE_LOGGER: enables UART logging functionality
   - USE_UART_STREAM_DRIVER: uses UART instead of USB for commands rx/tx
   - CPU: defaults to 24FJ64GB002
   - ENABLE_BOOTLOADER: defaults to enabled.
 
  A detailed explanation of the build environment is included in 
  \ref buildconf "HowtoBuild.txt".

  \section Using
  Before using any other functions the AS3993 needs to be initialized
  using as3993Initialize(). If initialization of AS3993 fails the return value
  of as3993Initialize() is not zero.\n
  After successful initialization functions from gen2.h and iso6b.h can
  be called.

  \section Porting
  The developer which needs to port this firmware to another hardware will need
  to change platform.c, spi_driver.c, timer.c + related .h files and adapt
  the host-communication related files (UART/USB). Additionally
  he should also provide a proper interrupt service routine (currently
  as3993Isr() in as3993.c).


*/
#endif
