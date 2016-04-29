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
 *      PROJECT:   PIC24FJxxGB flash access
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author M. Arpa
 *          
 *  \brief routines to erase, program and read flash on pic.
 *
 *  
 *
 */


/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "flash_access.h"
#include <p24Fxxxx.h>


/*
******************************************************************************
* DEFINES
******************************************************************************
*/

#define FLASH_ROW_SIZE_IN_WORDS  ((FLASH_ROW_SIZE_IN_BYTES)/2)  /*!< flash row size in words */

#define FLASH_PAGE_OFFSET( addr ) \
  ( (DWORD)(addr) & ((DWORD)(FLASH_PAGE_SIZE_IN_WORDS)-1) )  /*!< only get the offset into  a page */

#define FLASH_ROW( addr ) \
  ( (DWORD)(addr) & ~((DWORD)(FLASH_ROW_SIZE_IN_WORDS)-1) )  /*!< normalise address to the beginning of a row */



/*! Constants for flash access */
#define FLASH_ACCESS_PAGE_ERASE    0x4042
#define FLASH_ACCESS_ROW_PROGRAM   0x4001
#define FLASH_ACCESS_WORD_PROGRAM  0x4003


/* Note: A table page is not a flash page. Table pages have the size of 0x10000,
   while a flash page typically has the size of 0x800. */
#define FLASH_TABLE_PAGE_SIZE_MASK   0xFFFF
#define FLASH_TABLE_PAGE_SIZE_SHIFT  16


/*
******************************************************************************
* FUNCTIONS
******************************************************************************
*/

void flashErasePage ( u32 wordAddress )
{
    u32 offset = wordAddress & FLASH_TABLE_PAGE_SIZE_MASK; /* offset into table page */
    TBLPAG = wordAddress >> FLASH_TABLE_PAGE_SIZE_SHIFT; /* select table page */

    __builtin_tblwtl( offset, 0x0000 /* value is don't care */ );

    NVMCON = FLASH_ACCESS_PAGE_ERASE;
    asm volatile ( "disi #5" );
    __builtin_write_NVM(); /* unlock sequence + start erase cycle */
    while(NVMCONbits.WR == 1);
}

void flashProgramInstructionRow ( u32 * wordAddress, const u32 * instructions )
{
    s16 i;
    u32 temp = *wordAddress;
    u32 offset;
    DWORD_VAL value;
    temp = FLASH_ROW( temp ); /* normalize to beginning of row */
    offset = temp & FLASH_TABLE_PAGE_SIZE_MASK; /* offset into table page */
    TBLPAG = temp >> FLASH_TABLE_PAGE_SIZE_SHIFT; /* select table page */

    /* a complete row is written -> update wordAddress */
    *wordAddress = temp + FLASH_ROW_SIZE_IN_WORDS; 

    /* perform write of complete row */
    for ( i = 0; i < FLASH_ROW_SIZE_IN_BYTES; i += 4, instructions++, offset += 2 )
    {
	  value.Val = *instructions;

	__builtin_tblwtl( offset, value.word.LW );
	__builtin_tblwth( offset, value.word.HW );
    }

    NVMCON = FLASH_ACCESS_ROW_PROGRAM;
    asm("DISI #5");
    __builtin_write_NVM();
    while(NVMCONbits.WR == 1);
}


void flashProgram1Instruction ( u32 * wordAddress, u32 instruction )
{
    DWORD_VAL value;
    u32 temp = *wordAddress;
    u32 offset = temp & FLASH_TABLE_PAGE_SIZE_MASK; /* offset into table page */
    TBLPAG = temp >> FLASH_TABLE_PAGE_SIZE_SHIFT; /* select table page */

    /* a pic 24 word is 16-bits, we write 3-Bytes + phantom = 2 16-bit words */
    *wordAddress += 2;

    value.Val = instruction;

    __builtin_tblwtl( offset, value.word.LW );
    __builtin_tblwth( offset, value.word.HW );

    NVMCON = FLASH_ACCESS_WORD_PROGRAM;
    asm("DISI #5");
    __builtin_write_NVM();
    while(NVMCONbits.WR == 1);
}

u32 flashRead1Instruction ( u32 * wordAddress )
{
    DWORD_VAL value;
    u32 temp = *wordAddress;
    u32 offset = temp & FLASH_TABLE_PAGE_SIZE_MASK; /* offset into table page */
    TBLPAG = temp >> FLASH_TABLE_PAGE_SIZE_SHIFT; /* select table page */

    /* a pic 24 word is 16-bits, we write 3-Bytes + phantom = 2 16-bit words */
    *wordAddress += 2;

    value.word.LW = __builtin_tblrdl( offset );
    value.word.HW = __builtin_tblrdh( offset );

    return value.Val;
}


void flashProgram3Bytes ( u32 * wordAddress, const u8 * data )
{
    DWORD_VAL value;

    value.v[0] = data[ 0 ];
    value.v[1] = data[ 1 ];
    value.v[2] = data[ 2 ];
    value.v[3] = 0x00; /* phantom byte */

    flashProgram1Instruction( wordAddress, value.Val );
}


void flashRead3Bytes ( u32 * wordAddress, u8 * data )
{
    DWORD_VAL value;

    value.Val = flashRead1Instruction( wordAddress );

    data[ 0 ] = value.v[ 0 ];
    data[ 1 ] = value.v[ 1 ];
    data[ 2 ] = value.v[ 2 ];
}


