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
 *  \author Rene Eggerstorfer
 *  (based on the work of C. Eisendle and T. Luecker (Substitute), project AS399x)
 *
 *   \brief Implementation of ISO18000-6b protocol
 *
 *  Implementation of ISO18000-6b protocol.
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "global.h"
#include "timer.h"
#include "bitbang.h"
#include "as3993.h"
#include "iso6b.h"
#include "crc16.h"
#include "logger.h"
#include "platform.h"

#if ISO6B
/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
static t_bbData cmdData[50]; /*!< buffer used for the bitbang module */

/*
******************************************************************************
* LOCAL DATATYPES
******************************************************************************
*/
/*!
  * function pointer indicating the function to be used
  * in the next collision arbitration iteration
  */
typedef void (*iso6bCmd)(void);

/*
******************************************************************************
* LOCAL FUNCTION PROTOTYPES
******************************************************************************
*/
static s8 iso6bPreparePreambleDelimiter(t_bbData *preData);
static s8 iso6bPrepareByte (t_bbData *manchdata, u8 byteData);
static s8 iso6bPrepareCRC(t_bbData *crcData, s16 crc);

static void iso6bResync(void);
static void iso6bSendCmd(u8 cmd);
static void iso6bSendResend(void);
static void iso6bSendInitialize(void);
static void iso6bSendSuccess(void);
static void iso6bSendFail(void);
static void iso6bSendSelect(u8 mask, u8* filter, u8 startaddress);

/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
s8 iso6bInitialize()
{
    bbSetup(13);
    return ISO6B_ERR_NONE;
}

s8 iso6bDeinitialize()
{
    return ISO6B_ERR_NONE;
}

void iso6bOpen()
{
    as3993SingleWrite(AS3993_REG_STATUSCTRL, 0x01);
    /* dir_mode Normal; Protocol selection: ISO 18000-6 */
    as3993SingleWrite(AS3993_REG_PROTOCOLCTRL, 0x01);
    /* Tari 25us */
    as3993SingleWrite(AS3993_REG_TXOPTIONS, 0x02);
    /*  Set to 40kHz Link frequency, FM0,Long Preamble */
    as3993SingleWrite(AS3993_REG_RXOPTIONS, 0x08);
    /*  No Response 690us*/
    as3993SingleWrite(AS3993_REG_RXNORESPONSEWAITTIME, 0x1B);
    /*  Set Rx Wait Time to 172.8us(Quiet Time: 40kHz -> 381.25us)*/
    as3993SingleWrite(AS3993_REG_RXWAITTIME, 0x1B);
    /*  RX Filter Setting proposed in Datasheet */
    as3993SingleWrite(AS3993_REG_RXFILTER, 0xFF);
    /* ASK */
    as3993SingleWrite(AS3993_REG_MODULATORCONTROL2, 0x8F);
}

void iso6bClose()
{
}

s8 iso6bInventoryRound (Tag* tags, u8 maxtags, u8 mask, u8* filter, u8 startaddress)
{
    u8 found = 0;
    u8 cntFifoLength = 0;
    u16 resp = 0;
    u8 uid[8];
    u8 mem[8];
    u8 cnt;
    u8 max_depth = 10;
    u8 max_commands = 50;
    iso6bCmd nextCmd;

    /*  ISO 6B Type B UID 64 Byte */
    as3993SingleWrite(AS3993_REG_RXLENGTHUP, 0); 
    as3993SingleWrite(AS3993_REG_RXLENGTHLOW, 0x50);

    as3993EnterDirectMode();
    DIRECT_MODE_ENABLE_SENDER() ;
    iso6bResync();
    delay_us(600);
    iso6bSendInitialize();
    delay_us(600);
    iso6bSendSelect(mask, filter, startaddress);
    delay_us(20);
    DIRECT_MODE_ENABLE_RECEIVER() ;
    as3993ExitDirectMode();   
    as3993SingleCommand(AS3993_CMD_BLOCKRX);
    as3993SingleCommand(AS3993_CMD_ENABLERX);
    as3993WaitForResponse(RESP_RXDONE_OR_ERROR);
    resp = as3993GetResponse();
    as3993ClrResponse();

    if (resp & RESP_NORESINTERRUPT )
    {
        return 0;
    }
    if ((resp & AS3993_IRQ1_RX ) && !(resp & AS3993_IRQ1_RXERR)) // one Tag found
    {
        cntFifoLength = as3993SingleRead(AS3993_REG_FIFOSTATUS) & 0x1F; /*get the number of bytes */
        if (cntFifoLength)
            as3993FifoRead(cntFifoLength, &uid[0]);
        else
            return 0;

        found = 1;
        memcpy(&tags[0].epc , &uid[0] , 8);
    }
    else // more than one Tag found
    {
        /* crc error - two or more tags responded */
        nextCmd = iso6bSendFail;
        cnt = 2;
        while (maxtags && (cnt < max_depth) && max_commands--)
        {
            as3993EnterDirectMode();
            DIRECT_MODE_ENABLE_SENDER();
            nextCmd();
            DIRECT_MODE_ENABLE_RECEIVER();
            as3993ExitDirectMode();
            as3993SingleCommand(AS3993_CMD_BLOCKRX);
            as3993SingleCommand(AS3993_CMD_ENABLERX);
            as3993WaitForResponse(RESP_RXDONE_OR_ERROR);
            resp = as3993GetResponse();
            as3993ClrResponse();

            if (resp & AS3993_IRQ1_RXERR) // Collision
            {
                nextCmd = iso6bSendFail;
                cnt ++;
            }
            else if (resp & AS3993_IRQ1_RX ) // Tag found
            {
                cntFifoLength = as3993SingleRead(AS3993_REG_FIFOSTATUS) & 0x1F;
                if (cntFifoLength)
                {
                    as3993FifoRead(cntFifoLength, &uid[0]);
                    iso6bRead(uid, 0x0, mem); /* send data_read command so the tag changes its state */
                    memcpy(&tags[found].epc , &uid[0] , 8);
                    found ++;
                    maxtags --;
                    cnt --;
                    nextCmd = iso6bSendSuccess;
                }
                else
                {
                    nextCmd = iso6bSendFail;
                    cnt ++;
                }
            }
            else if (resp & RESP_NORESINTERRUPT ) // No Response
            {
                nextCmd = iso6bSendSuccess;
                cnt --;
            }
            else // Default
            {
                if ( nextCmd != iso6bSendResend )
                {
                    nextCmd = iso6bSendResend;
                }
                else
                {
                    nextCmd = iso6bSendFail;
                    cnt ++;
                }
            }
        }
    }

    return found;
}

s8 iso6bRead(u8* uid, u8 addr, u8* buffer)
{
    s8 cntSend, i;
    u8 payload[10];
    u8 cntFifoLength;
    u16 crc;
    u16 resp;

    /*  ISO 6B Word data 64 Bit */
    as3993SingleWrite(AS3993_REG_RXLENGTHUP, 0);
    as3993SingleWrite(AS3993_REG_RXLENGTHLOW, 0x50);
    /*  Enter Direct Mode and Sender */
    as3993EnterDirectMode();
    DIRECT_MODE_ENABLE_SENDER() ;
    /*  Prepare and Send Data */
    cntSend = iso6bPreparePreambleDelimiter(cmdData);
    cntSend += iso6bPrepareByte(&cmdData[cntSend], ISO6B_READ);
    payload[0] = ISO6B_READ;

    for (i = 0; i < 8; i++)
    {
        payload[i+1] = uid[i];
        cntSend += iso6bPrepareByte(&cmdData[cntSend], payload[i+1]);
    }

    payload[9] = addr;
    cntSend += iso6bPrepareByte(&cmdData[cntSend], addr);

    crc = calcCrc16(payload, 10);
    cntSend += iso6bPrepareCRC(&cmdData[cntSend], crc);
    bbRun(cmdData, cntSend);
    /*  Receive the Data */
    delay_us(20);
    DIRECT_MODE_ENABLE_RECEIVER();
    as3993ExitDirectMode();
    as3993SingleCommand(AS3993_CMD_BLOCKRX);
    as3993SingleCommand(AS3993_CMD_ENABLERX);
    as3993WaitForResponse(RESP_RXDONE_OR_ERROR);

    resp = as3993GetResponse();
    as3993ClrResponse();

    if (resp & RESP_NORESINTERRUPT )
    {
        return ISO6B_ERR_NOTAG;
    }
    else if ((resp & AS3993_IRQ1_RX ) && !(resp & AS3993_IRQ1_RXERR))   // one Tag found
    {
        cntFifoLength = as3993SingleRead(AS3993_REG_FIFOSTATUS) & 0x1F; /*get the number of bytes */
        if (cntFifoLength == 8)
        {
            as3993FifoRead(cntFifoLength, buffer);
            return ISO6B_ERR_NONE;
        }
        else
            return ISO6B_ERR_REG_FIFO;
    }

    return ISO6B_ERR_IRQ;

}

s8 iso6bWrite(u8* uid, u8 addr, u8 data, u8* buffer)
{
    s8 cntSend, i;
    u8 payload[11];
    u16 crc;
    u16 resp;
    u8 cntFifoLength;

   /*  ISO 6B Response data 24 Bit */
    as3993SingleWrite(AS3993_REG_RXLENGTHUP, 0);
    as3993SingleWrite(AS3993_REG_RXLENGTHLOW, 0x18);
    /*  Enter Direct Mode and Sender */
    as3993EnterDirectMode();
    DIRECT_MODE_ENABLE_SENDER() ;
    /*  Prepare and Send Data */
    cntSend = iso6bPreparePreambleDelimiter(cmdData);
    cntSend += iso6bPrepareByte(&cmdData[cntSend], ISO6B_WRITE);
    payload[0] = ISO6B_WRITE;

    for (i = 0; i < 8; i++)
    {
        payload[i+1] = uid[i];
        cntSend += iso6bPrepareByte(&cmdData[cntSend], payload[i+1]);
    }
    payload[9] = addr;
    cntSend += iso6bPrepareByte(&cmdData[cntSend], addr);

    payload[10] = data;
    cntSend += iso6bPrepareByte(&cmdData[cntSend], data);

    crc = calcCrc16(payload, 11);
    cntSend += iso6bPrepareCRC(&cmdData[cntSend], crc);
    bbRun(cmdData, cntSend);

    /*  Receive the Data */
    delay_us(20);
    DIRECT_MODE_ENABLE_RECEIVER();
    as3993ExitDirectMode();
    as3993SingleCommand(AS3993_CMD_BLOCKRX);
    as3993SingleCommand(AS3993_CMD_ENABLERX);
    as3993WaitForResponse(RESP_RXDONE_OR_ERROR);

    resp = as3993GetResponse();
    as3993ClrResponse();
    if (resp & RESP_NORESINTERRUPT )
    {
        return ISO6B_ERR_NOTAG;
    }
    else if ((resp & AS3993_IRQ1_RX ) && !(resp & AS3993_IRQ1_RXERR))   // one Tag found
    {
        cntFifoLength = as3993SingleRead(AS3993_REG_FIFOSTATUS) & 0x1F; /*get the number of bytes */
        if (cntFifoLength == 1)
        {
            as3993FifoRead(cntFifoLength, buffer);
            return ISO6B_ERR_NONE;
        }
        else
            return ISO6B_ERR_REG_FIFO;
    }

    return ISO6B_ERR_IRQ;

}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/
/*!
 *****************************************************************************
 *  \brief  Write the preamble and delimiter into the output packet
 *
 *  This function writes the preamble and delimiter into the output packet.
 *  According to the ISO18000-6B protocol, a packet from the interrogator
 *  must start with a preamble and a delimiter. See section 8.1.4.3. and
 *  8.1.4.4.2. of the ISO18000-6 document.
 *  The data is written to an output buffer which is then sent to the
 *  AS3991 via the bitbang module.
 *
 *  \param  preData : A buffer where the preamble and delimiter will be
 *                    written. Buffer must be allocated
 *                    and has to have a size of 4 (at least)
 *
 *\return : Actual size of the buffer used by the function
 *****************************************************************************
 */
static s8 iso6bPreparePreambleDelimiter(t_bbData *preData)
{
    /* prepare the preamble and the delimiter1 */
    /* preamble is 9 times 01 - see section 8.1.4.3 of ISO18000-6b doc */
    /* delimiter 1 is 1100111010 - see section 8.1.4.4.2 of ISO18000-6b doc */
    preData[0].bbdata = 0x55; /* 01010101 */
    preData[1].bbdata = 0x55; /* 01010101 */
    preData[2].bbdata = 0x73;  /* 01 and 110011 */
    preData[3].bbdata = 0xa;  /* 1010 */
    preData[0].length = 8;
    preData[1].length = 8;
    preData[2].length = 8;
    preData[3].length = 4;
    return 4;
}

/*!
 *****************************************************************************
 *  \brief  Send out a resync to the tags via the AS3993
 *
 *  This function is used to send out a resync signal to the tags. This is
 *  needed to initialize the internal data recovery circuit of the tag
 *  which calibrates the tag. (Especially after read and write commands)
 *
 *****************************************************************************
 */
static void iso6bResync()
{
    t_bbData resyncdata[3];
    DM_TX = HIGH;
    delay_ms(1);
    /* send 10 times 10 */
    resyncdata[0].bbdata = 0xaa; /* 10101010 */
    resyncdata[1].bbdata = 0xaa; /* 10101010 */
    resyncdata[2].bbdata = 0xa; /* 1010     */
    resyncdata[0].length = 8;
    resyncdata[1].length = 8;
    resyncdata[2].length = 4;

    bbRun(resyncdata, 3); /* BANG */
    DM_TX = HIGH;
}

/*!
 *****************************************************************************
 *  \brief  Convert a byte to manchester coded data
 *
 *  The data within an ISO18000-6B packet is manchester encoded. E.g. a 0
 *  is encoded as 01 and a 1 is encoded as 10.
 *
 *  \param[out] manchdata : Buffer where the manchster coded data is stored.
 *  \param[in] byteData : data to write
 * 
 * \return : Actual size of the buffer used by the function
 *****************************************************************************
 */
static s8 iso6bPrepareByte (t_bbData *manchdata, u8 byteData)
{
    manchdata[0].bbdata = 0;
    manchdata[1].bbdata = 0;
    manchdata[0].length = 8;
    manchdata[1].length = 8;

    if (byteData & BIT7) manchdata[0].bbdata |= 2 << 6;
    else manchdata[0].bbdata |= 1 << 6;
    if (byteData & BIT6) manchdata[0].bbdata |= 2 << 4;
    else manchdata[0].bbdata |= 1 << 4;
    if (byteData & BIT5) manchdata[0].bbdata |= 2 << 2;
    else manchdata[0].bbdata |= 1 << 2;
    if (byteData & BIT4) manchdata[0].bbdata |= 2 << 0;
    else manchdata[0].bbdata |= 1 << 0;
    if (byteData & BIT3) manchdata[1].bbdata |= 2 << 6;
    else manchdata[1].bbdata |= 1 << 6;
    if (byteData & BIT2) manchdata[1].bbdata |= 2 << 4;
    else manchdata[1].bbdata |= 1 << 4;
    if (byteData & BIT1) manchdata[1].bbdata |= 2 << 2;
    else manchdata[1].bbdata |= 1 << 2;
    if (byteData & BIT0) manchdata[1].bbdata |= 2 << 0;
    else manchdata[1].bbdata |= 1 << 0;
    return 2;
}

/*!
 *****************************************************************************
 *  \brief  Convert a the CRC checksum to manchester coded data
 *
 *  The data within an ISO18000-6B packet is manchester encoded. E.g. a 0
 *  is encoded as 01 and a 1 is encoded as 10. This function is used to convert
 *  the already calculated CRC-16 sum to manchester encoded data. The checksum
 *  is decribed in section 6.5.7.3. in ISO18000-6 doc
 *
 *  \param  crcData : A buffer where the encoded CRC will be
 *                    written. Buffer must be allocated
 *                    and has to have a size of 4 (at least)
 *  \param  crc : CRC sum to send
 *
 *\return : Actual size of the buffer used by the function
 *****************************************************************************
 */
static s8 iso6bPrepareCRC(t_bbData *crcData, s16 crc)
{
    /* send inverted crc (ISO18000-6 spec, chapter 6.5.7.3) */
    crc = ~crc;
    iso6bPrepareByte (crcData, (0xff00 & crc) >> 8);
    crcData += 2;
    iso6bPrepareByte (crcData, 0xff & crc);
    return 4; /* return number of used bytes */
}

/*!
 *****************************************************************************
 *  \brief  Send a command in direct mode according to ISO18000-6
 *
 *  This function sends the command via the AS3993 to the tags.
 *  It only supports simple commands, i.e. commands where no additional
 *  information or data payload is needed. Note that the AS3993 needs to be
 *  in direct mode when using this function.
 *
 *  \param[in]  cmd : Command code to send
 *
 *****************************************************************************
 */
static void iso6bSendCmd(u8 cmd)
{
    s8 cnt;
    u16 crc;

    DM_TX = HIGH;
    delay_ms(1);
    cnt = iso6bPreparePreambleDelimiter(cmdData);
    cnt += iso6bPrepareByte(&cmdData[cnt], cmd);

    crc = calcCrc16(&cmd, 1);
    cnt += iso6bPrepareCRC(&cmdData[cnt], crc);

    bbRun(cmdData, cnt);
    DM_TX = HIGH;
}


/*!
 *****************************************************************************
 *  \brief  Issue RESEND command according to ISO18000-6
 *
 *  The resend command is used during collision abitration. RESEND is issued
 *  when tags with count = 0 shall resend their UID while other tags shall
 *  keep their current count value.
 *
 *****************************************************************************
 */
static void iso6bSendResend()
{
    iso6bSendCmd(ISO6B_RESEND);
}

/*!
 *****************************************************************************
 *  \brief  Issue INITIALIZE command according to ISO18000-6
 *
 *  The initialize command sets all tags into state READY.
 *
 *****************************************************************************
 */
static void iso6bSendInitialize()
{
    iso6bSendCmd(ISO6B_INITIALIZE);
}

/*!
 *****************************************************************************
 *  \brief  Issue SUCCESS command according to ISO18000-6
 *
 *  The SUCCESS command is used during collision arbitration. SUCCESS is
 *  issued when NO reply has been received, i.e. all tags in the field
 *  have count > 0. As soon as the tags receive the SUCCESS command, they
 *  decrement their counter. If count is 0 the tag transmits its uid.
 *
 *****************************************************************************
 */
static void iso6bSendSuccess()
{
    iso6bSendCmd(ISO6B_SUCCESS);
}

/*!
 *****************************************************************************
 *  \brief  Issue FAIL command according to ISO18000-6
 *
 *  The FAIL command is used during collision arbitration. FAIL is
 *  issued when there was a CRC error, i.e. more then one tag have replied.
 *  Tags having a count of 0 receiving the FAIL command call a random function
 *  (rand(0, 1)). If the function returns 0 they leave their internal count
 *  and resend the UID. If it returns 1 then they increment their counter, i.e.
 *  count is 1 then. All tags having a count greater than 0 increment their
 *  counter when they receive the FAIL command, i.e. they move fruther away
 *  from sending their UID.
 *
 *****************************************************************************
 */
static void iso6bSendFail()
{
    iso6bSendCmd(ISO6B_FAIL);
}

/*!
 *****************************************************************************
 *  \brief  Issue the GROUP_SELECT command according to ISO18000-6
 *
 *  The GROUP_SELECT command starts the collision arbitration. All tags
 *  receiving this command change their state from READY to ID, i.e.
 *  they set their internal counter to 0 and send their UID.
 *
 *****************************************************************************
 */
static void iso6bSendSelect(u8 mask, u8* filter, u8 startaddress)
{
    u8 bsData[11];
    s8 cnt;
    s8 i;
    u16 crc;

    cnt = iso6bPreparePreambleDelimiter(cmdData);
    /* issue GROUP_SELECT_EQ command (0x0) */
    bsData[0] = ISO6B_GROUP_SELECT_EQ;
    cnt += iso6bPrepareByte(&cmdData[cnt], bsData[0]);
    /* address */
    bsData[1] = startaddress;
    cnt += iso6bPrepareByte(&cmdData[cnt], bsData[1]);
    /* mask - if mask is 0 all tags are selected */
    bsData[2] = mask;
    cnt += iso6bPrepareByte(&cmdData[cnt], bsData[2]);
    /* data */
    for (i = 3; i < 11; i++)
    {
        bsData[i] = *filter++;
        cnt += iso6bPrepareByte(&cmdData[cnt], bsData[i]);
    }

    crc = calcCrc16(bsData, 11);
    cnt += iso6bPrepareCRC(&cmdData[cnt], crc);
    bbRun(cmdData, cnt);

}

#endif
