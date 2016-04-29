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
 *  \author F. Lobmaier
 *  \author R. Veigl
 *
 *  \brief display driver definition file
 *
 *  This is the definition file for the display driver.
 *  To get this driver working the following macros must be defined:
 *
 *  example:
 *      #define LCD_SPI_CS          _LATA3  // chip select
 *      #define LCD_SPI_A0          _LATA0  // control/data switch
 *      #define LCD_SPI_RS          _LATA2  // reset
 *
 *  Make also sure the lcd_NHD_A1Z_Init(u8 spiInstance) function is called before
 *  other commands are executed.
 */

/*!
 * \defgroup display display driver module
 */

#ifndef LCD_NHD_A1Z_H
#define LCD_NHD_A1Z_H

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
* MACROS
******************************************************************************
*/

/* Make sure that LCD_SPI_xx macros are defined in main.h */
#define LCD_NHD_A1Z_ENABLE_CS()             LCD_SPI_CS = 0 //_LATA3 = 0
#define LCD_NHD_A1Z_DISABLE_CS()            LCD_SPI_CS = 1 //_LATA3 = 1
#define LCD_NHD_A1Z_COMMAND_A0()            LCD_SPI_A0 = 0 //_LATA0 = 0
#define LCD_NHD_A1Z_DATA_A0()               LCD_SPI_A0 = 1 //_LATA0 = 1
#define LCD_NHD_A1Z_RESET_RS()              LCD_SPI_RS = 0 //_LATA2 = 0
#define LCD_NHD_A1Z_SET_RS()                LCD_SPI_RS = 1 //_LATA2 = 1

#define LCD_NHD_A1Z_NUMBER_MAX_PAGES        4
#define LCD_NHD_A1Z_PIXELS_WIDTH            128
#define LCD_NHD_A1Z_PIXELS_PER_COLUMN       6
#define LCD_NHD_A1Z_PIXELS_PER_LINE         8
#define LCD_NHD_A1Z_NUM_LINES               4
#define LCD_NHD_A1Z_NUM_COLUMNS             21


/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/


/*! \ingroup lcd_NHD_A1Z
 *****************************************************************************
 *  \brief  Returns the display's resolution in pixel
 *
 *  \param[out] pixelsWidth : The display's width in pixel.
 *  \param[out] pixelsHeight : The display's height in pixel.
 *
 *****************************************************************************
 */
extern void lcd_NHD_A1Z_GetResolution(u16 *pixelsWidth, u16 *pixelsHeight);


/*! \ingroup lcd_NHD_A1Z
 *****************************************************************************
 *  \brief  Returns a struct that contains function pointer to access
 *          the driver's basic functions
 *****************************************************************************
 */
extern lcd_access_functions_t lcd_NHD_A1Z_GetAccessFunctions(void);

/********************************************************************
 *  \brief  Initializes the SPI interface
 ********************************************************************/
extern s8 lcd_NHD_A1Z_InitializeSPI (void);

/********************************************************************
 *  \brief  Performs a hard reset
 ********************************************************************/
extern void lcd_NHD_A1Z_HardReset (void);

/********************************************************************
 *  \brief  Performs a soft reset
 ********************************************************************/
extern void lcd_NHD_A1Z_SoftReset (void);

/********************************************************************
 *  \brief  Writes data to the display
 *
 *  \param[in] value : the value written to the display
 ********************************************************************/
extern void lcd_NHD_A1Z_WriteData (u8 value);

/********************************************************************
 *  \brief  Writes a command to the display
 *
 *  \param[in] command : the command written to the display
 ********************************************************************/
extern void lcd_NHD_A1Z_WriteCommand (u8 command);

/********************************************************************
 *  \brief  Initializes the display
 ********************************************************************/
extern void lcd_NHD_A1Z_Init ( u8 spiInstance );

/********************************************************************
 *  \brief  Enable sleep
 ********************************************************************/
extern void lcd_NHD_A1Z_SleepEnable (void);

/********************************************************************
 *  \brief  Disable sleep
 ********************************************************************/
extern void lcd_NHD_A1Z_SleepDisable (void);

/********************************************************************
 *  \brief  Set the page address
 *
 *  \param[in] page : the page index
 ********************************************************************/
extern void lcd_NHD_A1Z_SetPageAddress (u8 page);

/********************************************************************
 *  \brief  Set the cursor to the specified column
 *
 *  \param[in] column : the column index
 ********************************************************************/
extern void lcd_NHD_A1Z_SetColumnAddress (u8 column);

/********************************************************************
 *  \brief  Set the display RAM display start line address
 *
 *  \param[in] line : the line address
 ********************************************************************/
extern void lcd_NHD_A1Z_SetStartLineAddress (u8 line);

/********************************************************************
 *  \brief  Enable the display
 ********************************************************************/
extern void lcd_NHD_A1Z_On (void);

/********************************************************************
 *  \brief  Disable the display
 ********************************************************************/
extern void lcd_NHD_A1Z_Off (void);

/********************************************************************
 *  \brief  Set all pixels
 *
 *  \param[in] pixels_on : if true, all pixels are set otherwise
 *                         all pixels are reset
 ********************************************************************/
extern void lcd_NHD_A1Z_SetAllPixels (bool_t pixels_on);

/********************************************************************
 *  \brief  Enable the display inversion
 ********************************************************************/
extern void lcd_NHD_A1Z_InvertEnable (void);

/********************************************************************
 *  \brief  Disable the display inversion
 ********************************************************************/
extern void lcd_NHD_A1Z_InvertDisable (void);

/********************************************************************
 *  \brief  Set the cursor position
 *
 *  \param[in] x : the x-position of the cursor
 *  \param[in] y : the y-position of the cursor
 ********************************************************************/
extern void lcd_NHD_A1Z_CharPos(u8 x, u8 y);

/********************************************************************
 *  \brief  Clear the display
 *
 ********************************************************************/
extern void lcd_NHD_A1Z_Clear( void );

/********************************************************************
 *  \brief  Write a character to the display's current position
 *
 *  \param[in] value : the ascii value of the character
 ********************************************************************/
extern void lcd_NHD_A1Z_WriteChar(u8 value);

/********************************************************************
 *  \brief  Write an inverted character to the display's current position
 *
 *  \param[in] value : the ascii value of the character
 ********************************************************************/
extern void lcd_NHD_A1Z_WriteCharInverted(u8 value);


/********************************************************************
 *  \brief  Display a character table for testing
 ********************************************************************/
extern void lcd_NHD_A1Z_TestCharTable (void);



#endif /* LCD_NHD_A1Z_H */

