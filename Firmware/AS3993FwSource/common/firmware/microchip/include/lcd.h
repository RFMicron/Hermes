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
 *  \brief LCD common functions
 *
 *  This is a collection of functions to control an lcd display.
 *  Before functions can be executed, the module must be initialized.
 *  
 *  example:
 *      lcd_NHD_A1Z_Init(SPI2);                         // initialize the display driver
 *      lcd_Init(lcd_NHD_A1Z_GetAccessFunctions());     // initialize lcd by passing the drivers basic functions
 *
 */


#ifndef LCD_H
#define LCD_H

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "main.h"
#include "ams_types.h"
#include "lcd_common.h"


/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/


/*! \ingroup lcd
 *****************************************************************************
 *  \brief  Initialize the display
 *
 *  \param[in] accessFunctions : the struct containing function pointers to
 *                               the driver functions
 *
 ******************************************************************************
 */
void lcd_Init(lcd_access_functions_t lcdDriverAccess);


/*! \ingroup lcd
 *****************************************************************************
 *  \brief  Writes a signed integer right-aligned to the display
 *
 *  This function is used to write \a value right-aligned to the display.
 *  Generally, this function shall be used to display numbers; number to
 *  string conversion (using sprintf...) should be avoided since this costs a
 *  lot of code as the stdio lib is linked in.
 *  Since floats are not supported the \a precision last digits of value are
 *  used as decimal places. If, for instance 324.6 needs to be displayed,
 *  \a value needs to be set to 3246, \a precision is 1 and nbdigit is 4.
 * 
 *  \param[in] x : x position where the number will be written to. The number
 *                 will be written from right to left!
 *  \param[in] y : y position
 *  \param[in] value : numeric value to be displayed
 *  \param[in] nbdigit: number of digits to display.
 *  \param[in] precision : number of decimal places used out of \a value.
 *
 ******************************************************************************
 */
extern void lcd_WriteNumRightAligned(u8 x, u8 y, s32 value, u8 nbdigit, u8 precision);

/*! \ingroup lcd
 *****************************************************************************
 *  \brief  Writes a bitmap to the display
 *
 *  \param[in] bitmap : The bitmap as char[512].
 *
 ******************************************************************************
 */
extern void lcd_WriteBitMap (u8 const *bitmap);

/*! \ingroup lcd
 *****************************************************************************
 *  \brief  Writes a string to the display at the specified position
 *
 *  \param[in] fontCol : the column number.
 *  \param[in] fontRow : the row number.
 *  \param[in] string : the string which should be displayed.
 *
 ******************************************************************************
 */
extern void lcd_WriteStr(u8 fontCol, u8 fontRow, char* string);

/*! \ingroup lcd
 *****************************************************************************
 *  \brief  Writes an inverted string to the display at the specified position
 *
 *  \param[in] fontCol : the column number.
 *  \param[in] fontRow : the row number.
 *  \param[in] string : the string which should be displayed.
 *
 ******************************************************************************
 */
extern void lcd_WriteStrInverted(u8 fontCol, u8 fontRow, char* string);

/*! \ingroup lcd
 *****************************************************************************
 *  \brief  Clears the display.
 *
 ******************************************************************************
 */
extern void lcd_Clear (void);

#endif /* LCD_H */

