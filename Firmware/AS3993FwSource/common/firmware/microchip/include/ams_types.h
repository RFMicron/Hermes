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
 *      PROJECT:   ASxxxx firmware
 *      $Revision: $
 *      LANGUAGE:  ANSI C
 */

/*! \file
 *
 *  \author
 *
 *  \brief Basic datatypes
 *
 */

#ifndef AMS_TYPES_H
#define AMS_TYPES_H

#include "GenericTypeDefs.h"

/*!
 * Basic datatypes are mapped to ams datatypes that
 * shall be used in all ams projects.
 */
typedef unsigned char u8;   /*!< represents an unsigned 8bit-wide type */
typedef signed char s8;     /*!< represents a signed 8bit-wide type */
typedef unsigned int u16;   /*!< represents an unsigned 16bit-wide type */
typedef signed int s16;     /*!< represents a signed 16bit-wide type */
typedef unsigned long u32;  /*!< represents an unsigned 32bit-wide type */
typedef unsigned long long u64;/*!< represents an unsigned 64bit-wide type */
typedef signed long long s64;  /*!< represents n signed 64bit-wide type */
typedef signed long s32;      /*!< represents a signed 32bit-wide type */
typedef u16 umword; /*!< USE WITH CARE!!! unsigned machine word:
                        8 bit on 8bit machines, 16 bit on 16 bit machines... */
typedef s16 smword; /*!< USE WITH CARE!!! signed machine word:
                         8 bit on 8bit machines, 16 bit on 16 bit machines... */
typedef unsigned int uint; /*!< type for unsigned integer types,
                            useful as indices for arrays and loop variables */
typedef signed int sint; /*!< type for integer types,
                            useful as indices for arrays and loop variables */

extern u8  invalid_sizeof_u8 [(sizeof(u8 ) == 1)?(1):(-1)];
extern s8  invalid_sizeof_s8 [(sizeof(s8 ) == 1)?(1):(-1)];
extern u16 invalid_sizeof_u16[(sizeof(u16) == 2)?(1):(-1)];
extern s16 invalid_sizeof_s16[(sizeof(s16) == 2)?(1):(-1)];
extern u32 invalid_sizeof_u32[(sizeof(u32) == 4)?(1):(-1)];
extern s32 invalid_sizeof_s32[(sizeof(s32) == 4)?(1):(-1)];
extern u64 invalid_sizeof_u64[(sizeof(u64) == 8)?(1):(-1)];
extern s64 invalid_sizeof_s64[(sizeof(s64) == 8)?(1):(-1)];

#define U8_C(x)     (x) /*!< 
Define a constant of type u8 */
#define S8_C(x)     (x) /*!< 
Define a constant of type s8 */
#define U16_C(x)    (x) /*!< 
Define a constant of type u16 */
#define S16_C(x)    (x) /*!< 
Define a constant of type s16 */
#define U32_C(x)    (x##UL) /*!< 
Define a constant of type u32 */
#define S32_C(x)    (x##L) /*!< 
Define a constant of type s32 */
#define U64_C(x)    (x##ULL) /*!< 
Define a constant of type u64 */
#define S64_C(x)    (x##LL) /*!< 
Define a constant of type s64 */
#define UMWORD_C(x) (x) /*!< 
Define a constant of type umword */
#define MWORD_C(x)  (x) /*!< 
Define a constant of type mword */

#if 0
typedef umword bool_t; /*!<
                            represents a boolean type */

#ifndef TRUE
#define TRUE 1 /*!< 
used for the #bool_t type */
#endif
#ifndef FALSE
#define FALSE 0 /*!< 
used for the #bool_t type */
#endif

#else
typedef BOOL bool_t;
#endif


#ifndef NULL
#define NULL (void*)0 /*!< 
    represents a NULL pointer */
#endif

#endif /* AMS_TYPES_H */

