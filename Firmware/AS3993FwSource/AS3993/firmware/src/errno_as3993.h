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
 *      PROJECT:   AS3993 firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/** @file
  * @brief
  * This file provides defines for error codes reported by the FW to the host.
  *
  * @author Ulrich Herrmann
  * @author Rene Eggerstorfer
  * This file contains error codes for Gen2 protocol handling, Gen2 tag
  * access functions and ISO6B tag access functions
  *
  */

#ifndef ERRNO_AS3993_H
#define ERRNO_AS3993_H
#include "errno.h"

#define ERR_FIRST_AS3993_ERROR (ERR_LAST_ERROR-1)

/* Errors primarily raised by AS3993 itself, however codes like ERR_CHIP_CRCERROR can be reused by ISO6B */
#define ERR_CHIP_NORESP               (ERR_FIRST_AS3993_ERROR - 0)
#define ERR_CHIP_HEADER               (ERR_FIRST_AS3993_ERROR - 1)
#define ERR_CHIP_PREAMBLE             (ERR_FIRST_AS3993_ERROR - 2)
#define ERR_CHIP_RXCOUNT              (ERR_FIRST_AS3993_ERROR - 3)
#define ERR_CHIP_CRCERROR             (ERR_FIRST_AS3993_ERROR - 4)
#define ERR_CHIP_FIFO                 (ERR_FIRST_AS3993_ERROR - 5)

#define ERR_REFLECTED_POWER           (ERR_FIRST_AS3993_ERROR - 16)

/* Upper level protocol errors for GEN2, e.g. a Write can fail when access command has failed... */
#define ERR_GEN2_SELECT               (ERR_FIRST_AS3993_ERROR - 32)
#define ERR_GEN2_ACCESS               (ERR_FIRST_AS3993_ERROR - 33)
#define ERR_GEN2_REQRN                (ERR_FIRST_AS3993_ERROR - 34)
#define ERR_GEN2_CHANNEL_TIMEOUT      (ERR_FIRST_AS3993_ERROR - 35)


/* Upper level protocol errors for ISO6B */
#define ERR_ISO6B_NOTAG	        (ERR_FIRST_AS3993_ERROR - 65)  /*!< No response from any tag */
#define ERR_ISO6B               (ERR_FIRST_AS3993_ERROR - 66)  /*!< IRQ error occured */
#define ERR_ISO6B_REG_FIFO      (ERR_FIRST_AS3993_ERROR - 67)  /*!< FIFO error occured */
#define ERR_ISO6B_OTHER	        (ERR_FIRST_AS3993_ERROR - 68)  /*!< Other error occurred */
#endif /* ERRNO_AS3993_H */

