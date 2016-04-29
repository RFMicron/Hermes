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
  * @brief
  * This file provides declarations for functions for the GEN2 aka ISO6c protocol.
  *
  * @author Ulrich Herrmann
  * @author Bernhard Breinbauer
  *
  * Before calling any of the functions herein the AS3993 chip needs to
  * be initialized using as3993Initialize(). Thereafter the function 
  * gen2Open() needs to be called for opening a session.
  * gen2SearchForTags() should be called to identify the tags in
  * reach. Usually the the user then wants to select one of the found
  * tags. For doing this gen2Select() can be used. If gen2Config and gen2SelectParams
  * provide a matching configuration the next call to gen2SearchForTags() will only
  * return 1 tag. If singulate parameter of gen2SearchForTags() is set to true
  * the found tag will be in the Open state and
  * can then be accessed using the other functions ,
  * gen2WriteWordToTag(), gen2ReadFromTag()...
  * When finished with gen2 operations this session should be closed using 
  * gen2Close(). 
  *
  * The tag state diagram looks as follows using the
  * provided functions. For exact details please refer to
  * the standard document provided by EPCglobal under 
  * <a href="http://www.epcglobalinc.org/standards/uhfc1g2/uhfc1g2_1_2_0-standard-20080511.pdf">uhfc1g2_1_2_0-standard-20080511.pdf</a>
  *
  * If the tag in question does have a password set:
  * \dot
  * digraph gen2_statesp{
  * Ready -> Open [ label="gen2SearchForTags()" ];
  * Ready->Ready;
  * Open -> Secured [ label="gen2AccessTag() + correct PW" ];
  * Open -> Open [ label="gen2ReadFromTag()\n..." ];
  * Secured -> Secured [ label="gen2WriteWordToTag()\ngen2ReadFromTag()\n..." ];
  * }
  * \enddot
  *
  * If the tag in question does <b>not</b> have a password set:
  * \dot
  * digraph gen2_statesn{
  * Ready -> Secured [ label="gen2SearchForTags()" ];
  * Secured -> Secured [ label="gen2WriteWordToTag()\ngen2ReadFromTag()\n..." ];
  * }
  * \enddot
  *
  * So a typical use case may look like this:
  * \code
  * Tag tags[16];
  * u8 tag_error;
  * struct gen2Config config = = {TARI_125, GEN2_LF_256, GEN2_COD_MILLER4, TREXT_OFF, 0, GEN2_SESSION_S0, 0};
  * // setup gen2SelectParams
  * unsigned n;
  * u8 buf[4];
  * ...
  * as3993Initialize(912000);
  *
  * gen2Open(&config);
 *
  * performSelects();
  * n = gen2SearchForTags(tags_+1, 1, 0, continueAlways, 1);
  * if ( n == 0) return;
  *
  * //Pick one of the tags based on the contents of tags. Here we use the very first tag
  *
  * if (gen2ReadFromTag(tags+0, MEM_USER, 0, 2, buf))
  *     return;
  *
  * buf[0] ^= 0xff; buf[1]^= 0x55; // change the data
  *
  * if (gen2WriteWordToTag( tags+0, MEM_USER, 0, buf, &tag_error))
  * { // wrote back one of the two read words
  *     gen2Close();
  *     return;
  * }
  *
  * //...
  *
  * gen2Close();
  *
  * \endcode
  *
  */

#ifndef __GEN2_H__
#define __GEN2_H__

#include "as3993_public.h"
#include "ams_types.h"
#include "errno_as3993.h"

/* Protocol configuration settings */
#define GEN2_LF_40  0   /*!<link frequency 40 kHz*/
#define GEN2_LF_160 6   /*!<link frequency 160 kHz*/
#define GEN2_LF_213 8   /*!<link frequency 213 kHz*/
#define GEN2_LF_256 9   /*!<link frequency 213 kHz*/
#define GEN2_LF_320 12  /*!<link frequency 213 kHz*/
#define GEN2_LF_640 15  /*!<link frequency 640 kHz*/

/** Rx coding values */
#define GEN2_COD_FM0      0 /*!<FM coding for rx */
#define GEN2_COD_MILLER2  1 /*!<MILLER2 coding for rx*/
#define GEN2_COD_MILLER4  2 /*!<MILLER4 coding for rx*/
#define GEN2_COD_MILLER8  3 /*!<MILLER8 coding for rx*/

/* TrExt defines */
#define TREXT_OFF         0 /**< use short preamble */
#define TREXT_ON          1 /**< use long preamble */

/* Tari defines */
#define TARI_625          0 /**< set tari to 6.25us */
#define TARI_125          1 /**< set tari to 12.5us */
#define TARI_25           2 /**< set tari to 25us */

/*EPC Mem Banks */
/** Definition for EPC tag memory bank: reserved */
#define MEM_RES           0x00
/** Definition for EPC tag memory bank: EPC memory */
#define MEM_EPC           0x01
/** Definition for EPC tag memory bank: TID */
#define MEM_TID           0x02
/** Definition for EPC tag memory bank: user */
#define MEM_USER          0x03

/*EPC Wordpointer Addresses */
/** Definition for EPC wordpointer: Address for CRC value */
#define MEMADR_CRC        0x00
/** Definition for EPC wordpointer: Address for PC value Word position*/
#define MEMADR_PC         0x01
/** Definition for EPC wordpointer: Address for EPC value */
#define MEMADR_EPC        0x02

/** Definition for EPC wordpointer: Address for kill password value */
#define MEMADR_KILLPWD    0x00
/** Definition for EPC wordpointer: Address for access password value */
#define MEMADR_ACCESSPWD  0x02

/** Definition for EPC wordpointer: Address for TID value */
#define MEMADR_TID        0x00

/*EPC SELECT TARGET */
/** Definition for inventory: Inventoried (S0) */
#define GEN2_IINV_S0           0x00 /*Inventoried (S0) */
/** Definition for inventory: 001: Inventoried (S1) */
#define GEN2_IINV_S1           0x01 /*001: Inventoried (S1) */
/** Definition for inventory: 010: Inventoried (S2) */
#define GEN2_IINV_S2           0x02 /*010: Inventoried (S2) */
/** Definition for inventory: 011: Inventoried (S3) */
#define GEN2_IINV_S3           0x03 /*011: Inventoried (S3) */
/** Definition for inventory: 100: SL */
#define GEN2_IINV_SL           0x04 /*100: SL */

#define GEN2_SESSION_S0           0x00
#define GEN2_SESSION_S1           0x01
#define GEN2_SESSION_S2           0x02
#define GEN2_SESSION_S3           0x03

/** Defines for various return codes */
#define GEN2_OK                  ERR_NONE /**< No error */
#define GEN2_ERR_REQRN           ERR_GEN2_REQRN /**< Error sending ReqRN */
#define GEN2_ERR_ACCESS          ERR_GEN2_ACCESS /**< Error sending Access password */
#define GEN2_ERR_SELECT          ERR_GEN2_SELECT /**< Error when selecting tag*/
#define GEN2_ERR_CHANNEL_TIMEOUT ERR_GEN2_CHANNEL_TIMEOUT /**< Error RF channel timed out*/

struct gen2Config{
    u8 tari;        /*< Tari setting */
    u8 linkFreq;    /*< GEN2_LF_40, ... */
    u8 miller;      /*< GEN2_COD_FM0, ... */
    u8 trext;       /*< 1 if the preamble is long, i.e. with pilot tone */
    u8 sel;         /*< For QUERY Sel field */
    u8 session;     /*< GEN2_SESSION_S0, ... */
    u8 target;      /*< For QUERY Target field */
};

struct gen2SelectParams{
    u8 target;
    u8 action;
    u8 mem_bank;
    u8 mask[32];
    u32 mask_address;
    u8 mask_len;
    u8 truncation;
};

/*------------------------------------------------------------------------- */
/** Search for tags (aka do an inventory round). Before calling any other
  * gen2 functions this routine has to be called. It first selects using the 
  * given mask a set of tags and then does an inventory round issuing query 
  * commands. All tags are stored in then tags array for examination by the 
  * user. 
  *
  * @param *tags an array for the found tags to be stored to
  * @param maxtags the size of the tags array
  * @param q 2^q slots will be done first, additional 2 round with increased 
  * or decreased q may be performed
  * @param cbContinueScanning callback is called after each slot to inquire if we should
  * continue scanning (e.g. for allowing a timeout)
  * @param singulate If set to true Req_RN command will be sent to get tag into Open state
  *                  otherwise it will end up in arbitrate
  * @param toggleSession If set to true, QueryRep commands will be sent immediately
  *                  after receiving tag reply to toggle session flag on tag.
  * @param followTagCommand callback function is called after a tag is inventoried.
  *        If function will be called, no fast mode is possible.
  * @return the number of tags found
  */
unsigned gen2SearchForTags(Tag *tags
                          , u8 maxtags
                          , u8 q
                          , BOOL (*cbContinueScanning)(void)
                          , BOOL singulate
                          , BOOL toggleSession
                          , void (*followTagCommand)(Tag *tag, s8 *readErr)
                          );


/** For reference see gen2SearchForTags(). The main difference is that it
  * uses the autoACK mode of the reader.
  */
unsigned gen2SearchForTagsAutoAck(Tag *tags_
                      , u8 maxtags
                      , u8 q
                      , BOOL (*cbContinueScanning)(void)
                      , BOOL singulate
                      , BOOL toggleSession
                      , void (*followTagCommand)(Tag *tag, s8 *readErr)
                      );
/*------------------------------------------------------------------------- */
/** EPC ACCESS command send to the Tag.
  * This function is used to bring a tag with set access password from the Open
  * state to the Secured state.
  *
  * @attention This command works on the one tag which is currently in the open 
  *            state, i.e. on the last tag found by gen2SearchForTags().
  *
  * @param *tag Pointer to the Tag structure.
  * @param *password Pointer to first byte of the access password
  * @return The function returns an errorcode.
                  0x00 means no Error occoured.
                  Any other value is the backscattered error code from the tag.
  */
s8 gen2AccessTag(Tag const * tag, u8 const * password);

/*------------------------------------------------------------------------- */
/** EPC LOCK command send to the Tag.
  * This function is used to lock some data region in the tag.
  *
  * @attention This command works on the one tag which is currently in the open 
  *            state, i.e. on the last tag found by gen2SearchForTags().
  *
  * @param *tag Pointer to the Tag structure.
  * @param *mask_action Pointer to the first byte of the mask and
                                    action array.
  * @param *tag_reply In case ERR_HEADER is returned this variable will 
  *                   contain the 8-bit error code from the tag.
  * @return The function returns an errorcode.
                  0x00 means no Error occoured.
                  Any other value is the backscattered error code from the tag.
  */
s8 gen2LockTag(Tag *tag, const u8 *mask_action, u8 *tag_reply);

/*------------------------------------------------------------------------- */
/** EPC KILL command send to the Tag.
  * This function is used to permanently kill a tag. After that the
  * tag will never ever respond again.
  *
  * @attention This command works on the one tag which is currently in the open 
  *            state, i.e. on the last tag found by gen2SearchForTags().
  *
  * @param *tag Pointer to the Tag structure.
  * @param *password Pointer to first byte of the kill password
  * @param rfu 3 bits used as rfu content for first half of kill, should be zero
  * @param recom 3 bits used as recom content for second half of kill, zero 
  *        for real kill, !=0 for recommisioning
  * @param *tag_error: in case header bit is set this will be the return code from the tag
  * @return The function returns an errorcode.
                  0x00 means no Error occoured.
                  Any other value is the backscattered error code from the tag.
  */
s8 gen2KillTag(Tag const * tag, u8 const * password, u8 rfu, u8 recom, u8 *tag_error);

/*------------------------------------------------------------------------- */
/** EPC WRITE command send to the Tag.
  * This function writes one word (16 bit) to the tag.
  * It first requests a new handle. The handle is then exored with the data.
  *
  * @attention This command works on the one tag which is currently in the open 
  *            state, i.e. on the last tag found by gen2SearchForTags().
  *
  * @param *tag Pointer to the Tag structure.
  * @param memBank Memory Bank to which the data should be written.
  * @param wordPtr Word Pointer Address to which the data should be written.
  * @param *databuf Pointer to the first byte of the data array. The data buffer
                             has to be 2 bytes long.
  * @param *tag_error In case tag returns an error (header bit set), this 
                      functions returns ERR_HEADER and inside tag_error the actual code
  * @return The function returns an errorcode.
                  0x00 means no error occoured.
                  0xFF unknown error occoured.
                  Any other value is the backscattered error code from the tag.
  */
s8 gen2WriteWordToTag(Tag const * tag, u8 memBank, u32 wordPtr, u8 const * databuf, u8 * tag_error);

/*------------------------------------------------------------------------- */
/** EPC READ command send to the Tag.
  *
  * @attention This command works on the one tag which is currently in the open 
  *            state, i.e. on the last tag found by gen2SearchForTags().
  *
  * @param *tag Pointer to the Tag structure.
  * @param memBank Memory Bank to which the data should be written.
  * @param wordPtr Word Pointer Address to which the data should be written.
  * @param wordCount Number of bytes to read from the tag.
  * @param *destbuf Pointer to the first byte of the data array.
  * @return The function returns an errorcode.
                  0x00 means no error occoured.
                  0xFF unknown error occoured.
                  Any other value is the backscattered error code from the tag.
  */
s8 gen2ReadFromTag(Tag *tag, u8 memBank, u32 wordPtr,
                          u8 wordCount, u8 *destbuf);

/*------------------------------------------------------------------------- */
/** EPC SELECT command. send to the tag.
  * This function does not take or return a parameter
  */
void gen2Select(struct gen2SelectParams *p);

/*------------------------------------------------------------------------------ */
/*Sends the tags EPC via the UART to the host */
/*The function needs the tags structure */
void gen2PrintEPC(Tag *tag);

/*------------------------------------------------------------------------- */
/** Prints the tag information out (UART).
  * @param *tag Pointer to the Tag structure.
  * @param epclen Length of the EPC.
  * @param tagNr Number of the tag.
  */
void gen2PrintTagInfo(Tag *tag, u8 epclen, u8 tagNr);

/*!
 *****************************************************************************
 *  \brief  Set the link frequency
 *
 *  Set the link frequency and gen2 specific parameters. After calling
 *  this function the AS3993 is in normal.
 *****************************************************************************
 */
void gen2Configure(const struct gen2Config *config);

/*!
 *****************************************************************************
 *  \brief  Open a session
 *
 * @param config: configuration to use 
 *
 *  Open a session for gen2 protocol
 *****************************************************************************
 */
void gen2Open(const struct gen2Config * config);

/*!
 *****************************************************************************
 *  \brief  Close a session
 *
 *  Close the session for gen2 protocol
 *****************************************************************************
 */
void gen2Close(void);

/*!
 *****************************************************************************
 *  \brief  Perform a gen2 QUERY command and measure received signal strength
 *
 *****************************************************************************
 */
s8 gen2QueryMeasureRSSI(u8 *agc, u8 *log_rssis, s8 *irssi, s8 *qrssi);

/*!
 *****************************************************************************
 *  \brief  Returns the Gen2 Configuration
 *  @return Gen2 Configuration
 *****************************************************************************
 */

struct gen2Config *getGen2IntConfig();


#endif
