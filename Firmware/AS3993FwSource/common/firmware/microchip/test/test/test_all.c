#include "unity.h"

void setUp()
{
}

void tearDown()
{
}

//------------------------------------------------------------------------------------------
// General
#include <p24fxxxx.h>
#include "../../include/ams_types.h"
#include "../../include/system_clock.h"
#include "../../include/errno.h"

// CONFIG4
#pragma config DSWDTPS = DSWDTPS3       // DSWDT Postscale Select->1:1048576 (33.825 Secs)
#pragma config DSWDTOSC = LPRC          // Deep Sleep Watchdog Timer Oscillator Select->DSWDT uses Low Power RC Oscillator (LPRC)
#pragma config DSBOREN = OFF            // Deep Sleep BOR Enable bit->BOR enabled in Deep Sleep
#pragma config DSWDTEN = OFF            // Deep Sleep Watchdog Timer->DSWDT enabled
#pragma config DSSWEN = OFF             // DSEN Bit Enable->Deep Sleep is controlled by the register bit DSEN

// CONFIG3
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable->Segmented code protection disabled
#pragma config WPFP = WPFP0             // Write Protection Flash Page Segment Boundary->Page 0 (0x0)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select->Write Protect from WPFP to the last page of memory
#pragma config WPCFG = WPCFGDIS         // Write Protect Configuration Page Select->Last page and Flash Configuration words are unprotected
#pragma config SOSCSEL = ON             // SOSC Selection bits->SOSC circuit selected
#pragma config BOREN = ON               // Brown-out Reset Enable->Brown-out Reset Disabled
#pragma config WDTWIN = PS25_0          // Window Mode Watchdog Timer Window Width Select->Watch Dog Timer Window Width is 25 percent
#pragma config PLLSS = PLL_FRC          // PLL Secondary Selection Configuration bit->PLL is fed by the Primary oscillator

// CONFIG2
#pragma config IESO = ON                // Internal External Switchover->Enabled
#pragma config IOL1WAY = OFF            // IOLOCK One-Way Set Enable->Once set, the IOLOCK bit cannot be cleared
#pragma config FNOSC = FRCDIV           // Initial Oscillator Select->Fast RC Oscillator with Postscaler (FRCDIV)
#pragma config PLLDIV = NODIV           // USB 96 MHz PLL Prescaler Select->Oscillator input divided by 2 (8 MHz input)
#pragma config I2C1SEL = DISABLE        // Alternate I2C1 enable bit->I2C1 uses SCL1 and SDA1 pins
#pragma config POSCMD = NONE            // Primary Oscillator Select->HS Oscillator Enabled
#pragma config OSCIOFCN = OFF           // OSCO Pin Configuration->OSCO/CLKO/RA3 pin functions as port I/O (RA3)
#pragma config FCKSM = CSECMD           // Clock Switching and Fail-Safe Clock Monitor->Sw Enabled, Mon Disabled

// CONFIG1
#pragma config WDTPS = PS1              // Watchdog Timer Postscaler->1:1
#pragma config ICS = PGx1               // Emulator Pin Placement Select bits->Emulator functions are shared with PGEC1/PGED1
#pragma config FWPSA = PR32             // WDT Prescaler->Prescaler ratio of 1:32
#pragma config WINDIS = OFF             // Windowed WDT->Standard Watchdog Timer disabled
#pragma config GWRP = OFF               // General Segment Write Protect->Writes to program memory are allowed
#pragma config GCP = OFF                // General Segment Code Protect->Code protection is disabled
#pragma config FWDTEN = OFF             // Watchdog Timer->Watchdog Timer is disabled
#pragma config JTAGEN = OFF             // JTAG Port Enable->JTAG port is disabled
#pragma config WDTCLK = LPRC            // WDT Clock Source Select bits->WDT uses LPRC
#pragma config WDTCMX = WDTCLK          // WDT Clock Source Select bits->WDT clock source is determined by the WDTCLK Configuration bits
#pragma config ALTCMPI = CxINC_RX       // Alternate Comparator Input bit->C1INC is on RB13, C2INC is on RB9 and C3INC is on RA0
#pragma config ALTRB6 = APPEND          // Alternate RB6 pin function enable bit->Append the RP6/ASCL1/PMPD6 functions of RB6 to RA1 pin functions
#pragma config LPCFG = ON               // Low power regulator control->Low voltage regulator controlled by RETEN bit

void test_pic_init()
{
#define SYSCLK 4000000ULL
    
#if (SYSCLK == 250000ULL)
   CLKDIV = 0x0400;   // for SYSCLK(FCY) = 0.25MHz
#elif (SYSCLK == 500000ULL)
   CLKDIV = 0x0300;   // for SYSCLK(FCY) = 0.5MHz
#elif (SYSCLK == 1000000ULL)
   CLKDIV = 0x0200;   // for SYSCLK(FCY) = 1MHz
#elif (SYSCLK == 2000000ULL)
   CLKDIV = 0x0100;   // for SYSCLK(FCY) = 2MHz
#elif (SYSCLK == 4000000ULL)
   CLKDIV = 0x0000;   // for SYSCLK(FCY) = 4MHz     TODO: config is currently without PLL (with PLL also possible if in USB powered mode, but did not work somehow)
#elif (SYSCLK == 8000000ULL)
   CLKDIV = 0x0140;   // for SYSCLK(FCY) = 8MHz
#elif (SYSCLK == 16000000ULL)
   CLKDIV = 0x0100;   // for SYSCLK(FCY) = 16MHz
#endif

   TEST_ASSERT_TRUE(SYSCLK == systemClockGetFcy());
}

//------------------------------------------------------------------------------------------
// UART
#include "../../include/uart_driver.h"
void test_uart_init()
{
    u32 sysclk = 16000000ULL;
    u32 baudrate = 115200;
    u32 actbaudrate = 0;
    TEST_ASSERT_TRUE(ERR_NONE == uartInitialize(sysclk,baudrate,&actbaudrate));
}
void test_uart_baud_rate()
{
    u32 sysclk = 16000000ULL;
    u32 baudrate = 115200;
    u32 actbaudrate = 0;
    uartInitialize(sysclk,baudrate,&actbaudrate);

    if(actbaudrate != baudrate)
        printf("%ld baud instead of %ld baud\n",actbaudrate,baudrate);
    TEST_ASSERT_FLOAT_WITHIN(0.2,1.0,baudrate/actbaudrate);
}

void test_uart_read()
{
    TEST_ASSERT_TRUE(U1RXREG == ReadUART1());
}
//------------------------------------------------------------------------------------------
// SPI
#include "../../include/spi_driver.h"
void test_spi_init()
{
    spiConfig_t spiConfigIn;
    spiConfig_t spiConfigIn2;
    spiConfig_t spiConfigOut;

    spiConfigIn.frequency =  100000ULL;
    spiConfigIn.instance = SPI1;
    spiConfigIn.clockPhase = 1;
    spiConfigIn.clockPolarity = 0;
    spiInitialize(&spiConfigIn,NULL);
    spiConfigIn2.frequency =  500000ULL;
    spiConfigIn2.instance = SPI2;
    spiConfigIn2.clockPhase = 0;
    spiConfigIn2.clockPolarity = 1;
    // spiConfigOut should save last configuration (spiConfigIn)
    spiInitialize(&spiConfigIn2,&spiConfigOut);

    TEST_ASSERT_TRUE(spiConfigIn.frequency == spiConfigOut.frequency);
    TEST_ASSERT_TRUE(spiConfigIn.instance == spiConfigOut.instance);
    TEST_ASSERT_TRUE(spiConfigIn.clockPhase == spiConfigOut.clockPhase);
    TEST_ASSERT_TRUE(spiConfigIn.clockPolarity == spiConfigOut.clockPolarity);
}

void test_spi_speed()
{
    spiConfig_t spiConfigIn;

    spiConfigIn.frequency =  100000;
    spiConfigIn.instance = SPI1;
    spiConfigIn.clockPhase = 1;
    spiConfigIn.clockPolarity = 0;
    spiInitialize(&spiConfigIn,NULL);

    u32 actFrequency = 4000000ULL / (2*(SPI1BRGL+1));

    if(actFrequency != spiConfigIn.frequency)
        printf("%ld instead of %ld\n",actFrequency,spiConfigIn.frequency);
    TEST_ASSERT_FLOAT_WITHIN(0.2,1.0,spiConfigIn.frequency/actFrequency);
}

//------------------------------------------------------------------------------------------
// I2C
#include "../../include/i2c_driver.h"
i2cConfig_t i2cConfig,i2cConfigReal;
void test_i2c_init()
{
    i2cConfig.i2cAddressMode = I2C_ADDRESS_MODE_7_BIT;
    i2cConfig.i2cClockMode = I2C_SCK_100KHZ;
    i2cConfig.i2cModule = I2C1_MODULE;
    TEST_ASSERT_TRUE(ERR_NONE == i2cInitialize(4000000ULL, &i2cConfig, &i2cConfigReal));
}

void test_i2c_speed()
{
    i2cConfig.i2cAddressMode = I2C_ADDRESS_MODE_7_BIT;
    i2cConfig.i2cClockMode = I2C_SCK_100KHZ;
    i2cConfig.i2cModule = I2C1_MODULE;

    i2cInitialize(4000000ULL, &i2cConfig, &i2cConfigReal);
    u32 actualSpeed = 4000000ULL/(I2C1BRG + 1 + (4000000ULL/10000000));
    if(actualSpeed != 100000)
        printf("%ld instead of %ld\n",actualSpeed,100000);
    TEST_ASSERT_FLOAT_WITHIN(0.2,1.0,100000/actualSpeed);
}