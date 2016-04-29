#include "sprintf.h"
#include "stdarg.h"
#include "errno.h"

s16 sprintf_pic(char * str, const char* format, ...)
{
    u16 i = 0;
    u16 argint = 0;
    bool_t PercentageFound = FALSE;
    bool_t ZeroFound = FALSE;
    bool_t ZeroFill = FALSE;
    bool_t LeftAlign = FALSE;
    u16 NoOfNonzeroChars, NoOfZeros;
    u16 NoOfChars = 0;
    bool_t upperCase = FALSE;   /*user ABCDEF of abcdef for hex digits  */
    u8 CharSent[5];
    s16 hs = 0;
    u16 nibbles;
    u16 index = 0;

    va_list argptr;
    va_start(argptr, format);

    while (*((u8*)format) != '\0')
    {
        switch (*((u8*)format))
        {
        case '%':
            hs = 0;
            if (PercentageFound == TRUE)
            {
                PercentageFound = FALSE;
                str[index] = '%';
                index++;
            }
            else
            {
                PercentageFound = TRUE;
            }
            break;
        case 'h':
            if (PercentageFound)
            {
                hs++;
            }
            else
            {
                str[index] = *((u8*)format);
                index++;
            }
            break;
        case 's':
        {
            if (PercentageFound == TRUE)
            {
                PercentageFound = FALSE;
                char *temp = (char*)va_arg(argptr,char*);
                u16 length;
                for(length=0;temp[length]!='\0';length++);
                if(LeftAlign == TRUE)
                {
                    for(i=0;i<length;i++,index++)
                        str[index] = temp[i];
                    for(;i<NoOfChars;i++,index++)
                        str[index] = ' ';
                }
                else
                {
                    if(NoOfChars>length)
                    {
                        NoOfChars = NoOfChars-length;
                        for(i=0;i<NoOfChars;i++,index++)
                            str[index] = ' ';
                    }
                    for(i=0;i<length;i++,index++)
                        str[index] = temp[i];
                }
                NoOfChars = 0;
                LeftAlign = FALSE;
                break;
            }
        }
        case 'd':
            if (PercentageFound == TRUE)
            {
                u16 digits = 0;
                char number[INT_MAX_DIGITS+1];
                argint = (s16)va_arg(argptr, s16);
                if(argint&0x8000)
                {
                    str[index] = '-';
                    index++;
                    argint = -argint;
                    if(NoOfChars)
                        NoOfChars--;
                }

                number[INT_MAX_DIGITS-1] = '0';
                number[INT_MAX_DIGITS] = '\0';
                if(!argint)
                    NoOfNonzeroChars = 0;
                else
                    NoOfNonzeroChars = 1;
                while((argint || (digits<NoOfChars)) && (digits<INT_MAX_DIGITS))
                {
                    if(!argint && digits>0 && ZeroFill==FALSE)
                    {
                        digits++;
                        number[INT_MAX_DIGITS-digits] = ' ';
                    }
                    else
                    {
                        digits++;
                        number[INT_MAX_DIGITS-digits] = '0' + (argint % 10);
                    }
                    argint = argint / 10;
                }
                if(!digits)
                    digits=1;
                for(i=0;i<digits;i++)
                    str[index+i] = number[INT_MAX_DIGITS-digits+i];
                index+=digits;
                PercentageFound = FALSE;
                NoOfChars = 0;
            }
            /* if d wasnt preceeded by % it is not a format but a string;so just print it */
            else
            {
                str[index] = *((u8*)format);
                index++;
            }
            break;
        case '-':
            {
                if(PercentageFound == TRUE)
                {
                    LeftAlign = TRUE;
                    break;
                }
            }
        case 'c':
            {
                if (PercentageFound == TRUE)
                {
                    PercentageFound = FALSE;
                    str[index] = (char)va_arg(argptr,s16);
                    index++;
                    break;
                }
            }
        case 'l':
            {
                if(PercentageFound == TRUE)
                    break;
            }
        case 'X':
            upperCase = TRUE;
            /* fall through                                               */
        case 'D':
        case 'U':
        case 'u':
        case 'x':

            /* Initialise the counters to 0 */
            NoOfNonzeroChars = 0;
            NoOfZeros = 0;

            /* if d/x was preceeded by % it represents some format */
            if (PercentageFound == TRUE)
            {
                nibbles = 4;        // 4 bit = 1 nibble
                if (hs == 2)        // (unsigned) short short integer = char
                {
                    nibbles = 2;
                    argint = (u16)va_arg(argptr, u16);
                }
                else if (hs == 1)   // (unsigned) short integer = 16 bit
                {
                    argint = (u16)va_arg(argptr, u16);
                }
                else                // (unsigned) integer = usually 32/64 depending on OS, here 16 bit
                {
                    argint = (s16)va_arg(argptr, s16);
                }

                /* shift and take nibble by nibble from MSB to LSB */
                for (i = nibbles; i > 0; i--)
                {
                    CharSent[i-1]=(u8)((argint & ((0xf)<<(4*(nibbles-1)))) >> (4*(nibbles-1)));

                    if ((CharSent[i-1] >= 0x0a) && (CharSent[i-1] <= 0x0f))
                    {
                        /*if the nibble is greater than 9 */
                        CharSent[i-1] = (u8)((u16)CharSent[i-1]+(upperCase ? 55 : 87));
                    }
                    else
                    {
                        /*if nibble lies from 0 to 9*/
                        CharSent[i-1]=(u8)((u16)CharSent[i-1]+48);
                    }
                    /* see how many nibbles the number is actually filling */
                    if (CharSent[i-1] == '0')
                    {
                        if (NoOfNonzeroChars == 0)
                        {
                            NoOfZeros++;
                        }
                        else
                        {
                            NoOfNonzeroChars++;
                        }
                    }
                    else
                    {
                        NoOfNonzeroChars++;
                    }
                    argint = argint << 4;
                }

                CharSent[nibbles] = '\0' ;

                PercentageFound = FALSE;

                /* If the user hasnt specified any number along with format display by default
                word size */
                if (NoOfChars == 0)
                {
                    //NoOfChars = nibbles;
                    if(NoOfNonzeroChars == 0)
                        NoOfChars = 1;
                    else
                        NoOfChars = NoOfNonzeroChars;
                }

                /* print the number displaying as many nibbles as the user has given in print format */
                if (NoOfChars >= NoOfNonzeroChars)
                {
                    for (i = 0; i < (NoOfChars - NoOfNonzeroChars); i++,index++)
                    {
                        if((ZeroFill == TRUE)||(!NoOfNonzeroChars && i==(NoOfChars-1)))
                            str[index] = '0';
                        else
                            str[index] = ' ';
                    }

                    for (i = NoOfNonzeroChars; i > 0; i--,index++)
                    {
                        str[index] = CharSent[i-1];
                    }
                }
                else
                {
                    for (i = NoOfNonzeroChars; i > 0; i--,index++)
                    {
                        str[index] = CharSent[i-1];
                    }
                }
            }

            /* if d/x wasnt preceeded by % it is not a format but a string;so just print it */
            else
            {
                str[index] = *((u8*)format);
                index++;
            }
            /*clean the number of s8 for next cycle                     */
            NoOfChars = 0;

            ZeroFill = FALSE;
            /*use lower case by default                                 */
            upperCase = FALSE;
            break;
        case '0':
            /*if 0 was preceeded by % it represents some format */
            if (PercentageFound == TRUE)
            {
                ZeroFound=TRUE;
            }
            /* if 0 wasnt preceeded by % it is not a format but a string;so just print it */
            else
            {
                str[index] = *((u8*)format);
                index++;
            }
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            /* if the number was preceeded by % and then a 0 it represents some format */
            if (ZeroFound == TRUE)
            {
                NoOfChars = (u16) (*((u8*)format) - 48);
                ZeroFill  = TRUE;
                ZeroFound = FALSE;
            }
            else if (PercentageFound == TRUE)
            {
                NoOfChars = (u16) (*((u8*)format) - 48);
            }

            /* if the number wasnt preceeded by % it is not a format but a string;so just print it */
            else
            {
                str[index] = *((u8*)format);
            }
            break;
            /*if the character doesnt represent any of these formats just print it*/
        default:
            str[index] = *((u8*)format);
            index++;
            break;
        }
        format = ((char*)format) + 1;
    }
    va_end(argptr);
    str[index]='\0';

    return ERR_NONE;
}
