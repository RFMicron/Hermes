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
 *  \brief Implementation of lcd_NHD_A1Z.h for NHD C12832A1Z display
 *  
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/
#include "main.h"
#include "lcd_common.h"
#include "lcd_NHD_A1Z.h"
#include "spi_driver.h"
#include "system_clock.h"
#include "lcd_font_5x7_ASCII.h"
#include <libpic30.h>


/*
******************************************************************************
* MACROS
******************************************************************************
*/
#define  C12832A1Z_CMD_DISPLAY_ON                     0xAF
#define  C12832A1Z_CMD_DISPLAY_OFF                    0xAE
#define  C12832A1Z_CMD_START_LINE_SET(line)           (0x40 | (line))
#define  C12832A1Z_CMD_PAGE_ADDRESS_SET(page)         (0xB0 | (page))
#define  C12832A1Z_CMD_COLUMN_ADDRESS_SET_MSB(column) (0x10 | (column))
#define  C12832A1Z_CMD_COLUMN_ADDRESS_SET_LSB(column) (0x00 | (column))
#define  C12832A1Z_CMD_ADC_NORMAL                     0xA0
#define  C12832A1Z_CMD_ADC_REVERSE                    0xA1
#define  C12832A1Z_CMD_DISPLAY_NORMAL                 0xA6
#define  C12832A1Z_CMD_DISPLAY_REVERSE                0xA7
#define  C12832A1Z_CMD_DISPLAY_ALL_POINTS_OFF         0xA4
#define  C12832A1Z_CMD_DISPLAY_ALL_POINTS_ON          0xA5
#define  C12832A1Z_CMD_LCD_BIAS_1_DIV_5_DUTY33        0xA1
#define  C12832A1Z_CMD_LCD_BIAS_1_DIV_6_DUTY33        0xA2
#define  C12832A1Z_CMD_NORMAL_SCAN_DIRECTION          0xC0
#define  C12832A1Z_CMD_REVERSE_SCAN_DIRECTION         0xC8
#define  C12832A1Z_CMD_VOLTAGE_RESISTOR_RATIO_0       0x20
#define  C12832A1Z_CMD_VOLTAGE_RESISTOR_RATIO_1       0x21
#define  C12832A1Z_CMD_VOLTAGE_RESISTOR_RATIO_2       0x22
#define  C12832A1Z_CMD_VOLTAGE_RESISTOR_RATIO_3       0x23
#define  C12832A1Z_CMD_VOLTAGE_RESISTOR_RATIO_4       0x24
#define  C12832A1Z_CMD_VOLTAGE_RESISTOR_RATIO_5       0x25
#define  C12832A1Z_CMD_VOLTAGE_RESISTOR_RATIO_6       0x26
#define  C12832A1Z_CMD_VOLTAGE_RESISTOR_RATIO_7       0x27
#define  C12832A1Z_CMD_POWER_CTRL_ALL_ON              0x2F
#define  C12832A1Z_CMD_SLEEP_MODE                     0xAC
#define  C12832A1Z_CMD_NORMAL_MODE                    0xAD
#define  C12832A1Z_CMD_RESET                          0xE2
#define  C12832A1Z_CMD_NOP                            0xE3
#define  C12832A1Z_CMD_ELECTRONIC_VOLUME_MODE_SET     0x81
#define  C12832A1Z_CMD_ELECTRONIC_VOLUME(volume)      (0x3F & (~volume))
#define  C12832A1Z_CMD_BOOSTER_RATIO_SET              0xF8
#define  C12832A1Z_CMD_BOOSTER_RATIO_2X_3X_4X         0x00
#define  C12832A1Z_CMD_BOOSTER_RATIO_5X               0x01
#define  C12832A1Z_CMD_BOOSTER_RATIO_6X               0x03
#define  C12832A1Z_CMD_STATUS_READ                    0x00
#define  C12832A1Z_CMD_END                            0xEE
#define  C12832A1Z_CMD_READ_MODIFY_WRITE              0xE0
#define  C12832A1Z_DISPLAY_CONTRAST_MAX               45  //40
#define  C12832A1Z_DISPLAY_CONTRAST_MIN               30

/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
static u8 gSpiInstance;

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/

void lcd_NHD_A1Z_GetResolution(u16 *pixelsWidth, u16 *pixelsHeight)
{
    *pixelsWidth = LCD_NHD_A1Z_PIXELS_WIDTH;
    *pixelsHeight = LCD_NHD_A1Z_PIXELS_PER_LINE * LCD_NHD_A1Z_NUMBER_MAX_PAGES;
}

lcd_access_functions_t lcd_NHD_A1Z_GetAccessFunctions(void)
{
    lcd_access_functions_t result;

    result.ptLcd_GetResolution = &lcd_NHD_A1Z_GetResolution;
    result.ptLcd_SetPageAddress = &lcd_NHD_A1Z_SetPageAddress;
    result.ptLcd_SetColumnAddress = &lcd_NHD_A1Z_SetColumnAddress;
    result.ptLcd_SetCursor = &lcd_NHD_A1Z_CharPos;
    result.ptLcd_WriteChar = &lcd_NHD_A1Z_WriteChar;
    result.ptLcd_WriteCharInverted = &lcd_NHD_A1Z_WriteCharInverted;
    result.ptLcd_WriteData = &lcd_NHD_A1Z_WriteData;
    result.ptLcd_Clear = &lcd_NHD_A1Z_Clear;
    
    return result;
}

s8 lcd_NHD_A1Z_InitializeSPI()
{
    spiConfig_t lcdSpiCfg;
    
    LCD_NHD_A1Z_DISABLE_CS();       // Set CS line to HIGH
    LCD_NHD_A1Z_SET_RS();           // Set RS line to HIGH
    LCD_NHD_A1Z_COMMAND_A0();       // Set A0 line to LOW

    lcdSpiCfg.clockPhase = 0;
    lcdSpiCfg.clockPolarity = 1;
    lcdSpiCfg.deviceId = SPI_DEV_LCD;
    lcdSpiCfg.frequency = 1000000UL;
    lcdSpiCfg.instance = gSpiInstance; //SPI2;
    spiInitialize(systemClockGetFcy(), &lcdSpiCfg, NULL);

    return ERR_NONE;
}

void lcd_NHD_A1Z_WriteCommand(u8 command)
{
    lcd_NHD_A1Z_InitializeSPI();
    IRQ_INC_DISABLE();
    LCD_NHD_A1Z_ENABLE_CS();
    LCD_NHD_A1Z_COMMAND_A0();
    spiTxRx(&command, NULL, 1);    // write command via SPI
    LCD_NHD_A1Z_DISABLE_CS();
    IRQ_DEC_ENABLE();
}

void lcd_NHD_A1Z_WriteData(u8 value)
{
    lcd_NHD_A1Z_InitializeSPI();
    IRQ_INC_DISABLE();
    LCD_NHD_A1Z_ENABLE_CS();
    LCD_NHD_A1Z_DATA_A0();
    spiTxRx(&value, NULL, 1);    // write value via SPI
    LCD_NHD_A1Z_DISABLE_CS();
    IRQ_DEC_ENABLE();
}

void lcd_NHD_A1Z_HardReset()
{
    LCD_NHD_A1Z_RESET_RS();
    __delay_us(1);              // wait min 1us
    LCD_NHD_A1Z_SET_RS();
    __delay_us(2);              // wait min 2us
}

void lcd_NHD_A1Z_SoftReset()
{
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_RESET);
}

void lcd_NHD_A1Z_SleepEnable()
{
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_SLEEP_MODE);
}

void lcd_NHD_A1Z_SleepDisable()
{
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_NORMAL_MODE);
}

void lcd_NHD_A1Z_Init(u8 spiInstance)
{
    LCD_NHD_A1Z_DISABLE_CS();       // CS to default = HIGH
    LCD_NHD_A1Z_COMMAND_A0();       // A0 to default = LOW
    LCD_NHD_A1Z_SET_RS();           // RS to default = HIGH

    if(spiInstance >= SPI_MAX_INSTANCES)
        spiInstance = SPI1;

    gSpiInstance = spiInstance;

    lcd_NHD_A1Z_InitializeSPI(); // Initialize SPI1 or SPI2
    lcd_NHD_A1Z_HardReset();
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_ADC_NORMAL);
    lcd_NHD_A1Z_InvertDisable();
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_REVERSE_SCAN_DIRECTION);
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_LCD_BIAS_1_DIV_6_DUTY33);
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_POWER_CTRL_ALL_ON);
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_BOOSTER_RATIO_SET);
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_BOOSTER_RATIO_2X_3X_4X);
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_VOLTAGE_RESISTOR_RATIO_2);      // 0x21
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_ELECTRONIC_VOLUME_MODE_SET);    // 0x81
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_ELECTRONIC_VOLUME(C12832A1Z_DISPLAY_CONTRAST_MAX));
    lcd_NHD_A1Z_On();
}

void lcd_NHD_A1Z_SetPageAddress(u8 page)
{
    page &= 0x0F;       // Make sure that page is only 4 bits (only 8 pages)
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_PAGE_ADDRESS_SET(page));
}

void lcd_NHD_A1Z_SetColumnAddress(u8 column)
{
    column &= 0x7F;    // Make sure the address is 7 bits -> 128 is the maximum
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_COLUMN_ADDRESS_SET_MSB(column >> 4));
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_COLUMN_ADDRESS_SET_LSB(column & 0x0F));
}

void lcd_NHD_A1Z_SetStartLineAddress(u8 line)    // Starts the already written RAM from a specific line
{
    line &= 0x3F;    // Make sure address is 6 bits
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_START_LINE_SET(line));
}

void lcd_NHD_A1Z_On(void)
{
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_DISPLAY_ON);
}

void lcd_NHD_A1Z_Off(void)
{
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_DISPLAY_OFF);
}

void lcd_NHD_A1Z_SetAllPixels(bool_t pixels_on)
{
    if (pixels_on)
    {
        lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_DISPLAY_ALL_POINTS_ON);
    }
    else
    {
        lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_DISPLAY_ALL_POINTS_OFF);
    }
}

void lcd_NHD_A1Z_InvertEnable()
{
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_DISPLAY_REVERSE);
}

void lcd_NHD_A1Z_InvertDisable()
{
    lcd_NHD_A1Z_WriteCommand(C12832A1Z_CMD_DISPLAY_NORMAL);
}

void lcd_NHD_A1Z_CharPos(u8 x, u8 y)
{
    u16 column  = ((u16)x * LCD_NHD_A1Z_PIXELS_PER_COLUMN) % (u16)LCD_NHD_A1Z_PIXELS_WIDTH;

    lcd_NHD_A1Z_SetPageAddress(y % LCD_NHD_A1Z_NUMBER_MAX_PAGES);
    lcd_NHD_A1Z_SetColumnAddress(column);
}

void lcd_NHD_A1Z_Clear(void)
{
    u8 pageIndex, columnIndex;

    lcd_NHD_A1Z_SetColumnAddress(0);
    lcd_NHD_A1Z_SetPageAddress(0);

    for(pageIndex = 0; pageIndex <= LCD_NHD_A1Z_NUMBER_MAX_PAGES; pageIndex++)
    {
        lcd_NHD_A1Z_SetPageAddress(pageIndex);
        lcd_NHD_A1Z_SetColumnAddress(0);
        lcd_NHD_A1Z_WriteData(0x00);
        for(columnIndex = 0; columnIndex < LCD_NHD_A1Z_PIXELS_WIDTH; columnIndex++)
        {
            //displaySetColumnAddress(j);               // Column Address is autoincremented
            lcd_NHD_A1Z_WriteData(0x00);
        }
    }

    lcd_NHD_A1Z_SetColumnAddress(0);
    lcd_NHD_A1Z_SetPageAddress(0);
}

void lcd_NHD_A1Z_WriteChar(u8 value)
{
    u8 i;
    short addr;

    if(value <= 0x80)
    {
	//use standard ascii chars
        addr = (value - 0x20) * LCD_NHD_A1Z_PIXELS_PER_COLUMN;

	for (i = 0; i < LCD_NHD_A1Z_PIXELS_PER_COLUMN; i++)
	{
            lcd_NHD_A1Z_WriteData(lcd_font_5x7_ASCII_Std_Chars[addr + i]);
	}
    }
    else
    {
        //use extended ascii chars
        addr = (value - 0x80) * LCD_NHD_A1Z_PIXELS_PER_COLUMN;

        for (i = 0; i < LCD_NHD_A1Z_PIXELS_PER_COLUMN; i++)
        {
            lcd_NHD_A1Z_WriteData(lcd_font_5x7_ASCII_Ext_Chars[addr + i]);
        }
    }
}

void lcd_NHD_A1Z_WriteCharInverted(u8 value)
{
    u8 i;
    short addr;

    if(value <= 0x80)
    {
	//use standard ascii chars
        addr = (value - 0x20) * LCD_NHD_A1Z_PIXELS_PER_COLUMN;

	for (i = 0; i < LCD_NHD_A1Z_PIXELS_PER_COLUMN; i++)
	{
            lcd_NHD_A1Z_WriteData(~lcd_font_5x7_ASCII_Std_Chars[addr + i]);
	}
    }
    else
    {
        //use extended ascii chars
        addr = (value - 0x80) * LCD_NHD_A1Z_PIXELS_PER_COLUMN;

        for (i = 0; i < LCD_NHD_A1Z_PIXELS_PER_COLUMN; i++)
        {
            lcd_NHD_A1Z_WriteData(~lcd_font_5x7_ASCII_Ext_Chars[addr + i]);            
        }
    }
}


void lcd_NHD_A1Z_TestCharTable (void)
{
    u8 i, j;
    u8 pageAddr = 0;
    u8 fontCount = 0;
    u8 columnAddress = 0;

    lcd_NHD_A1Z_SetPageAddress(pageAddr);
    for (i = 0; i < sizeof(lcd_font_5x7_ASCII_Std_Chars) / LCD_NHD_A1Z_PIXELS_PER_COLUMN; i++)
    {
        for (j = 0; j < LCD_NHD_A1Z_PIXELS_PER_COLUMN; j++)
        {
            lcd_NHD_A1Z_SetColumnAddress(columnAddress++);
            lcd_NHD_A1Z_WriteData(lcd_font_5x7_ASCII_Std_Chars[i * LCD_NHD_A1Z_PIXELS_PER_COLUMN + j]);
        }

        fontCount++;

        if (fontCount >= LCD_NHD_A1Z_PIXELS_WIDTH / LCD_NHD_A1Z_PIXELS_PER_COLUMN)
        {
            lcd_NHD_A1Z_SetPageAddress(++pageAddr);
            columnAddress = 0;
            fontCount = 0;
        }
    }
}

