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
 *  \brief ISO6B protocol header file
 *
 * Before calling any of the functions herein the AS399X chip needs to
 * be initialized using as3993Initialize(). Thereafter the function
 * iso6bOpen() needs to be called for opening a session. 
 * 
 * The following graph shows several states of an ISO 6B tag as well as their 
 * transitions based on iso6b commands.
 *
 * \dot
 * digraph iso6b_states{
 * Power_off -> Ready [ label="as3993AntennaPower(1)" ];
 * Ready -> Data_Exchange [ label="iso6bRead()" ];
 * Ready -> Data_Exchange [ label="iso6bWrite()" ];
 * Ready -> ID [ label="iso6bInventoryRound(),\nafter select command has been emitted" ];
 * ID -> Data_Exchange [ label="iso6bInventoryRound(),\nafter collision arbitration" ];
 * Data_Exchange -> Power_off [ label="as3993AntennaPower(0)" ];
 * }
 * \enddot
 *
 * It can be seen that iso6bRead() and iso6bWrite() can be called without a prior
 * inventory round. Both commands, however, do need the ID of the tag which can
 * only be determined by calling iso6bInventoryRound().
 */

#ifndef ISO6B_H
#define ISO6B_H

#include "as3993_config.h"

#if ISO6B


#include "as3993_public.h"
#include "errno_as3993.h"
/*
******************************************************************************
* GLOBAL DEFINES
******************************************************************************
*/
/* ERROR CODES*/
#define ISO6B_ERR_NONE	        ERR_NONE            /*!< no error occured during, tag has been found */
#define ISO6B_ERR_NOTAG	        ERR_ISO6B_NOTAG     /*!< No response from any tag */
#define ISO6B_ERR_IRQ           ERR_ISO6B           /*!< IRQ error occured */
#define ISO6B_ERR_REG_FIFO      ERR_ISO6B_REG_FIFO  /*!< FIFO error occured */
#define ISO6B_ERR_OTHER	        ERR_ISO6B_OTHER     /*!< Other error occurred */

/*Command Codes*/
#define ISO6B_GROUP_SELECT_EQ     0x00
#define ISO6B_GROUP_SELECT_NE     0x01
#define ISO6B_GROUP_SELECT_GT     0x02
#define ISO6B_GROUP_SELECT_LT     0x03
#define ISO6B_GROUP_UNSELECT_EQ   0x04
#define ISO6B_GROUP_UNSELECT_NE   0x05
#define ISO6B_GROUP_UNSELECT_GT   0x06
#define ISO6B_GROUP_UNSELECT_LT   0x07
#define ISO6B_FAIL                0x08
#define ISO6B_SUCCESS             0x09
#define ISO6B_INITIALIZE          0x0a
#define ISO6B_READ                0x0c
#define ISO6B_WRITE               0x0d
#define ISO6B_RESEND              0x15

/**
 * This function performs an iso6b inventory round and stores up to \a maxtags 
 * tags into the variable \a tags.
 * It performs a select function which compares up to eight consecutive bytes within
 * the tags to a given buffer. If the tag's memory content which starts at address 
 * \a startaddress matches the buffer given by \a filter the tag backscatters his
 * UID. The bitmask \a mask is used to indicate which bytes of the buffer shall
 * be compared with the tag's memory content. If \a mask is zero all tags are
 * selected.
 *
 * \note The function #iso6bOpen() has to be called first.
 *
 * \param[out] tags : an array of Tag structs which will be filled by this function
 * \param[in] maxtags : the size of the array
 * \param[in] mask : mask identifying which bytes have to be compared
 * \param[in] filter : 8 byte long compare buffer
 * \param[in] startaddress : address of the first register to compare within the tag
 *
 * \returns the number of tags found.
 */
extern s8 iso6bInventoryRound (Tag* tags, u8 maxtags, u8 mask, u8* filter, u8 startaddress);

/*!
 *****************************************************************************
 *  \brief  Issue READ command according to ISO18000-6 and stores
 *          the result in a buffer
 *
 *  This function sends the READ command via the AS3993 to the tag with a given
 *  id. The data is read from start address \a startaddr until 8 bytes
 *  have been read. 
 *  The answer is then written to the memory location 
 *  pointed by \a buffer.
 *
 *  \note The function #iso6bOpen() has to be called first.
 *
 *  \param[in]  uid : 8 byte long uid of the tag
 *  \param[in]  startaddr : start address within the tag
 *  \param[out] buffer : pointer to a memory location where the result shall be stored
 *
 *  \return #ISO6B_ERR_REG_FIFO  : Error reading the FIFO
 *  \return #ISO6B_ERR_NOTAG : No response from tag
 *  \return #ISO6B_ERR_IRQ   : Irq Error
 *  \return #ISO6B_ERR_NONE  : No error
 *
 *****************************************************************************
 */
extern s8 iso6bRead(u8* uid, u8 startaddr, u8* buffer);

/*!
 *****************************************************************************
 *  \brief  Issue WRITE command according to ISO18000-6 to write a specified
 *          number of bytes to a memory location within a tag
 *
 *  This function sends the WRITE command via the AS3993 to the
 *  tag with a given \a uid. This command stores the \a data to
 *  the memory location at the \a addr on the tag.
 *
 * \note WARNING: THIS FUNCTION WAS NEVER TESTED !!!
 *  \note The function #iso6bOpen() has to be called first.
 *
 *  \param[in]  uid : 8 byte long uid of the tag
 *  \param[in]  addr : address of the register to write
 *  \param[in]  data : data to write
 *  \param[out] buffer : pointer to a memory location where
 *                    the result acknowledge or error shall be stored
 *
 *  \return #ISO6B_ERR_REG_FIFO  : Error reading the FIFO
 *  \return #ISO6B_ERR_NOTAG : No response from tag
 *  \return #ISO6B_ERR_IRQ   : Irq Error
 *  \return #ISO6B_ERR_NONE  : No error
 *
 *****************************************************************************
 */
extern s8 iso6bWrite(u8* uid, u8 addr, u8 data, u8* buffer);

/*!
 *****************************************************************************
 *  \brief  Close a session
 *
 *  Close the session for iso6b protocol.
 *****************************************************************************
 */
extern void iso6bClose(void);


/*!
 *****************************************************************************
 *  \brief  Open a session
 *
 *  Open a session for iso6b protocol.
 * 
 *  Set iso6b specific parameters.
 *****************************************************************************
 */
extern void iso6bOpen(void);

/*!
 *****************************************************************************
 *  \brief  Initializes the iso6b module
 *
 *  \return #ISO6B_ERR_NONE : No error
 *
 *****************************************************************************
 */
extern s8 iso6bInitialize(void);

#endif /* ISO6B_H */

#endif
