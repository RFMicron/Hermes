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
/** @file
  * @brief This file includes functions providing an implementation of the ISO6c aka GEN2 RFID EPC protocol.
  *
  * Detailed documentation of the provided functionality can be found in gen2.h.
  *
  * @author Ulrich Herrmann
  * @author Bernhard Breinbauer
  */
#include "as3993_config.h"
#include "platform.h"
#include "as3993.h"
#include "logger.h"
#include "timer.h"
#include "gen2.h"
#include "string.h"

/** Definition for debug output: epc.c */
#define EPCDEBUG          0

#if EPCDEBUG && (USE_LOGGER == LOGGER_ON)
#define EPCLOG dbgLog /*!< macro used for printing debug messages */
#define EPCLOGDUMP dbgHexDump /*!< macro used for dumping buffers */
#else
//if debugging is disabled set empty log functions.
#define EPCLOG(...) /*!< macro used for printing debug messages if USE_LOGGER is set */
#define EPCLOGDUMP(...) /*!< macro used for dumping buffers if USE_LOGGER is set */
#endif

/*EPC Commands */
/** Definition for queryrep EPC command */
#define EPC_QUERYREP      0
/** Definition for acknowldege EPC command */
#define EPC_ACK           1
/** Definition for query EPC command */
#define EPC_QUERY         0x08
/** Definition for query adjust EPC command */
#define EPC_QUERYADJUST   0x09
/** Definition for select EPC command */
#define EPC_SELECT        0x0A
/** Definition for not acknowldege EPC command */
#define EPC_NAK           0xC0
/** Definition for request new RN16 or handle */
#define EPC_REQRN         0xC1
/** Definition for read EPC command */
#define EPC_READ          0xC2
/** Definition for write EPC command */
#define EPC_WRITE         0xC3
/** Definition for kill EPC command */
#define EPC_KILL          0xC4
/** Definition for lock EPC command */
#define EPC_LOCK          0xC5
/** Definition for access EPC command */
#define EPC_ACCESS        0xC6
/** Definition for blockwrite EPC command */
#define EPC_BLOCKWRITE    0xC7
/** Definition for blockerase EPC command */
#define EPC_BLOCKERASE    0xC8
/** Definition for block permalock EPC command */
#define EPC_BLOCKPERMALOCK 0xC9

#define GEN2_RESET_TIMEOUT 10
/*------------------------------------------------------------------------- */
/* local types */
/*------------------------------------------------------------------------- */

struct gen2InternalConfig{
    struct gen2Config config;
    u8 DR; /* Division ratio */
    u8 no_resp_time; /* value for AS3993_REG_RXNORESPONSEWAITTIME */
};

/*------------------------------------------------------------------------- */
/*global variables */
/*------------------------------------------------------------------------- */

/** Global buffer for generating data, sending to the Tag.\n
  * Caution: In case of a global variable pay attention to the sequence of generating
  * and executing epc commands.
  */
static u8 buf_[8+EPCLENGTH+PCLENGTH+CRCLENGTH]; /*8->tx_length+EPCcommand+wordptr+wordcount+handle+broken byte */

static struct gen2InternalConfig gen2IntConfig;

/*------------------------------------------------------------------------- */
/* local prototypes */
/*------------------------------------------------------------------------- */


/*------------------------------------------------------------------------- */
/** EPC REQRN command send to the Tag. This function is used to
  * request a new RN16 or a handle from the tag.
  *
  * @param *handle Pointer to the first byte of the handle.
  * @param *dest_handle Pointer to the first byte of the backscattered handle.
  * @return The function returns an errorcode.
                  0x00 means no Error occoured.
                  0xff means Error occoured.
  */
static s8 gen2ReqRNHandleChar(u8 const * handle, u8 *dest_handle);

static u8 gen2InsertEBV(u32 value, u8 *p, u8 bitpos)
{
    u8 ebv[5];
    u8 ebvlen;
    u32ToEbv(value, ebv, &ebvlen);
    insertBitStream(p, ebv, ebvlen, bitpos);
    return ebvlen;
}

static void gen2GetAgcRssi(u8 *agc, u8 *rssi)
{
    u8 buf[2];
    as3993ContinuousRead(AS3993_REG_AGCANDSTATUS, 2, buf);
    *agc = buf[0];
    *rssi = buf[1];
}
/*------------------------------------------------------------------------- */
/* global functions */
/*------------------------------------------------------------------------- */

void gen2Select(struct gen2SelectParams *p)
{
#if !RUN_ON_AS3980     // no select on AS3980 available
    u16 len = p->mask_len;
    u8 ebvLen;
    u16 rxbits = 1;
    u8 *mask = p->mask;
    u8 j,i;
    u8 resp_null;
    u8 jLeftBits;
    u8 posTruncbit;
    as3993ClrResponse();
    memset(buf_,0,sizeof(buf_));

    buf_[0] = ((EPC_SELECT<<4)&0xF0) | ((p->target<<1)&0x0E)   | ((p->action>>2)&0x01);
    buf_[1] = ((p->action<<6)&0xC0)  | ((p->mem_bank<<4)&0x30);
    ebvLen = gen2InsertEBV(p->mask_address, buf_+1, 4);
    buf_[1+ebvLen] = ((p->mask_len>>4)&0x0F);
    buf_[2+ebvLen] = ((p->mask_len<<4)&0xF0);

    i = 2+ebvLen; /* the counter set to last buffer location */
    for (j = len; j >= 8 ; j -= 8, mask++)
    {
        buf_[i] |= 0x0F & (*mask >> 4);
        i++;
        buf_[i] = 0xF0 & (*mask << 4);
    }

    if (j == 0)
    {
            buf_[i] |=  ((p->truncation<<3)&0x08)/*Truncate*/;
    }
    else    // if length is not dividable by 8
    {   jLeftBits = 0xFF << (8-j);

        buf_[i]   |= (jLeftBits >> 4) & (*mask >> 4);
        buf_[i+1]  = (jLeftBits << 4) & (*mask << 4);
        // add truncation bit
        posTruncbit = (p->truncation & 0x01) <<(7-j);
        buf_[i]   |= (posTruncbit >> 4) ;
        buf_[i+1] |= (posTruncbit << 4) ;
    }

    len += 21 + ebvLen * 8; /* Add the bits for EPC command before continuing */

    /* Pseudo 1-bit receival with small timeout to have AS3993 state machine 
       finished and avoiding spurious interrupts (no response) */
    as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRC, buf_, len, &resp_null, &rxbits, 1, 0, 1);
#endif
}

static void gen2PrepareQueryCmd(u8 *buf, u8 q)
{
    buf[0] = ((gen2IntConfig.DR<<5)&0x20)/*DR*/ |
              ((gen2IntConfig.config.miller<<3)&0x18)/*M*/ |
              ((gen2IntConfig.config.trext<<2)&0x04)/*TREXT*/ |
              ((gen2IntConfig.config.sel<<0)&0x03)/*SEL*/;
    buf[1] = ((gen2IntConfig.config.session<<6)&0xC0)/*SESSION*/ |
              ((gen2IntConfig.config.target<<5)&0x20)/*TARGET*/ |
              ((q<<1)&0x1E)/*Q*/;

}

s8 gen2QueryMeasureRSSI(u8 *agc, u8 *log_rssis, s8 *irssi, s8 *qrssi)
{
    u16 rxlen;
    u8 rn16[2];
    s8 ret = 0;
    u8 reg;
    uint i = 100;

    reg = as3993SingleRead(AS3993_REG_MEASUREMENTCONTROL);

    /*********************************************************************************/
    /* Send simple query command, make the tag react immediately                     */
    /*********************************************************************************/
    gen2PrepareQueryCmd(buf_, 0);
    rxlen = 16;
    ret = as3993TxRxGen2Bytes(AS3993_CMD_QUERY,buf_,16,NULL,&rxlen,gen2IntConfig.no_resp_time,0,1);

    if (ret)
    {
        return ret;
    }

    while (i--)
    { /* Check rx_status, if 1 receive is in progress/has started */
        if (0x40 & as3993SingleRead(AS3993_REG_FIFOSTATUS)) break;
    }

    gen2GetAgcRssi(agc, log_rssis);
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, (reg & ~0xf) | 0xb );
    *irssi = as3993GetADC(); /* RSSI I level */
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, (reg & ~0xf) | 0xc );
    *qrssi = as3993GetADC(); /* RSSI Q level */

    rxlen = 16;
    ret = as3993TxRxGen2Bytes(0,NULL,NULL,rn16,&rxlen,gen2IntConfig.no_resp_time,0,0);

    return ret;
}

/*
 * Sends a Query, QueryRep or QueryAdjust command and checks for replies from tags.
 * If a tag is found, the Ack command is sent. If fast mode is not enabled the Reg_RN
 * command will be sent after receiving the Ack reply.\n
 * All of the data which is retrieved from the tag is written into parameter tag.
 * \param tag The reply data of the tag is written into this data structure
 * \param qCommand The command which should be sent. Should be Query, QueryRep or QueryAdjust
 * \param q Q as specified in the Gen2 spec
 * \param fast If fast mode is activated no Req_RN command will be sent after receiving the
 * Ack reply. If no QueryRep, QueryAdjust or Reg_RN is sent to the tag within T2 the tag will
 * go back to arbitrate state.
 * \param followCommand The command which will be sent after a slot has been processed
 * successfully (valid tag response). Usually this will be used to send a QueryRep
 * immediately after receiving a tag response to trigger a change of the current
 * session flag.
 * \return 1 if one tag has been succesfully read, 0 if no response in slot, -1 if error occured (collision)
 */
static s8 gen2Slot(Tag *tag, u8 qCommand, u8 q, BOOL fast, u8 followCommand)
{
    u16 rxlen;
    s8 ret = 0;

    /*********************************************************************************/
    /* 1. Send proper query command */
    /*********************************************************************************/
    rxlen = 16;
    switch (qCommand)
    {
        case AS3993_CMD_QUERY:
            gen2PrepareQueryCmd(buf_, q);
            ret = as3993TxRxGen2Bytes(AS3993_CMD_QUERY,buf_,16,tag->rn16,&rxlen,gen2IntConfig.no_resp_time,AS3993_CMD_ACK,1);
            break;
        case 0: //query rep has already been sent
            ret = as3993TxRxGen2Bytes(0,NULL,0,tag->rn16,&rxlen,gen2IntConfig.no_resp_time,AS3993_CMD_ACK,1);
            break;
        default:
            ret = as3993TxRxGen2Bytes(qCommand,NULL,0,tag->rn16,&rxlen,gen2IntConfig.no_resp_time,AS3993_CMD_ACK,1);
    }
    if (ret == AS3993_ERR_NORES)
    {
        EPCLOG("  query -> noresp\n");
        return 0;
    }
    if (ret < 0)
    {
        EPCLOG("  query -> err %hhx\n", ret);
        return -1;
    }

    /*********************************************************************************/
    /* 2. Sent ACK, Receive pc, epc, send REQRN immediately after FIFO has the data  */
    /*********************************************************************************/
    rxlen = sizeof(buf_)*8;  //receive all data, length auto-calculated by AS3993
    ret = as3993TxRxGen2Bytes(0,buf_,0,buf_,&rxlen,gen2IntConfig.no_resp_time,fast?followCommand:AS3993_CMD_REQRN,0);
    tag->pc[0] = buf_[0];
    tag->pc[1] = buf_[1];
    if (ret < 0 || rxlen < 16)
    {
        EPCLOG("  ack rx rest(pc=%hhx) -> err %hhx\n", tag->pc[0], ret);
        return -1;
    }
#if RUN_ON_AS3980 
    tag->epclen = (rxlen+7)/8-4;    //on AS3980 crc is in the fifo, omit it.
#else
    tag->epclen = (rxlen+7)/8-2;
#endif
    if(tag->epclen > EPCLENGTH)
        tag->epclen = EPCLENGTH;
    memcpy(tag->epc, buf_+2, tag->epclen);

    if (fast)
    {
        gen2GetAgcRssi(&tag->agc, &tag->rssi);
        if (((tag->pc[0] & 0xF8) >> 2) != tag->epclen)
        {
            return -1;
        }
        return 1;
    }
    /*********************************************************************************/
    /* 3. Receive the two bytes handle */
    /*********************************************************************************/
    rxlen = 32;
    ret = as3993TxRxGen2Bytes(0,NULL,0,tag->handle,&rxlen,gen2IntConfig.no_resp_time,followCommand,0);
    gen2GetAgcRssi(&tag->agc, &tag->rssi);
    if (ret < 0)
    {
        EPCLOG("  reqrn -> err %hhx\n", ret);
        return -1;
    }
    if (((tag->pc[0] & 0xF8) >> 2) != tag->epclen)
    {
        return -1;
    }
    return 1;
}


/** Same as gen2Slot() but uses autoACK feature of reader.
 */
static s8 gen2SlotAutoAck(Tag *tag, u8 qCommand, u8 q, BOOL fast, u8 followCommand)
{
    u16 rxlen;
    s8 ret = 0;

    /*********************************************************************************/
    /* 1. Send proper query command */
    /*********************************************************************************/
    rxlen = sizeof(buf_)*8;  //receive all data, length auto-calculated by AS3993
    switch (qCommand)
    {
        case AS3993_CMD_QUERY:
            gen2PrepareQueryCmd(buf_, q);
            ret = as3993TxRxGen2Bytes(AS3993_CMD_QUERY,buf_,16,buf_,&rxlen,gen2IntConfig.no_resp_time,fast?followCommand:0,0);
            break;
        case 0:
            ret = as3993TxRxGen2Bytes(0,NULL,0,buf_,&rxlen,gen2IntConfig.no_resp_time,fast?followCommand:0,0);
            break;
        default:
            ret = as3993TxRxGen2Bytes(qCommand,NULL,0,buf_,&rxlen,gen2IntConfig.no_resp_time,fast?followCommand:0,0);
    }
    if (ret == AS3993_ERR_NORES)
    {
        EPCLOG("  query -> noresp\n");
        return 0;
    }

    /*********************************************************************************/
    /* 2. ACK was sent automatically, pc and epc already received */
    /*********************************************************************************/
    tag->pc[0] = buf_[0];
    tag->pc[1] = buf_[1];
    if (ret < 0 || rxlen < 16)
    {
        EPCLOG("  auto ack rx (pc=%hhx) -> err %hhx\n", tag->pc[0], ret);
        return -1;
    }
#if RUN_ON_AS3980 
    tag->epclen = (rxlen+7)/8-4;    //on AS3980 crc is in the fifo, omit it.
#else
    tag->epclen = (rxlen+7)/8-2;
#endif
    if(tag->epclen > EPCLENGTH)
        tag->epclen = EPCLENGTH;
    memcpy(tag->epc, buf_+2, tag->epclen);

    if (fast)
    {
        gen2GetAgcRssi(&tag->agc, &tag->rssi);
        if (((tag->pc[0] & 0xF8) >> 2) != tag->epclen)
        {
            return -1;
        }
        return 1;
    }

    /*********************************************************************************/
    /* 4. Receive the two bytes handle */
    /*********************************************************************************/
    rxlen = 32;
    ret = as3993TxRxGen2Bytes(0,NULL,0,tag->handle,&rxlen,gen2IntConfig.no_resp_time,followCommand,0);
    gen2GetAgcRssi(&tag->agc, &tag->rssi);
    if (ret < 0)
    {
        EPCLOG("  reqrn -> err %hhx\n", ret);
        return -1;
    }

    if (((tag->pc[0] & 0xF8) >> 2) != tag->epclen)
    {
        return -1;
    }
    return 1;
}

/*------------------------------------------------------------------------- */
static s8 gen2ReqRNHandleChar(u8 const * handle, u8 *dest_handle)
{
    s8 ret;
    u16 rxbits = 32;

    buf_[0] = EPC_REQRN;                 /*Command REQRN */
    buf_[1] = handle[0];
    buf_[2] = handle[1];

    ret = as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRC,buf_,24,dest_handle,&rxbits,gen2IntConfig.no_resp_time,0,1);

    if (ret < 0) return GEN2_ERR_REQRN;

    return GEN2_OK;
}

/*------------------------------------------------------------------------- */
s8 gen2AccessTag(Tag const * tag, u8 const * password)
{
    s8 ret;
    s8 error;
    u8 count;
    u16 rxcount;
    u8 tagResponse[5];
    u8 temp_rn16[2];

    for (count = 0; count < 2; count++)
    {
        error = gen2ReqRNHandleChar(tag->handle, temp_rn16);
        if (error)
        {
            return(error);
        }

        buf_[0] = EPC_ACCESS;
        buf_[1] = password[0] ^ temp_rn16[0];
        buf_[2] = password[1] ^ temp_rn16[1];
        buf_[3] = tag->handle[0];
        buf_[4] = tag->handle[1];

        rxcount = 32;
        ret = as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRC,buf_,40,tagResponse,&rxcount,gen2IntConfig.no_resp_time,0,1);
        if (ret < 0)
        {
            EPCLOG("access failed, tx-ret: %hhx\n", ret);
            return GEN2_ERR_ACCESS;
        }
        password += 2;  /* Increase pointer by two to fetch the next bytes;- */

        if  ((tagResponse[1] != tag->handle[1]) ||
                (tagResponse[0] != tag->handle[0]))
        {
            EPCLOG("handle not correct\n");
            return GEN2_ERR_ACCESS;
        }
#if EPCDEBUG
        if (count ==0) EPCLOG("first  part of access ok\n");
        if (count ==1) EPCLOG("second part of access ok\n");
#endif
    }
    return ret;
}

/*------------------------------------------------------------------------- */
s8 gen2LockTag(Tag *tag, const u8 *mask_action, u8 *tag_reply)
{
    s8 ret;
#if EPCDEBUG
    u8 count;
#endif
    u16 rxbits = 32+1;

    *tag_reply = 0xa5;

    buf_[0] = EPC_LOCK;                 /*Command EPC_LOCK */

    buf_[1] = mask_action[0];
    buf_[2] = mask_action[1];

    buf_[3] = ((mask_action[2] ) & 0xF0);
    insertBitStream(&buf_[3], tag->handle, 2, 4);

#if EPCDEBUG
    EPCLOG("lock code\n");
    for (count=0; count<6; count++)
    {
        EPCLOG("%hhx ",buf_[count]);
    }
    EPCLOG("\n");
#endif

    ret = as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRCEHEAD, buf_, 44, buf_, &rxbits, 0xff, 0, 1);

    if (ERR_CHIP_HEADER == ret && rxbits) *tag_reply = buf_[0];
    return ret;
}

/*------------------------------------------------------------------------- */
s8 gen2KillTag(Tag const * tag, u8 const * password, u8 rfu, u8 recom, u8* tag_error)
{
    s8 error;
    u8 count;
    u8 temp_rn16[2];
    u16 rxbits = 32;
    u8 cmd = AS3993_CMD_TRANSMCRC;/* first command has no header Bit */
    u8 no_resp_time = gen2IntConfig.no_resp_time;

    *tag_error = 0xa5;
    for (count = 0; count < 2; count++)
    {
        error = gen2ReqRNHandleChar(tag->handle, temp_rn16);
        if (error)
        {
            break;
        }

        if (count==1)
        { /* Values for second part of kill */
            cmd = AS3993_CMD_TRANSMCRCEHEAD; /* expect header bit */
            rxbits = 32 + 1; /* add header bit */
            rfu = recom; /* different data for rfu/recom */
            no_resp_time = 0xff; /* waiting time up to 20ms */
        }

        buf_[0] = EPC_KILL;

        buf_[1] = password[0] ^ temp_rn16[0];
        buf_[2] = password[1] ^ temp_rn16[1];

        buf_[3] = ((rfu << 5) & 0xE0);
        insertBitStream(&buf_[3], tag->handle, 2, 5);

        error = as3993TxRxGen2Bytes(cmd, buf_, 43, buf_, &rxbits, no_resp_time, 0, 1);

        if ( error ) 
        {
            break;
        }
        password += 2;
    }
    if(ERR_CHIP_HEADER == error && rxbits) *tag_error = buf_[0];
    return error;
}

/*------------------------------------------------------------------------- */
s8 gen2WriteWordToTag(Tag const * tag, u8 memBank, u32 wordPtr,
                                  u8 const * databuf, u8 * tag_error)
{
    s8 error;
    u8 datab;
    s8 ret;
    u8 ebvlen;
    u8 temp_rn16[2];
    u16 rxbits = 32+1;
    *tag_error = 0xa5;

    error = gen2ReqRNHandleChar(tag->handle, temp_rn16);
    EPCLOG("wDtT %hx%hx->%hhx%hhx\n",wordPtr,databuf[0],databuf[1]);
    if (error)
    {
        EPCLOG("reqrn error\n");
        return(error);
    }

    buf_[0]  = EPC_WRITE;                 /*Command EPC_WRITE */

    buf_[1]  = (memBank << 6) & 0xC0;
    ebvlen = gen2InsertEBV(wordPtr, &buf_[1], 6);

    datab = databuf[0] ^ temp_rn16[0];
    buf_[1+ebvlen] |= ((datab >> 2) & 0x3F);
    buf_[2+ebvlen]  = (datab << 6) & 0xC0;

    datab = databuf[1] ^ temp_rn16[1];
    buf_[2+ebvlen] |= ((datab >> 2) & 0x3F);
    buf_[3+ebvlen]  = (datab << 6) & 0xC0;

    insertBitStream(&buf_[3+ebvlen], tag->handle, 2, 6);

    ret = as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRCEHEAD, buf_, 42+8*ebvlen, buf_, &rxbits, 0xff, 0, 1);

    if (ERR_CHIP_HEADER == ret && rxbits) *tag_error = buf_[0];
    EPCLOG("  smo %hhx\n",ret);
    EPCLOG("  handle %hhx %hhx\n",tag->handle[0],tag->handle[1]);
    EPCLOG("  buf_ %hhx %hhx\n",buf_[0],buf_[1]);
    EPCLOG("  rxbits = %hx\n",rxbits);
    return ret;
}

/*------------------------------------------------------------------------- */
s8 gen2ReadFromTag(Tag *tag, u8 memBank, u32 wordPtr,
                          u8 wordCount, u8 *destbuf)
{
    u16 bit_count = (wordCount * 2 + 4) * 8 + 1; /* + 2 bytes rn16 + 2bytes crc + 1 header bit */
    s8 ret;
    u8 ebvlen;

    buf_[0]  = EPC_READ;                 /*Command EPC_READ */
    buf_[1]  = (memBank << 6) & 0xC0;
    ebvlen = gen2InsertEBV(wordPtr, &buf_[1], 6);
    buf_[1+ebvlen] |= ((wordCount >> 2) & 0x3F);
    buf_[2+ebvlen]  = (wordCount << 6) & 0xC0;
    insertBitStream(&buf_[2+ebvlen], tag->handle, 2, 6);

    ret = as3993TxRxGen2Bytes(AS3993_CMD_TRANSMCRCEHEAD, buf_, 34+8*ebvlen, destbuf, &bit_count, gen2IntConfig.no_resp_time, 0, 1);
    
    EPCLOG("bank=%hhx, w=%hx%hx, wc = %hhx ret=%hhx\n",memBank,wordPtr,wordCount,ret);
    EPCLOGDUMP(destbuf,wordCount*2);
    return ret;
}

void gen2PrintGen2Settings()
{
    u8 buf[9];
    as3993ContinuousRead(AS3993_REG_PROTOCOLCTRL, 9, buf);
    LOG("Gen2 registers:\n");
    LOGDUMP(buf, 9);

}

/*------------------------------------------------------------------------------ */
void gen2PrintEPC(Tag *tag)
{
    u16 count;
    EPCLOG("Print PC %hhx %hhx\n",tag->pc[0], tag->pc[1]);
    EPCLOG("Print EPC, len= %hhx \n", tag->epclen);
    for (count=0; count<(tag->epclen); count++)
    {
        EPCLOG("%hhx ",tag->epc[count]);
    }
    EPCLOG("\n");
}

/*------------------------------------------------------------------------- */
void gen2PrintTagInfo(Tag *tag, u8 epclen, u8 tagNr)
{
    u8 count = 0;

    EPCLOG("TAG %hhx:\n",tagNr);
    EPCLOG("RN16: %hhx %hhx\n",tag->rn16[1]
                                 ,tag->rn16[0]);
    EPCLOG("Number of read bytes: %d\n",epclen+2);
    EPCLOG("PC: %hhx %hhx", tag->pc[1]
                             , tag->pc[0]);
    EPCLOG("EPC: ");
    while (count < epclen)
    {
        EPCLOG("%hhx ",tag->epc[count]);
        count++;
    }
    EPCLOG("\n");

    EPCLOG("EPCLEN (bytes): %hhd\n",tag->epclen);

    EPCLOG("HANDLE: %hhx %hhx\n", tag->handle[0]
                                   , tag->handle[1]);
}

unsigned gen2SearchForTags(Tag *tags_
                      , u8 maxtags
                      , u8 q
                      , BOOL (*cbContinueScanning)(void)
                      , BOOL singulate
                      , BOOL toggleSession
                      , void (*followTagCommand)(Tag *tag, s8 *readErr)
                      )
{
    u16 num_of_tags = 0;
    u16 collisions = 0;
    u16 slot_count;
    u8 i = 0;
    u8 addRounds = 1; /* the maximal number of rounds performed */
    u8 cmd = AS3993_CMD_QUERY;
    u8 followCmd = 0;
    s8 readErr[5];
     
#if !RUN_ON_AS3980 
    if (toggleSession)
        followCmd = AS3993_CMD_QUERYREP;
#endif

    if (followTagCommand != NULL)
    {
        followCmd = 0;  //no follow Command
        singulate = 1;  // not fast
    }
    as3993AntennaPower(1);
    as3993ContinuousRead(AS3993_REG_IRQSTATUS1, 2, &buf_[0]);    // ensure that IRQ bits are reset
    as3993ClrResponse();

    EPCLOG("Searching for Tags, maxtags=%hhd, q=%hhd\n",maxtags,q);
    EPCLOG("-------------------------------\n");

    for (i=0; i < maxtags; i++)   /*Reseting the TAGLIST */
    {
        tags_[i].rn16[0] = 0;
        tags_[i].rn16[1] = 0;
        tags_[i].epclen=0;
    }
    do
    {
        BOOL goOn;
        collisions = 0;
        slot_count = 1UL<<q;   /*get the maximum slot_count */
        do
        {
            if (num_of_tags >= maxtags)
            {/*    ERROR it is not possible to store more than maxtags Tags */
                break;
            }
            EPCLOG("next slot, command: %x\n", cmd);
            slot_count--;

            switch (gen2Slot(tags_+num_of_tags, cmd, q, !singulate, followCmd))
            {
                case -1:
                    //EPCLOG("collision\n");
                    collisions++;
                    cmd = AS3993_CMD_QUERYREP;
                    break;
                case 1:
                    if (followCmd)
                        cmd = 0;    // query_rep has already been sent as followCmd
                    else
                        cmd = AS3993_CMD_QUERYREP;
                    if (followTagCommand != NULL)
                    {
                        followTagCommand(tags_+num_of_tags, readErr);
                        cmd = AS3993_CMD_QUERYREP;
                    }
                    num_of_tags++;
                    break;
                case 0:
                    //EPCLOG("NO EPC response -> empty Slot\n");
                    cmd = AS3993_CMD_QUERYREP;
                    break;
                default:
                    break;
            }
            goOn = cbContinueScanning();
        } while (slot_count && goOn );
        addRounds--;
        EPCLOG("q=%hhx, collisions=%x, num_of_tags=%x",q,collisions,num_of_tags);
        if( collisions )
            if( collisions >= (1UL<<q) /4)
            {
                q++;
                EPCLOG("->++\n");
                cmd = AS3993_CMD_QUERYADJUSTUP;
            }
            else if( collisions < (1UL<<q) /8)
            {
                q--;
                EPCLOG("->--\n");
                cmd = AS3993_CMD_QUERYADJUSTDOWN;
            }
            else
            {
                EPCLOG("->==\n");
                cmd = AS3993_CMD_QUERYADJUSTNIC;
            }
        else
        {
            EPCLOG("->!!\n");
            addRounds = 0;
        }
    }while(num_of_tags < maxtags && addRounds && cbContinueScanning() );

    // If a tag is found in the last slot of an inventory round, follow Command QueryRep will be executed.
    // Add some time to send this command to the tag over the field.
    // If function will be left to early and field will be switched off the tag will not invert the inventoried flag.
    // Problem especially  for S2 and S3.
    // If there was a follow Tag Command a Query Rep. has to be sent to invert the session flag.

    if (followTagCommand == NULL)
    {
        delay_us(150);
    }
    else
    {
        as3993SingleCommand(AS3993_CMD_QUERYREP);
        as3993WaitForResponse(RESP_TXIRQ);
        as3993ClrResponse();
    }
    
#if EPCDEBUG
    EPCLOG("-------------------------------\n");
    EPCLOG("%hx  Tags found", num_of_tags);
    EPCLOG("\n");
#endif
    return num_of_tags;
}

unsigned gen2SearchForTagsAutoAck(Tag *tags_
                      , u8 maxtags
                      , u8 q
                      , BOOL (*cbContinueScanning)(void)
                      , BOOL singulate
                      , BOOL toggleSession
                      , void (*followTagCommand)(Tag *tag, s8 *readErr)
                      )
{
    u16 num_of_tags = 0;
    u16 collisions = 0;
    u16 slot_count;
    u8 i = 0;
    u8 cmd = AS3993_CMD_QUERY;
    u8 followCmd = 0;
    u8 autoAck;
    BOOL goOn = 1;
    s8 readErr[5];
    
    EPCLOG("Searching for Tags with autoACK, maxtags=%hhd, q=%hhd\n",maxtags, q);
    EPCLOG("-------------------------------\n");

#if !RUN_ON_AS3980
    if (toggleSession)
        followCmd = AS3993_CMD_QUERYREP;
#endif
    if (followTagCommand != NULL)
    {
        followCmd = 0;  //no follow Command
        singulate = 1;  // not fast
    }
    as3993AntennaPower(1);
    as3993ContinuousRead(AS3993_REG_IRQSTATUS1, 2, &buf_[0]);    // ensure that IRQ bits are reset
    as3993ClrResponse();

    //configure autoACK mode
    autoAck = as3993SingleRead(AS3993_REG_PROTOCOLCTRL);
    autoAck &= ~0x30;
    if (singulate)
        autoAck |= 0x20;
    else
        autoAck |= 0x10;
    as3993SingleWrite(AS3993_REG_PROTOCOLCTRL, autoAck);

    for (i=0; i < maxtags; i++)   /*Reseting the TAGLIST */
    {
        tags_[i].rn16[0] = 0;
        tags_[i].rn16[1] = 0;
        tags_[i].epclen=0;
    }

    slot_count = 1UL<<q;   /*get the maximum slot_count */
    do
    {
        if (num_of_tags >= maxtags)
        {/*    ERROR it is not possible to store more than maxtags Tags */
            break;
        }
        slot_count--;
        switch (gen2SlotAutoAck(tags_+num_of_tags, cmd, q, !singulate, followCmd))
        {
            case -1:
                //EPCLOG("collision\n");
                collisions++;
                cmd = AS3993_CMD_QUERYREP;
                break;
            case 1:

                if (followCmd)
                    cmd = 0;
                else
                    cmd = AS3993_CMD_QUERYREP;
                if (followTagCommand != NULL)
                {
                    followTagCommand(tags_+num_of_tags, readErr);
                    cmd = AS3993_CMD_QUERYREP;
                }
                num_of_tags++;               
                break;
            case 0:
                //EPCLOG("NO EPC response -> empty Slot\n");
                cmd = AS3993_CMD_QUERYREP;
                break;
            default:
                break;
        }
        goOn = cbContinueScanning();
    } while (slot_count && goOn );

    //unset autoACK mode again
    autoAck = as3993SingleRead(AS3993_REG_PROTOCOLCTRL);
    autoAck &= ~0x30;
    as3993SingleWrite(AS3993_REG_PROTOCOLCTRL, autoAck);

    // If a tag is found in the last slot of an inventory round, follow Command QueryRep will be executed.
    // Add some time to send this command to the tag over the field.
    // If function will be left to early and field will be switched off the tag will not invert the inventoried flag.
    // Problem especially  for S2 and S3.
    // If there was a follow Tag Command a Query Rep. has to be sent to invert the session flag.

    if (followTagCommand == NULL)
    {
        delay_us(150);
    }
    else
    {
        as3993SingleCommand(AS3993_CMD_QUERYREP);
        as3993WaitForResponse(RESP_TXIRQ);
        as3993ClrResponse();
    }

#if EPCDEBUG
    EPCLOG("-------------------------------\n");
    EPCLOG("%hx  Tags found", num_of_tags);
    EPCLOG("\n");
#endif
    return num_of_tags;
}

void gen2Configure(const struct gen2Config *config)
{
    /* depending on link frequency setting adjust */
    /* registers 01, 02, 03, 04, 05, 06, 07, 08 and 09 */
    u8 reg[9];
    u8 session = config->session;
    gen2IntConfig.DR = 1;
    gen2IntConfig.config = *config;
    if (session > GEN2_IINV_S3)
        session = GEN2_IINV_S0; /* limit SL and invalid settings */
    if (gen2IntConfig.config.miller == GEN2_COD_FM0 || gen2IntConfig.config.miller == GEN2_COD_MILLER2)
        gen2IntConfig.config.trext = TREXT_ON;

    switch (config->linkFreq) {
    case GEN2_LF_640:  /* 640kHz */
        reg[0] = 0x20; /* AS3993_REG_TXOPTIONS            */
        reg[1] = 0xF0; /* AS3993_REG_RXOPTIONS            */
        reg[2] = 0x01; /* AS3993_REG_TRCALHIGH            */
        reg[3] = 0x4D; /* AS3993_REG_TRCALLOW             */
        reg[4] = 0x03; /* AS3993_REG_AUTOACKTIMER         */
        reg[5] = 0x02; /* AS3993_REG_RXNORESPONSEWAITTIME */
        reg[6] = 0x01; /* AS3993_REG_RXWAITTIME           */
        if (gen2IntConfig.config.miller > GEN2_COD_MILLER2)
            reg[7] = 0x04; /* AS3993_REG_RXFILTER         */
        else
            reg[7] = 0x07; /* AS3993_REG_RXFILTER         */
        break;
    case GEN2_LF_320: /* 320kHz */
        reg[0] = 0x20;
        reg[1] = 0xC0;
        if (gen2IntConfig.config.tari == TARI_625)    /* if Tari = 6.25us */
        {   /* TRcal = 25us */
            gen2IntConfig.DR = 0;
            reg[2] = 0x00;
            reg[3] = 0xFA;
        } else
        {   /* TRcal = 66.7us */
            reg[2] = 0x02;
            reg[3] = 0x9B;
        }
        reg[4] = 0x04;
        reg[5] = 0x03;
        reg[6] = 0x02;
        if (gen2IntConfig.config.miller > GEN2_COD_MILLER2)
            reg[7] = 0x24; /* AS3993_REG_RXFILTER         */
        else
            reg[7] = 0x27; /* AS3993_REG_RXFILTER         */
        break;
    case GEN2_LF_256: /* 256kHz */
        reg[0] = 0x20;
        reg[1] = 0x90;
        if (gen2IntConfig.config.tari == TARI_625)    /* Tari = 6.25us */
        {   /* TRcal = 31.3us */
            gen2IntConfig.DR = 0;
            reg[2] = 0x01;
            reg[3] = 0x39;
        }
        else                                /* Tari = 25us or 12.5us */
        {   /* TRcal = 83.3us */
            reg[2] = 0x03;
            reg[3] = 0x41;
        }
        reg[4] = 0x05;
        reg[5] = 0x05;
        reg[6] = 0x04;
        if (gen2IntConfig.config.miller > GEN2_COD_MILLER2)
            reg[7] = 0x34; /* AS3993_REG_RXFILTER         */
        else
            reg[7] = 0x37; /* AS3993_REG_RXFILTER         */
        break;
    case GEN2_LF_213: /* 213.3kHz */
        reg[0] = 0x20;
        reg[1] = 0x80;
        if (gen2IntConfig.config.tari == TARI_625)    /* Tari = 6.25us */
        {   /* TRcal = 37.5us */
            gen2IntConfig.DR = 0;
            reg[2] = 0x01;
            reg[3] = 0x77;
        }
        else
        {   /* TRcal = 100us */
            reg[2] = 0x03;
            reg[3] = 0xE8;
        }
        reg[4] = 0x06;
        reg[5] = 0x05;
        reg[6] = 0x05;
        if (gen2IntConfig.config.miller > GEN2_COD_MILLER2)
            reg[7] = 0x34; /* AS3993_REG_RXFILTER         */
        else
            reg[7] = 0x37; /* AS3993_REG_RXFILTER         */
        break;
    case GEN2_LF_160: /* 160kHz */
        reg[0] = 0x20;
        reg[1] = 0x60;
        if (gen2IntConfig.config.tari == TARI_125)    /* Tari = 12.5us */
        {   /* TRcal = 50us */
            gen2IntConfig.DR = 0;
            reg[2] = 0x01;
            reg[3] = 0xF4;
        }
        else
        {   /* TRcal = 1333.3us */
            reg[2] = 0x05;
            reg[3] = 0x35;
        }
        reg[4] = 0x0A;
        reg[5] = 0x05;
        reg[6] = 0x08;
        if (gen2IntConfig.config.miller > GEN2_COD_FM0)
            reg[7] = 0x3F; /* AS3993_REG_RXFILTER         */
        else
            reg[7] = 0xBF; /* AS3993_REG_RXFILTER         */
        break;
    case GEN2_LF_40: /* 40kHz */
        reg[0] = 0x30; /* AS3993_REG_TXOPTIONS            */
        reg[1] = 0x00; /* AS3993_REG_RXOPTIONS            */
        reg[2] = 0x07; /* AS3993_REG_TRCALHIGH            */
        reg[3] = 0xD0; /* AS3993_REG_TRCALLOW             */
        reg[4] = 0x3F; /* AS3993_REG_AUTOACKTIMER         */
        reg[5] = 0x0C; /* AS3993_REG_RXNORESPONSEWAITTIME */
        reg[6] = 0x24; /* AS3993_REG_RXWAITTIME           */
        reg[7] = 0xFF; /* AS3993_REG_RXFILTER         */
        gen2IntConfig.DR = 0;
        break;
    default:
        return; /* use preset settings */
    }
    reg[0] |= gen2IntConfig.config.tari;
    reg[1] = (reg[1] & ~0x0F)
        | gen2IntConfig.config.miller
        | (gen2IntConfig.config.trext<<3);

    gen2IntConfig.no_resp_time = reg[5];
    /* Modify only the gen2 relevant settings */
    as3993ContinuousWrite(AS3993_REG_TXOPTIONS, reg+0, 8);

    reg[0] = as3993SingleRead(AS3993_REG_TXSETTING);
    reg[0] = (reg[0] & ~0x03) | gen2IntConfig.config.session;
    as3993SingleWrite(AS3993_REG_TXSETTING, reg[0]);
    reg[0] = as3993SingleRead(AS3993_REG_PROTOCOLCTRL);
    reg[0] = reg[0] & ~0x07;
    as3993SingleWrite(AS3993_REG_PROTOCOLCTRL, reg[0]);
}

void gen2Open(const struct gen2Config * config)
{
    gen2Configure( config );
}

void gen2Close(void)
{
}

struct gen2Config *getGen2IntConfig()
{
    return &(gen2IntConfig.config);
}
