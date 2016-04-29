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
  * @brief This file includes some useful functions.
  *
  * @author Ulrich Herrmann
  */

#include "as3993_config.h"
#include "global.h"
#include <p24Fxxxx.h>

/*------------------------------------------------------------------------- */
/** This function is used to convert a 8bit value to a Hexstring. \n
  * @param value Databyte which should be converted
  * @param *destbuf Destination buffer. The Buffer should be at least 5 bytes long.
                 The first byte is always a 0 and the second a x.
  */
void bin2Hex(char value, u8 *destbuf)
{
    u8 temp_v;
    destbuf[0] = '0';
    destbuf[1] = 'x';

    temp_v = (value >> 4) & 0x0F;
    if (temp_v  <= 9)
    {
        destbuf[2] = temp_v + '0';
    }
    else
    {
        temp_v -= 10;
        destbuf[2] = temp_v + 'A';
    }

    temp_v = value & 0x0F;
    if (temp_v  <= 9)
    {
        destbuf[3] = temp_v + '0';
    }
    else
    {
        temp_v -= 10;
        destbuf[3] = temp_v + 'A';
    }
    destbuf[4] = 0x00;

}

/*------------------------------------------------------------------------- */
/** This function is used to convert a 8bit value to a Char string. \n
  * @param value Databyte which should be converted
  * @param *destbuf Destination buffer. The Buffer should be at least 6 bytes long.
  */
void bin2Chars(int value, u8 *destbuf)
{
    u8 tenthousand=0;
    u8 thousand=0;
    u8 hundred=0;
    u8 ten=0;
    u8 one=0;

    while (value-10000 >= 0)
    {
        value -= 10000;
        tenthousand++;
    }
    while (value-1000 >= 0)
    {
        value -= 1000;
        thousand++;
    }
    while (value-100 >= 0)
    {
        value -= 100;
        hundred++;
    }
    while (value-10 >= 0)
    {
        value -= 10;
        ten++;
    }
    while (value-1 >= 0)
    {
        value--;
        one++;
    }

    if (tenthousand)
    {
        destbuf[0]=tenthousand + '0';
        destbuf[1]=thousand + '0';
        destbuf[2]=hundred + '0';
        destbuf[3]=ten + '0';
        destbuf[4]=one + '0';
        destbuf[5]=0x00;
    }
    else if (thousand)
    {
        destbuf[0]=thousand + '0';
        destbuf[1]=hundred + '0';
        destbuf[2]=ten + '0';
        destbuf[3]=one + '0';
        destbuf[4]=0x00;
    }
    else if (hundred)
    {
        destbuf[0]=hundred + '0';
        destbuf[1]=ten + '0';
        destbuf[2]=one + '0';
        destbuf[3]=0x00;
    }
    else if (ten)
    {
        destbuf[0]=ten + '0';
        destbuf[1]=one + '0';
        destbuf[2]=0x00;
    }
    else if (one)
    {
        destbuf[0]=one + '0';
        destbuf[1]=0x00;
    }
    else
    {
        destbuf[0]='0';
        destbuf[1]=0x00;
    }
}

/**
 * Converts u32 number into EBV format. The EBV is stored in parameter ebv.
 * Parameter len will be set to the length of data in ebv.
 * @param value u32 value to convert into EBV
 * @param ebv array to store the EBV
 * @param len number of data in ebv
 */
void u32ToEbv(u32 value, u8 *ebv, u8 *len)
{
    u8 lsbytefirst[6];  //additional byte for setting extension bit in loop
    u8 *buf = &lsbytefirst[0];
    int i;
    
    *len = 0;
    *buf = 0;
    do
    {
        (*buf) |= (u8)(value & 0x7F);
        value = value >> 7;
        buf++;
        (*len)++;
        *buf = 0x80;   //set extension bit in next block
    }
    while (value > 0);
    //the EBV in buf starts with LSByte -> reorder the content into ebv array
    for (i=0; i<(*len); i++)
    {
        buf--;
        ebv[i] = *buf;
    }
}

/**
 * Inserts an array of u8 values into an u8 buffer at a specific bitpos
 * @param dest Destination buffer
 * @param source Source buffer
 * @param len Number of bytes in source buffer
 * @param bitpos bitposition (1-8) on destination buffer where source values should be put.
 */
void insertBitStream(u8 *dest, u8 const *source, u8 len, u8 bitpos)
{
    int i;
    u8 mask0 = (1<<bitpos)-1;
    u8 mask1 = (1<<(8-bitpos))-1;

    for (i=0; i<len; i++)
    {
        dest[i+0] &= (~mask0);
        dest[i+0] |= ((source[i]>>(8-bitpos)) & mask0);
        dest[i+1] &= ((~mask1) << bitpos);
        dest[i+1] |= ((source[i] & mask1) << bitpos);
    }
}

u32 readU32FromLittleEndianBuffer(u8 const *buffer)
{
    return (u32)(buffer[0] | ((u32)buffer[1] << 8) |
            ((u32)buffer[2] << 16) | ((u32)buffer[3] << 24));
}
