#ifndef SPRINTF_H
#define SPRINTF_H

#define sprintf(stream,fmt,a...) sprintf_pic(stream,fmt,##a)

#include "ams_types.h"
#define INT_MAX_DIGITS      11

/*! 
 *****************************************************************************
 *  \brief  Used as a replacement for <stdio.h> sprintf function
 * allows fast write of %d/%x values to a string
 * 
 ******************************************************************************
 */
s16 sprintf_pic(char * str, const char* format, ...);

#endif
