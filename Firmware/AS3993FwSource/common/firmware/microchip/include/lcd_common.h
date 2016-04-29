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
 *      PROJECT:   ASxxxx
 *      LANGUAGE:  ANSI C
 */

/*!
 *
 *  \author R. Veigl
 *
 *  \brief common header for LCD functionality
 * 
 */


#ifndef LCD_COMMON_H
#define LCD_COMMON_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

/*
******************************************************************************
* MACROS
******************************************************************************
*/


/*
******************************************************************************
* STRUCTS
******************************************************************************
*/
typedef struct lcd_access_functions
{
    void (*ptLcd_GetResolution) ( u16 *width, u16 *height );
    void (*ptLcd_SetPageAddress)( u8 page );
    void (*ptLcd_SetColumnAddress)( u8 column );
    void (*ptLcd_SetCursor)( u8 column, u8 line );
    void (*ptLcd_WriteChar)( u8 value );
    void (*ptLcd_WriteCharInverted)( u8 value );
    void (*ptLcd_WriteData)( u8 value );
    void (*ptLcd_Clear)( void );
} lcd_access_functions_t;

/*
******************************************************************************
* GLOBAL VARIABLES
******************************************************************************
*/


/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/




#endif /* LCD_COMMON_H */

