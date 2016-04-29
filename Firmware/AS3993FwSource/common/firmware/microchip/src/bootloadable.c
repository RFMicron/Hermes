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
 *      PROJECT:   PIC bootloader v2.x.y
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author M. Arpa
 *
 *  \brief file to be compiled and linked by any application that shall be
 * loadable by the bootloader.
 *
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include <p24Fxxxx.h>
#include "bootloadable.h"

/* the following lines ensure that an application is loadable by
   the ams bootloader version 2.x.y */

extern void _reset();/* so that we can get the address of the reset - code */
const unsigned short userReset __attribute__ ((space(psv),address(USER_PROG_RESET_ADDR))) = (unsigned short)_reset;
const unsigned short appId __attribute__ ((space(psv),address(APP_ID_ADDR))) = APP_ID;

void __attribute__((space(prog),address(ENABLE_BOOTLOADER_ADDR))) enableBootloader()
{
    while ( 1 ) ; /* the bootloader brings this function with real functionality,
		here we just make sure if your application calls this function is never returns; */
}
