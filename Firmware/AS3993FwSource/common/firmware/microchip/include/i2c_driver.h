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
 *      PROJECT:   AS3668 firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author M. Arpa
 *  \author Wolfgang Reichart
 *  \author R. Veigl
 *
 *  \brief abstraction for the i2c driver
 *
 *  This is the definition file for the i2c driver. The i2c driver is an i2c master
 *  driver using the i2c 2 module of the pic.
 *
 */

/*!
 *
 *
 */

#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

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
#define I2C1_MODULE                 0x01    /** I2C Mocule 1 selection. */
#define I2C2_MODULE                 0x02    /** I2C Mocule 2 selection. */

#define I2C_ADDRESS_MODE_7_BIT      0x01    /** I2C 7-bit address mode. */
#define I2C_ADDRESS_MODE_10_BIT     0x02    /** I2C 10-bit address mode. */

#define I2C_SCK_100KHZ              0x01    /** SCK frequency set to 100kHz. */
#define I2C_SCK_400KHZ              0x02    /** SCK frequency set to 400kHz. */
#define I2C_SCK_1000KHZ             0x03    /** SCK frequency set to 1MHz. */
#define I2C_SCK_3400KHZ             0x04    /** SCK frequency set to 3.4MHz. */
#define I2C_SCK_CUSTOM              0xFF    /** SCK frequency set to value given in custom clock speed. */

/*
******************************************************************************
* STRUCTS, TYPEDEFS
******************************************************************************
*/
struct i2cConfigData
{
    u8  i2cModule;              /** Specifies the module ID. */
    u8  i2cAddressMode;         /** Specifies 7 or 10 bit mode. */
    u8  i2cClockMode;           /** Specifies the SCL frequency between 100kHz and 3.4MHz in 100kHz steps. */
    u32 i2cCustomClockSpeed;    /** Specifies a custom clock speed (in kHz). */
};

typedef struct i2cConfigData i2cConfig_t;

/*
******************************************************************************
* GLOBAL FUNCTION PROTOTYPES
******************************************************************************
*/

#ifdef __PIC24FJ128GB202__
// no plib support, needs some functions to work properly

/*!
 *********************************************************************************************
Function Prototype : void IdleI2C1(void)

Include            : i2c.h

Description        : This function generates Wait condition until I2C bus is Idle.

Arguments          : None

Return Value       : None

Remarks            : This function will be in a wait state until Start Condition Enable bit,
                     Stop Condition Enable bit, Receive Enable bit, Acknowledge Sequence
                     Enable bit of I2C Control register and Transmit Status bit I2C Status
                     register are clear. The IdleI2C function is required since the hardware
                     I2C peripheral does not allow for spooling of bus sequence. The I2C
                     peripheral must be in Idle state before an I2C operation can be initiated
                     or write collision will be generated.
***********************************************************************************************/
void IdleI2C1 (void);
void IdleI2C2 (void);

/*!
 ********************************************************************
Function Prototype : void StartI2C1(void)

Include            : i2c.h

Description        : Generates I2C Bus Start condition.

Arguments          : None

Return Value       : None

Remarks            : This function generates a I2C Bus Start condition.
*********************************************************************/
void StartI2C1 (void);
void StartI2C2 (void);

/*!
 ********************************************************************
Function Prototype : void StopI2C1(void)

Include            : i2c.h

Description        : Generates I2C Bus Stop condition.

Arguments          : None

Return Value       : None

Remarks            : This function generates a I2C Bus Stop condition.
*********************************************************************/
void StopI2C1(void);
void StopI2C2(void);

/*!
 ************************************************************************
Function Prototype : void RestartI2C1(void)

Include            : i2c.h

Description        : Generates I2C Bus Restart condition.

Arguments          : None

Return Value       : None

Remarks            : This function generates an I2C Bus Restart condition.
**************************************************************************/
void RestartI2C1(void);
void RestartI2C2(void);

/*!
 ***********************************************************************************************
Function Prototype : char MasterWriteI2C1(unsigned char data_out)

Include            : i2c.h

Description        : This function is used to write out a single data byte to the I2C device.

Arguments          : data_out - A single data byte to be written to the I2C bus device.

Return Value       : This function returns -1 if there was a write collision else it returns a 0.

Remarks            : This function writes  a single byte to the I2C bus.
                     This function performs the same function as MasterputcI2C.
**************************************************************************************************/
char MasterWriteI2C1(unsigned char data_out);
char MasterWriteI2C2(unsigned char data_out);

/*!
 ***********************************************************************
Function Prototype : char DataRdyI2C1(void)

Include            : i2c.h

Description        : This function provides status back to user if I2CRCV
                     register contain data.

Arguments          : None

Return Value       : This function returns ‘1’ if there is data in I2CRCV register;
                     else return ‘0’ which indicates no data in I2CRCV register.

Remarks            : This function determines if there is any byte to read from
                     I2CRCV register.
*************************************************************************/
char DataRdyI2C1(void);
char DataRdyI2C2(void);

/*!
 **************************************************************************************************
Function Prototype :  unsigned int MastergetsI2C1(unsigned int length,
                          unsigned char *rdptr, unsigned int i2c_data_wait)

Include            :  i2c.h

Description        :  This function reads predetermined data string length from the I2C bus.

Arguments          :  length - Number of bytes to read from I2C device.
                      rdptr - Character type pointer to RAM for storage of data read from I2C device
                      i2c_data_wait - This is the timeout count for which the module has
                      to wait before return. If the timeout count is ‘N’, the actual time out would
                      be about (20 * N – 1) instruction cycles.

Return Value       :  This function returns ‘0’ if all bytes have been sent or number of bytes
                      read from I2C bus if its not able to read the data with in the specified
                      i2c_data_wait time out value

Remarks            :  This routine reads a predefined data string from the I2C bus.
******************************************************************************************************/
unsigned int MastergetsI2C1(unsigned int length, unsigned char * rdptr, unsigned int i2c1_data_wait);
unsigned int MastergetsI2C2(unsigned int length, unsigned char * rdptr, unsigned int i2c2_data_wait);

/*!
 ********************************************************************
Function Prototype : void CloseI2C1(void)

Include            : i2c.h

Description        : This function turns off the I2C module

Arguments          : None

Return Value       : None

Remarks            : This function disables the I2C module and clears the
                     Master and Slave Interrupt Enable and Flag bits.
*********************************************************************/
void CloseI2C1(void);
void CloseI2C2(void);

/*!
 *****************************************************************************
Function Prototype : void OpenI2C1(unsigned int config1, unsigned int config2)

Include            : i2c.h

Description        : Configures the I2C module

Arguments          : config1 - This contains the parameter to configure the I2CCON register

					 I2C Enable bit
					   *	I2C_ON
					   *	I2C_OFF
					 I2C Stop in Idle Mode bit
					   *	I2C_IDLE_STOP
					   *	I2C_IDLE_CON
					 SCL Release Control bit
					   *	I2C_CLK_REL
					   *	I2C_CLK_HLD
					 Intelligent Peripheral Management Interface Enable bit
					   *	I2C_IPMI_EN
					   *	I2C_IPMI_DIS
					 10-bit Slave Address bit
					   *	I2C_10BIT_ADD
					   *	I2C_7BIT_ADD
					 Disable Slew Rate Control bit
					   *	I2C_SLW_DIS
					   *	I2C_SLW_EN
					 SMBus Input Level bits
					   *	I2C_SM_EN
					   *	I2C_SM_DIS
					 General Call Enable bit
					   *	I2C_GCALL_EN
					   *	I2C_GCALL_DIS
					 SCL Clock Stretch Enable bit
					   *	I2C_STR_EN
					   *	I2C_STR_DIS
					 Acknowledge Data bit
					   *	I2C_ACK
					   *	I2C_NACK
					 Acknowledge Sequence Enable bit
					   *	I2C_ACK_EN
					   *	I2C_ACK_DIS
					 Receive Enable bit
					   *	I2C_RCV_EN
					   *	I2C_RCV_DIS
					 Stop Condition Enable bit
					   *	I2C_STOP_EN
					   *	I2C_STOP_DIS
					 Repeated Start Condition Enable bit
					   *	I2C_RESTART_EN
					   *	I2C_RESTART_DIS
					 Start Condition Enable bit
					   *	I2C_START_EN
					   *	I2C_START_DIS

					 config2 - computed value for the baud rate generator

Return Value      :  None

Remarks           :  This function configures the I2C Control register and I2C
                     Baud Rate Generator register
*******************************************************************************/
void OpenI2C1(unsigned int config1,unsigned int config2);
void OpenI2C2(unsigned int config1,unsigned int config2);
#endif

/*!
 *****************************************************************************
 *  \brief Read configuration in from a byte-stream (the byte stream must
 * have been written with i2cSerialiseConfig.
 *
 *  \param[OUT] config : structure to be filled with the data read in
 *  \param[IN]  data : byte stream containing the data to be converted to the
 *  config struct
 *****************************************************************************
 */
extern void i2cDeserialiseConfig( i2cConfig_t * config, const u8 * data );

/*!
 *****************************************************************************
 *  \brief Write configuration to a byte-stream (the byte stream must
 * be read in with i2cDeserialiseConfig.
 *
 *  \param[IN] config : structure to filled with the data to be written
 *  \param[OUT] data : byte stream to be filled with the data from the
 *  config struct
 *****************************************************************************
 */
extern void i2cSerialiseConfig( const i2cConfig_t * config, u8 * data );

/*!
 *****************************************************************************
 *  \brief  Initializes the i2c driver module.
 *
 *  Initializes the i2c driver by using the microchip pheripheral library.
 *  The old configuration is returned in cfgDataOut structure.
 *
 *  \param *cfgDataIn : pointer to config parameters input structure
 *  \param *cfgDataOut : pointer to config parameters output structure
 *****************************************************************************
 */
extern s8 i2cInitialize ( u32 sysClk, const i2cConfig_t* cfgDataIn, i2cConfig_t *cfgDataOut );

#if(0) // deprecated
/*!
 *****************************************************************************
 *  \brief  Returns the number of transmitted bytes.
 *
 *  Transmits the given byte stream over the i2c interface as an i2c master.
 *
 *  \param slaveAddr : the i2c address of the slave (address is left-shifted
 *         before transmitting it)
 *  \param data : the byte stream to be transmitted
 *  \param numberOfBytesToTx : the number of bytes to be sent
 *
 *  \return xx : the number of bytes transmitted
 *****************************************************************************
 */
extern u16 i2cTx ( u8 slaveAddr, const u8 * data, u16 numberOfBytesToTx );


/*!
 *****************************************************************************
 *  \brief  Returns the number of received bytes.
 *
 *  Transmits the given byte stream over the i2c interface as an i2c master and
 *  receives up to numberOfBytesToRx bytes over i2c.
 *
 *  \param slaveAddr : the i2c address of the slave
 *  \param txData : the byte stream to be transmitted
 *  \param numberOfBytesToTx : the number of bytes to be sent
 *  \param rxData : destination of the received bytes
 *  \param numberOfBytesToRx : the number of bytes to be received
 *
 *  \return xx : the number of bytes received
 *****************************************************************************
 */
extern u16 i2cRx ( u8 slaveAddr, const u8 * txData, u16 numberOfBytesToTx, u8 * rxData, u16 numberOfBytesToRx );
#endif
/*!
 *****************************************************************************
 *  \brief  Performs a raw I2C transfer
 *
 *  Transmits the given byte stream over the i2c interface as an i2c master and
 *  receives numberOfBytesToRx bytes over i2c. Start- and stop-conditions are generated
 *  on demand.
 *
 *  \param numberOfBytesToTx : number of bytes to TX
 *  \param txData  : the byte stream to be transmitted
 *  \param numberOfBytesToRx  : the number of bytes to be received
 *  \param rxData   : the buffer to store the received data
 *  \param sendStartCond  : if true, a start condition is generated
 *  \param sendStopCond   : if true, a stop condition is generated
 *
 *  \return status : ERR_NONE on success, ERR_IO otherwise
 *****************************************************************************
 */
extern s8 i2cRxTx ( u16 numberOfBytesToTx, const u8 * txData, u16 numberOfBytesToRx, u8 * rxData, BOOL sendStartCond, BOOL sendStopCond );

#endif /* I2C_DRIVER_H */



