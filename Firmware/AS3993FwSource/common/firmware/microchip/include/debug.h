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
 *  \brief simple services to print e.g. debugging messages or have a system tick
 *
 */
/*!
 * \defgroup debug
 */

#ifndef DEBUG_H
#define DEBUG_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "ams_types.h"

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

extern void systemTicksInitialize( );
extern u32 systemTicks( );


/*! \ingroup debug
 *****************************************************************************
 *  \brief  Initialize debug service
 *
 *  This function initializes the UART interface, it assumes that the
 *  system clock is 16 MHZ and the desired baudrate is 115200
 *  and enables transmission only.
 *  \note If you have a different system clock frequency or need another
 *  baudrate, please make a copy for your project.
  *****************************************************************************
 */
extern void debugInitialize( );


/*! \ingroup debug
 *****************************************************************************
 *  \brief  Deinitialize debug interface
 *
 *  Calling this function deinitializes the debug interface.
 *
 *****************************************************************************
 */
extern void debugDeinitialize();

/*! \ingroup debug
 *****************************************************************************
 *  \brief  Transmit a zero-termianted string
 *
 *  This function is used to transmit a zero-terminated string.
 *
 *  \param[in] text: zero-terminated string
 *****************************************************************************
 */
extern void debugString(const char * text);

/*! \ingroup debug
 *****************************************************************************
 *  \brief  Write time and zero-termianted string
 *
 *  This function is used to transmit a zero-terminated string.
 *
 *  \param[in] text: zero-terminated string
 *****************************************************************************
 */
extern void debugWithTime(const char * text);


/*! \ingroup debug
 *****************************************************************************
 *  \brief  transmit a single byte
 *
 *  This function is used to transmit a single byte. Note that this function
 * does *not* wait until the byte is transmitted. However the function will
 * wait if the transmit fifo is full - it waits until the fifo has 1 free
 * place.
 *
 *  \param[in] dat: byte to be transmitted.
 *
 *****************************************************************************
 */
extern void debugByte(u8 dat);

/*! \ingroup debug
 *****************************************************************************
 *  \brief  transmit a number encoded in ascii (i.e. 4 bytes are transmitted)
 *
 *  This function is used to transmit a word. Note that this function
 * does *not* wait until the word is transmitted. However the function will
 * wait if the transmit fifo is full - it waits until the fifo has 1 free
 * place.
 *
 *  \param[in] number: word to be converted and transmitted.
 *
 *****************************************************************************
 */
extern void debugNumber(u16 number);

/*! \ingroup debug
 *****************************************************************************
 *  \brief  transmit a number encoded in ascii (i.e. 2 bytes are transmitted)
 *
 *  This function is used to transmit a byte. Note that this function
 * does *not* wait until the byte is transmitted. However the function will
 * wait if the transmit fifo is full - it waits until the fifo has 1 free
 * place.
 *
 *  \param[in] number: byte to be converted and transmitted.
 *
 *****************************************************************************
 */
extern void debugByteAsNumber( u8 number );

#endif /* DEBUG_H */

