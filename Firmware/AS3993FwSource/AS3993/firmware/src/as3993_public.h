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
/*! \file
 *
 *  \author U.Herrmann ( based on work by E.Grubmueller
 *  \author T. Luecker (Substitute)
 *  \author Bernhard Breinbauer
 *
 *   \brief Declaration of public functions provided by the AS3993 series chips
 *
 *   Declares functions provided by the AS3993 series chips. All higher level
 *   and protocol work is contained in gen2.h and iso6b.h. Register access is 
 *   performed using as3993.h.
 */

#ifndef _AS3993_PUBLIC_H_
#define _AS3993_PUBLIC_H_

#include "global.h"
#include "errno_as3993.h"

/** @struct TagInfo_
  * This struct stores the whole Tag information
  * @{
  */

#define AS3993_ERR_NORES      ERR_CHIP_NORESP
#define AS3993_ERR_PREAMBLE   ERR_CHIP_PREAMBLE
#define AS3993_ERR_HEADER     ERR_CHIP_HEADER
#define AS3993_ERR_FIFO_OVER  ERR_CHIP_FIFO
#define AS3993_ERR_RXCOUNT    ERR_CHIP_RXCOUNT
#define AS3993_ERR_CRC        ERR_CHIP_CRCERROR
#define AS3993_ERR_RXERR      ERR_CHIP_RXERR

/** @struct TagInfo_
  * This struct stores the whole information of one tag.
  *
  */
struct TagInfo_
{
    /** RN16 number */
    u8 rn16[2];

    /** PC value */
    u8 pc[2];
    /** EPC array */
    u8 epc[EPCLENGTH]; /* epc must immediately follow pc */
    /** EPC length */
    u8 epclen;  /*length in bytes */
    /** Handle for write and read communication with the Tag */
    u8 handle[2];
    /** rssi which has been measured when reading this Tag. */
    u8 rssi;
    /** content of AGC and Internal Status Display Register 0x2A after reading a tag. */
    u8 agc;
    /** TID length */
    u8 tidlength;  /*length in bytes */
    /** TID array */
    u8 tid[TIDLENGTH];
    /** MMS value */
    u8 mms[2];
    /** VFC value */
    u8 vfc[2];
    /** CAL values */
    u8 cal[8];
    /** TEMP value */
    u8 temp[2];
};

/** Type definition struct: TagInfo_ is named Tag */
typedef struct TagInfo_ Tag;

/** @struct Frequencies_
  * This struct stores the list of frequencies which are used for hopping.
  * For tuning enabled boards the struct also stores the tuning settings for
  * each frequency.
  *
  */
typedef struct
{
    /** Number of frequencies in freq. */
    u8 numFreqs;

    /** List of frequencies which are used for hopping. */
    u32 freq[MAXFREQ];
#ifdef TUNER
    /** Counts how often this freq has been used in hopping. Only available on tuner enabled boards. */
    u16 countFreqHop[MAXFREQ];
#endif
} Freq;

#ifdef TUNER
/** @struct TuningTable
  * This struct stores the whole information of the tuning parameters.
  *
  */
typedef struct
{
    /** number of entries in the table */
    u8 tableSize;
    /** currently active entry in the table. */
    u8 currentEntry;
    /** frequency which is assigned to this tune parameters. */
    unsigned long freq[MAXTUNE];
    /** tune enable for antenna. Valid values: 1 = antenna 1, 2 = antenna 2, 3 = antenna 1+2 */
    u8      tuneEnable[MAXTUNE];
    /** Cin tuning parameter for antenna 1 + 2. */
    u8           cin[2][MAXTUNE];
    /** Clen tuning parameter for antenna 1 + 2. */
    u8          clen[2][MAXTUNE];
    /** Cout tuning parameter for antenna 1 + 2. */
    u8          cout[2][MAXTUNE];
    /** Reflected power which was measured after last tuning. */
    u16      tunedIQ[2][MAXTUNE];
} TuningTable;
#endif

/** This function initialises the AS3993. A return value greater 0 indicates an error.\n
 *
 * @param baseFreq the base frequency to set at initialization time, may be 
 *        changed later on. 
 * @return       error code
 * @return 0     : if everything went o.k.
 * @return 1     : writing + reading SPI failed.
 * @return 2     : Reset via EN low + high failed.
 * @return 3     : IRQ line failed.
 * @return 4     : Crystal not stable (Bit0 in AGC and Internal Status register 0x2A)
 * @return 5     : PLL not locked (Bits1 in AGC and Internal Status register 0x2A)
 */
u16 as3993Initialize(u32 baseFreq);

/** This function reads the version register of the AS3993 and
  * returns the value. \n
  * @return Returns the value of the AS3993 version register.
  */
u8 as3993ReadChipVersion(void);

/*------------------------------------------------------------------------- */
/** This function sets the frequency in the approbiate register
  * @param regs Which register is being used either AS3993_REG_PLLMAIN or AS3993_REG_PLLAUX
  * @param frequency frequency in kHz
  */
void as3993SetBaseFrequency(u8 regs, u32 frequency);

/*------------------------------------------------------------------------- */
/** This function turns the antenna power on or off. 
  * @param on boolean to value to turn it on (1) or off (0).
  */
void as3993AntennaPower( u8 on);

/*------------------------------------------------------------------------- */
/**  This function stores the current sensitivity registers. After that 
  *  as3993SetSensitivity can be called to change sensitivty for subsequent 
  *  as3993GetRSSI() calls. as3993RestoreSensitivity() restores it again.
  */
void as3993SaveSensitivity( );

/*------------------------------------------------------------------------- */
/**  This function restores the sensitivity previousley saved using
  *  as3993SaveSensitivity().
  */
void as3993RestoreSensitivity( );

/*------------------------------------------------------------------------- */
/**  This function tries to set the sensitivity to a level to allow detection 
  *  of signals using as3993GetRSSI() with a level higher than miniumSignal(dBm).
  *  \return the level offset. Sensitivity was set to minimumSignal - (returned_val).
  *  negative values mean the sensitivity could not be reached.
  */
s8 as3993SetSensitivity( s8 minimumSignal );

/*------------------------------------------------------------------------- */
/**  This function gets the current rx sensitivity level.
  */
s8 as3993GetSensitivity( void );

/*------------------------------------------------------------------------- */
/** This function gets the values for the reflected power. For measuring the 
  * antenna will be switched on, a measurement performed and the antenna
  * if necessary turned back off. The two measured 8-bit values are returned as
  * one 16-bit value, the lower byte containing receiver-A (I) value, the 
  * higher byte containing receiver-B (Q) value. The two values are signed 
  * values and will be converted from sign magnitude representation to natural.
  * Note: To get a more accurate result the I and Q values of
  * as3993GetReflectedPowerNoiseLevel() should be subtracted from the result of
  * as3993GetReflectedPower()
  */
u16 as3993GetReflectedPower( void );

/*------------------------------------------------------------------------- */
/**
  * This function measures and returns the noise level of the reflected power
  * measurement of the active antenna. The returned value is of type u16 and
  * contains the 8 bit I and Q values of the reflected power. The I value is
  * located in the lower 8 bits, the Q value in the upper 8 bits. \n
  * See as3993GetReflectedPower() fore more info.
  */
u16 as3993GetReflectedPowerNoiseLevel( void );

/*------------------------------------------------------------------------- */
/** This function reads in the current register settings (configuration only), 
  * performs a reset by driving the enable line first low, then high and then
  * writes back the register values.
  */
void as3993Reset(void);

/*------------------------------------------------------------------------- */
/** This function performs a reset by driving the enable line first low,
  * then high.
  */
void as3993ResetDoNotPreserveRegisters(void);

/*------------------------------------------------------------------------- */
/** This function is for generic sending and receiving gen2 commands through 
    the FIFO.
    The parameters are:
    \param cmd: One of the AS3993_CMD_* commands wich transmits something.
    \param txbuf: buffer to be tranmitted.
    \param txbits: the size of txbuf in bits
    \param rxbuf: a buffer getting the response of the tag
    \param rxbits: the maximum number of expected rx bits (size of rxbuf in bits)
                      The value will be written to rxlength register. If 0 rxlength is
                      automatically calculated by the reader and no boundary check for rxbuf is done.
    \param norestime: value for register AS3993_REG_RXNORESPONSEWAITTIME
                      0xff means 26 ms. Other values are much shorter.
    \param followCmd: Command to be sent after rxdone interrupt but before 
           emptying FIFO. This is needed for not violating T2 timing requirement.
    \param waitTxIrq: If 0 tx irq will not be handled after transmitting command,
           otherwise FW will wait for Tx irq after transmitting command.

    Certain values may be set to 0/NULL thus changing the behavior. Examples:

    - as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRCEHEAD, buf_, 50, buf_, &rxbits, 0xff, 0) with rxbits=33
      can be used to send WRITE command and retrieve after max 20ms a reply expecting a header.
    - as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRC, buf_, len, NULL, &rxbits, 0, 0) with rxbits=0
      is a transmit-only used for sending SELECT. With *rxbits==0 and rxbuf==NULL RX will be turned off immediately
      after TX-done interrupt
    - as3993TxRxGen2Bytes(AS3993_CMD_QUERY,buf_,16,tag->rn16,&rxlen,gen2IntConfig.no_resp_time,AS3993_CMD_ACK) with rxlen==16
      sends QUERY command, waits for tx-done, then waits for rx-done, then 
      sends ACK command and only as the last step reads RN16 from FIFO
    - as3993TxRxGen2Bytes(0,buf_,0,buf_,&rxlen,gen2IntConfig.no_resp_time,fast?0:AS3993_CMD_REQRN) with rxlen==sizeof(buf_)*8
      does not send anything (ACK was executed via followCmd as previous example),
      waits for tx-done, waits for rx-done, sends REQRN, reads EPC from FIFO

  */
s8 as3993TxRxGen2Bytes(u8 cmd, u8 *txbuf, u16 txbits, 
                               u8 *rxbuf, u16 *rxbits,
                               u8 norestime, u8 followCmd,
                               u8 waitTxIrq);

/**
 * This function handles the rx-ed data for an command expecting a header bit.
 * On AS3980 we cannnot use the built in TRANSMCRCEHEADER command because
 * the header bit interrupt is hidden by the 2nd byte interrupt, which cannot be
 * disabled. Instead we use the the command TRANSMCRC and handle the header bit
 * in this function.\n
 * If a header bit is set the function will return ERR_CHIP_HEADER. \n
 * The content of the buffer will be shifted by 1 bit to remove the header bit.\n
 * As this function might ignore potential CRC errors, it checks if the received
 * data contains a the same handle as the handle parameter.
 * @param ret The return value of as399xTxRxGen2Bytes after sending a access command with TRANSMCRC
 * @param destbuf The received data. Will be shifted by 1 bit.
 * @param rxbits Number of received data bits.
 * @param handle Handle which should be inside the received data.
 * @return Returns the corrected error code.
 */
s8 as3980HandleResponseWithHeaderBit(s8 ret, u8 *destbuf, u16 rxbits, u8 const * handle);

/**
  * This functions performs an ADC conversion and returns the signed result in machine coding */
s8 as3993GetADC( void );
#endif /* _AS3993_PUBLIC_H_ */
