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
 *  \brief routines to erase, program and read flash on pic 24
 *
 *  
 *
 */


#ifndef FLASH_ACCESS_H
#define FLASH_ACCESS_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "ams_types.h"

/*
******************************************************************************
* DEFINES
******************************************************************************
*/


/*! Device Flash Memory Configuration */
/*! Size of flash memory sections in bytes includes upper phantom byte
 * (32-bit wide memory) */
#ifndef FLASH_ROW_SIZE_IN_BYTES
#define FLASH_ROW_SIZE_IN_BYTES          256      /*!< user flash row size in bytes */
#endif

#ifndef FLASH_PAGE_SIZE_IN_BYTES
#define FLASH_PAGE_SIZE_IN_BYTES         2048     /*!< user flash page size in bytes */
#endif

/*! Flash pages are internally address through word addresses.
    If we use the compiler to reserve a flash page, the compiler will use only the 
    lower 16-bit word of the flash, the upper-8-bits are lost. So you can use the
    following to reserve a page:
          #define FLASH_STORE_START_LOCATION  FLASH_PAGE(0x8000)
	  u8 flashStore[ FLASH_PAGE_SIZE_IN_WORDS ] __attribute__((space(prog),address(FLASH_STORE_START_LOCATION)));
*/
#define FLASH_PAGE_SIZE_IN_WORDS ((FLASH_PAGE_SIZE_IN_BYTES)/2) /*!< flash page size in words */

/* Every page must start at a page boarder -> normalize your address with this
   macro to a page boarder:
   #define FLASH_STORE_START_ADDRESS ( FLASH_PAGE( 0x8000 ) ) 
*/
#define FLASH_PAGE( addr ) \
  ( (DWORD)(addr) & ~((DWORD)(FLASH_PAGE_SIZE_IN_WORDS)-1) )  /*!< normalise address to the beginning of a page */


/*
******************************************************************************
* FUNCTIONS
******************************************************************************
*/

/*!
 *****************************************************************************
 *  \brief erases a flash page 
 *
 *  Function to erase a complete flash page (a page is FLASH_PAGE_SIZE_IN_BYTES).
 *  Note that on the PIC24 the flash is only 3 bytes wide, while the address
 *  counts 4 bytes (1 byte is the phantom byte). Flash addresses are 16-bit
 *  word addresses. 
 *
 * \param wordAddress - is the 16-bit address of the page to be erased (can
 *  point somewhere into the page, it is normalized by the function to the
 *  beginning of the flash page)
 *****************************************************************************
 */
void flashErasePage( u32 wordAddress );

/*!
 *****************************************************************************
 *  \brief programs a complete row to flash
 *
 *  Function to program a complete flash row (a row is FLASH_ROW_SIZE_IN_BYTES).
 *  Note that on the PIC24 the flash is only 3 bytes wide, while the address
 *  counts 4 bytes (1 byte is the phantom byte). Flash addresses are 16-bit
 *  word addresses. 
 *  You must provide (FLASH_ROW_SIZE_IN_BYTES/4) instructions to this function.
 *
 * \param wordAddress - is the 16-bit address page+row to be programmed 
 *  (it can point somewhere into the row, it is normalized by the function to the
 *  beginning of the flash row). The function increments the *wordAddress by 
 *  (FLASH_ROW_SIZE_IN_BYTES/2)
 * \param instructions - data to be written to flash
 *****************************************************************************
 */
void flashProgramInstructionRow( u32 * wordAddress, const u32 * instructions );

/*!
 *****************************************************************************
 *  \brief programs 3 bytes to flash
 *
 *  Function to program 3 bytes to flash.
 *  Note that on the PIC24 the flash is only 3 bytes wide, while the address
 *  counts 4 bytes (1 byte is the phantom byte). Flash addresses are 16-bit
 *  word addresses. 
 *
 * \param wordAddress - is the 16-bit address page+row to be programmed 
 *  The function increments the *wordAddress by 2
 * \param data - data to be written to flash
 *****************************************************************************
 */
void flashProgram3Bytes( u32 * wordAddress, const u8 * data );

/*!
 *****************************************************************************
 *  \brief reads 3 bytes from flash
 *
 *  Function to read 3 bytes from flash.
 *  Note that on the PIC24 the flash is only 3 bytes wide, while the address
 *  counts 4 bytes (1 byte is the phantom byte). Flash addresses are 16-bit
 *  word addresses. 
 *
 * \param wordAddress - is the 16-bit address page+row to be programmed 
 *  The function increments the *wordAddress by 2
 * \param data - data read from flash
 *****************************************************************************
 */
void flashRead3Bytes( u32 * wordAddress, u8 * data );


/*!
 *****************************************************************************
 *  \brief programs 1 instruction to flash
 *
 *  Function to program a 32-bit value (i.e. 2 16-bit words ) to flash. Note
 *  that one of the 4 bytes will be the phantom byte and not be written to flash.
 *  Note that on the PIC24 the flash is only 3 bytes wide, while the address
 *  counts 4 bytes (1 byte is the phantom byte). Flash addresses are 16-bit
 *  word addresses. 
 *
 * \param wordAddress - is the 16-bit address to be programmed 
 *  The function increments the *wordAddress by 2
 * \param instruction - data to be written to flash
 *****************************************************************************
 */
void flashProgram1Instruction( u32 * wordAddress, u32 instruction );

/*!
 *****************************************************************************
 *  \brief reads 1 instruction from flash
 *
 *  Function to read a 32-bit value (i.e. 2 16-bit words) from flash. Note that
 *  one of the 4 bytes is the phantom byte and will be 0.
 *  Note that on the PIC24 the flash is only 3 bytes wide, while the address
 *  counts 4 bytes (1 byte is the phantom byte). Flash addresses are 16-bit
 *  word addresses. 
 *
 * \param wordAddress - is the 16-bit address page+row to be programmed 
 *  The function increments the *wordAddress by 2
 * \returns the instruction read from flash
 *****************************************************************************
 */
u32 flashRead1Instruction( u32 * wordAddress );

#endif /* FLASH_ACCESS_H */
