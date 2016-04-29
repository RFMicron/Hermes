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
 *  \author Josef Pertl
 *  \author R. Veigl
 *
 *  \brief Implementation of display.h for display access
 *  
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "main.h"
#include "system_clock.h"
#include <libpic30.h>
#include "lcd_common.h"
#include "lcd.h"


/*
******************************************************************************
* MACROS
******************************************************************************
*/


/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
u16 gLcdPixelsWidth;
u16 gLcdPixelsHeight;
u16 gLcdPages;

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/

static lcd_access_functions_t gLcdDriverAccess;


/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

void lcd_Init(lcd_access_functions_t lcdDriverAccess)
{
    gLcdDriverAccess = lcdDriverAccess;
    lcdDriverAccess.ptLcd_GetResolution(&gLcdPixelsWidth, &gLcdPixelsHeight);
    gLcdPages = gLcdPixelsHeight / 8;
}

void lcd_Clear()
{
    gLcdDriverAccess.ptLcd_Clear();
}

void lcd_WriteBitMap (u8 const *bitmap)
{
    u8 i, j;
    u8 value;

    for(i = 0; i < gLcdPages; i++)
    {
        gLcdDriverAccess.ptLcd_SetPageAddress(i);
        gLcdDriverAccess.ptLcd_SetColumnAddress(0);

        for(j = 0; j < gLcdPixelsWidth; j++)
        {
            //displaySetColumnAddress(j);                   // Column Address is autoincremented
            value = bitmap[j + ( i * gLcdPixelsWidth)];            
            gLcdDriverAccess.ptLcd_WriteData(value);
        }
    }
}



void lcd_WriteStr(u8 fontCol, u8 fontRow, char* string)
{
    char *str;

    gLcdDriverAccess.ptLcd_SetCursor(fontCol, fontRow);    
    str = string;

    while(*str)
    {
        gLcdDriverAccess.ptLcd_WriteChar(*str);        
        str++;
    }
}

void lcd_WriteStrInverted(u8 fontCol, u8 fontRow, char* string)
{
    char *str;

    gLcdDriverAccess.ptLcd_SetCursor(fontCol, fontRow);
    
    str = string;

    while(*str)
    {
        gLcdDriverAccess.ptLcd_WriteCharInverted(*str);
        str++;
    }
}

void lcd_WriteNumRightAligned(u8 fontCol, u8 fontRow, s32 value, u8 nbdigit, u8 precision)
{
    u8 digit;
    bool_t negative = FALSE;
    bool_t precdone = (precision == 0);
    u8 i = nbdigit;

    if (value < 0)
    {
        negative = TRUE;
        value *= -1;
    }

    do
    {
        digit = value % 10L;
        value /= 10L;
        {
            gLcdDriverAccess.ptLcd_SetCursor(fontCol--, fontRow);
            gLcdDriverAccess.ptLcd_WriteChar('0' + digit);
        }
        if (i)
        {
            i--;
        }
        if (precision && ((nbdigit - i) == precision))
        {
            gLcdDriverAccess.ptLcd_SetCursor(fontCol--, fontRow);
            gLcdDriverAccess.ptLcd_WriteChar('.');

            if (!value)
            {
                gLcdDriverAccess.ptLcd_SetCursor(fontCol--, fontRow);
                gLcdDriverAccess.ptLcd_WriteChar('0');

                if (i)
                {
                    i--;
                }
            }

            precdone = TRUE;
        }
    }
    while (i && (value || !precdone));

    if (negative)
    {
        gLcdDriverAccess.ptLcd_SetCursor(fontCol--, fontRow);
        gLcdDriverAccess.ptLcd_WriteChar('-');

        i--;
    }

    while (i)
    {
        i--;

        gLcdDriverAccess.ptLcd_SetCursor(fontCol--, fontRow);
        gLcdDriverAccess.ptLcd_WriteChar(' ');
    }
}


