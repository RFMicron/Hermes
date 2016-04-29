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
 *  \author U.Herrmann (based on work by E.Grubmueller)
 *  \author T. Luecker (Substitute)
 *  \author Bernhard Breinbauer
 *  \author Rene Eggerstorfer
 *
 *   \brief Functions provided by the as3993 series chips
 *
 *   Functions provided by the as3993 series chips. All higher level
 *   and protocol work is contained in gen2.c and iso6b.c
 */
#include "as3993_config.h"
#include "platform.h"
#include "as3993.h"
#include "as3993_public.h"
#include "global.h"
#include "logger.h"
#include "timer.h"
#include "gen2.h"
#include "stdlib.h"
#include "string.h"
#include "Compiler.h"
#if VERBOSE_INIT
#include <math.h>
#endif

/** Definition protocol read bit */
#define READ                    0x40

/** time in ms to wait for AS3980 to be ready */
#define AS3980WAITTIME          510

/** Wait for AS3980 to be ready. */
#if RUN_ON_AS3980 
#define WaitForAS3980() {\
        delay_ms(AS3980WAITTIME); \
    }
#else
#define WaitForAS3980() {}
#endif

/** This variable is used as flag to signal an data reception.
  *  It is a bit mask of the RESP_TXIRQ, RESP_RXIRQ, RESP_* values
  */
volatile u16 as3993Response = 0;

/** temporary buffer for as3993SaveSensitivity() and as3993RestoreSensitivity() */
static u8 as3993SavedSensRegs[1];

/** Restore registers 0x00 to 0x1D + 5 registers (0x22, 0x29, 0x35, 0x36 and 0x3A)
 *  after power up */
static u8 as3993PowerDownRegs[AS3993_REG_ICD+6];

/** Will be set to 0 if version register is > 0x60. Silicon revision 0x60 needs
 specific handling in some functions. */
static u8 gChipRevisionZero = 1;

#if VERBOSE_INIT
static void as3993PrintRfpLine(void)
{
    s16 val;
    s16 ch_val_i;
    s16 ch_val_q;
    s16 rf;

    val = as3993GetReflectedPower( );
    ch_val_i = ((s8)(val&0xff));
    ch_val_q = ((s8)((val>>8)&0xff));
    rf = sqrt(ch_val_i * ch_val_i + ch_val_q * ch_val_q);
    LOG("rf=%hx ",rf);

    LOG("%hx", val);

    for (val = -32; val <= 32 ; val ++)
    {
        if ( val == rf )
        {
            LOG("A");
            continue;
        }
        if ( val == ch_val_i && val == ch_val_q)
        {
            LOG("X");
            continue;
        }
        if ( val == ch_val_i )
        {
            LOG("I");
            continue;
        }
        if ( val == ch_val_q )
        {
            LOG("Q");
            continue;
        }
        if ( val == 0 )
        {
            LOG("0");
            continue;
        }
        LOG(" ");
    }
    LOG("\n");
}
#endif

/*------------------------------------------------------------------------- */
u16 as3993Initialize(u32 baseFreq)
{
    u8 myBuf[4];

    as3993ResetDoNotPreserveRegisters();

#if AS3993DEBUG
    LOG(CHIP" Init\n");
    myBuf[0] = AS3993_REG_DEVICEVERSION;
    LOG("version reg %hhx->",myBuf[0]);
    myBuf[0] = as3993SingleRead(myBuf[0]);
    LOG("%hhx\n",myBuf[0]);
#endif
#if AS3993_DO_SELFTEST
    // check SPI communication
    myBuf[0] = 0x55;
    myBuf[1] = 0xAA;
    myBuf[2] = 0xFF;
    myBuf[3] = 0x00;
    as3993ContinuousWrite(AS3993_REG_MODULATORCONTROL1, myBuf, 4);
    memset(myBuf, 0x33, sizeof(myBuf));
    as3993ContinuousRead(AS3993_REG_MODULATORCONTROL1, 4,myBuf);
    if ((myBuf[0]!=0x55) || 
        (myBuf[1]!=0xAA) || 
        (myBuf[2]!=0xFF) ||
        (myBuf[3]!=0x00))
    {
        LOG("%hhx %hhx %hhx %hhx\n", myBuf[0], myBuf[1], myBuf[2], myBuf[3]);
        return 1; // data bus interface pins not working
    }

    // check EN pin + SPI communication
    as3993ResetDoNotPreserveRegisters();
    as3993ContinuousRead(AS3993_REG_MODULATORCONTROL1, 4, myBuf);
    if ((myBuf[0]==0x55) || 
        (myBuf[1]==0xAA) || 
        (myBuf[2]==0xFF) ||
        (myBuf[3]==0x00))
    {
        LOG("EN pin failed\n");
        return 2; /* enable pin not working */
    }

    // check IRQ line
    delay_ms(1);
    WaitForAS3980();
    as3993SingleWrite(AS3993_REG_IRQMASK1, 0x20);
    // set up 48Byte transmission, but we supply less, therefore a fifo underflow IRQ is produced
    as3993SingleWrite(AS3993_REG_TXLENGTHUP, 0x03);
    as3993SingleCommand(AS3993_CMD_TRANSMCRC);
    as3993ContinuousWrite(AS3993_REG_FIFO,myBuf,4);
    as3993ContinuousWrite(AS3993_REG_FIFO,myBuf,4);
    as3993ContinuousWrite(AS3993_REG_FIFO,myBuf,4);
    as3993ContinuousWrite(AS3993_REG_FIFO,myBuf,4);
    as3993ContinuousWrite(AS3993_REG_FIFO,myBuf,4);
    as3993ContinuousWrite(AS3993_REG_FIFO,myBuf,4);

    as3993WaitForResponse(RESP_FIFO);
    if ( !(as3993GetResponse() & RESP_FIFO) )
    {
        return 3;
    }
//    as3993SingleWrite(AS3993_REG_IRQMASK1, 0x00);
//    as3993SingleWrite(AS3993_REG_IRQMASK2, 0x40);
//    as3993SingleCommand(AS3993_CMD_TRIGGERADCCON);
//    as3993WaitForResponse(RESP_END_CMD);
//    if ( !(as3993GetResponse() & RESP_END_CMD) )
//        return 3; /* Interrupt pin not working */

    as3993ClrResponse();

    as3993ResetDoNotPreserveRegisters();
    as3993SingleCommand(AS3993_CMD_HOP_TO_MAIN_FREQUENCY);
#endif

    /* chip status control 0x00 */
    /*STBY - - - - AGC REC RF */
#if RADON || FEMTO2  || FEMTO2_1
    /* 0   0 0 0 0  1   1  0  = 0x06 */
    as3993SingleWrite(AS3993_REG_STATUSCTRL, 0x06);
#else
    /* 0   0 0 0 0  0   1  0  = 0x02 */
    as3993SingleWrite(AS3993_REG_STATUSCTRL, 0x02);
#endif

    /*protocl control register 0x01 */
    /*RX_CRC_N DIR_MODE AutoACK1 AutoACK2 - PROT1 PROT0  */
    /*   0         0       0         0    0   0     0 = 0x00 */
    as3993SingleWrite(AS3993_REG_PROTOCOLCTRL, 0x00);

    /* TX options 0x02 */
    /* - - TxOne1 TxOne0 - Tari2 Tari1 Tari0 */

#if FEMTO2 || FETMO2_1
     /* 0 0   1      1    0   0     1     0   = 0x30 */
    as3993SingleWrite(AS3993_REG_TXOPTIONS, 0x32);
#else
    /* 0 0   1      1    0   0     0     0   = 0x30 */
    as3993SingleWrite(AS3993_REG_TXOPTIONS, 0x30);
#endif
    /* RX options register is set by gen2 configuration */
    /* TRcal high + low register is set by gen2 configuration */

    /* PLL Main REgister 0x17*/
    /* use 100kHz as pll reference as we use ETSI as default */
    as3993SingleWrite(AS3993_REG_PLLMAIN1, 0x54);

#if AS3993DEBUG
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, 0x80);     // enable debug signals on OAD
#else
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, 0x00);
#endif

    /*MISC1 0x0D */
    /*hs_output hs_oad miso_pd2 miso_pd1 open_dr s_mix iadd_sink2 iadd_sink1 */
    /*     1       0       0        0        0     0       0          0 = 0xC0 */
    as3993SingleWrite(AS3993_REG_MISC1, 0x00
#if !RADON
            | 0x80
#endif
#if AS3993DEBUG
            | 0x40
#endif
#ifdef SINGLEINP
            | 0x04
#endif
            );

#ifdef SINGLEINP
#if VERBOSE_INIT
    LOG("SINLGEINP\n");
#endif
#endif

#ifdef BALANCEDINP
#if VERBOSE_INIT
    LOG("BALANCED INP\n");
#endif
#endif

#ifdef INTPA
    /*REGULATOR and PA Bias 0x0B */
    /*pa_bias1-0 rvs_rf2-1 rvs2-0  */
    /*   1 0       0 1 1    0 1 1   = 0x1B */
    as3993SingleWrite(AS3993_REG_REGULATORCONTROL, 0xBF);

    /*RF Output and LO Control Register 0x0C */
    /*eTX7 - eTX0  */
    /*  0 1 1 0 1 0 0 0   = 0x68 */
    /* int PA, PA 14mA */
    as3993SingleWrite(AS3993_REG_RFOUTPUTCONTROL, 0x68);

    /* Modulator Control Register 1 0x13 */
    /* - MAIN_MOD AUXMOD tpreset use_corr ELFP ASKRATE1 ASKRATE0 */
    /* 0    1       0       0       0       0      0        0      =0x40 */
    myBuf[0] = 0x40;
    /* Modulator Control Register 2 0x14 */
    /*ook_ask PR_ASK MOD_DEP5-MOD_DEP0 */
    /*    1      1     0 1     1 1 0 1   = 0xDD */
    myBuf[1] = 0xDD;
    /* Modulator Control Register 3 0x15 */
    /*TRFON1 TRFON0 lin_mode TXLEVEL4-TXLEVEL0 */
    /*   0     0       0      0       0 1 1 1  =0x07 */
    myBuf[2] = 0x01;
    as3993ContinuousWrite(AS3993_REG_MODULATORCONTROL1, myBuf, 3);
#if VERBOSE_INIT
    LOG("INTPA\n");
#endif
#endif

#ifdef EXTPA
    /*REGULATOR and PA Bias 0x0B */
    /*pa_bias1-0 rvs_rf2-1 rvs2-0  */
    /*   0 0       0 1 1    0 1 1   = 0x1B */
    as3993SingleWrite(AS3993_REG_REGULATORCONTROL, 0x1B);

    /*RF Output and LO Control Register 0x0C */
    /*eTX7 - eTX0  */
    /*  0 0 0 0 0 0 1 0   = 0x58 */
    /* int pa, PA 14mA */
    as3993SingleWrite(AS3993_REG_RFOUTPUTCONTROL, 0x22);

    /*Modulator Control Register 1 0x13 */
    /* - MAIN_MOD AUXMOD tpreset use_corr ELFP ASKRATE1 ASKRATE0 */
    /* 0    0       1       0       0       0      0        0      =0x20 */
    myBuf[0] = 0x20;
    /*Modulator Control Register 2 0x14 */
#if RADON
    /*ook_ask PR_ASK MOD_DEP5-MOD_DEP0 */
    /*    1      0     0 1     1 1 0 1   = 0xDD */
    myBuf[1] = 0x9D;
#else
    /*ook_ask PR_ASK MOD_DEP5-MOD_DEP0 */
    /*    1      1     0 1     1 1 0 1   = 0xDD */
    myBuf[1] = 0xDD;
#endif
    /*Modulator Control Register 3 0x15 */
    /*TRFON1 TRFON0 lin_mode TXLEVEL4-TXLEVEL0 */
#if RADON
    /*   0     0       0      0       0 1 1 0  =0x06 */
    myBuf[2] = 0x06;
#else
    /*   0     0       0      0       0 0 0 1  =0x07 */
    myBuf[2] = 0x01;
#endif
    as3993ContinuousWrite(AS3993_REG_MODULATORCONTROL1, myBuf, 3);
#if VERBOSE_INIT
    LOG("EXTPA\n");
#endif
#endif

    /*RF Output and LO Control Register 0x0C */
    /*LF_R3<2-1> LF_C3<5-3> cp<2-0>  */
    /*    0 0       1 1 0    1 0 1   = 0x25 */
    /* 30kOhm, 160pF, 1500uA */
    as3993SingleWrite(AS3993_REG_CPCONTROL, 0x35);

#ifdef INTVCO
#if FEMTO2 || FEMTO2_1 || RADON
    as3993SingleWrite(AS3993_REG_MISC2, 0x40);      /* external TCXO AC, no clsys */
#else
    as3993SingleWrite(AS3993_REG_MISC2, 0x00);      // no clsys
#endif

#if VERBOSE_INIT
    LOG("INTVCO\n");
#endif
#endif

    /*Enable Interrupt Register Register 1 0x35 */
    /*e_irq_TX e_irq_RX e_irq_fifo e_irq_err e_irq_header RFU e_irq_AutoAck e_irq_noresp  */
    /*    1        1          1         1          1       1        1             1     = 0xFF */
    as3993SingleWrite(AS3993_REG_IRQMASK1, 0xFF);
    /*Enable Interrupt Register Register 2 0x35 */
    /*e_irq_ana e_irq_cmd RFU RFU RFU e_irq_err1 e_irq_err2 e_irq_err3 */
    /*    0         0      0   0   0      1          1          1     = 0x07 */
    as3993SingleWrite(AS3993_REG_IRQMASK2, 0x07);
    
    /*RX Length Register 1 0x3A */
    /*RX_crc_n2 fifo_dir_irq2 rep_irg2 auto_errcode_RXl RXl11-RXl8 */
    /*    0           0           0            1         0 0 0 0  = 0x10 */
    as3993SingleWrite(AS3993_REG_RXLENGTHUP, 0x10);

    /* Give base freq. a reasonable value */
    as3993SetBaseFrequency(AS3993_REG_PLLMAIN1, baseFreq);
    as3993SetSensitivity(-AS3993_NOM_SENSITIVITY);

#if AS3993_DO_SELFTEST
    /* Now that the chip is configured with correct ref frequency the PLL 
       should lock */
    delay_ms(20);
    myBuf[0] = as3993SingleRead(AS3993_REG_AGCANDSTATUS);
    if (!(myBuf[0] & 0x03))
    {
        return 4; /* Crystal not stable */
    }
    if (!(myBuf[0] & 0x02)) 
    { 
       return 5; /* PLL not locked */
    }

#if VERBOSE_INIT
    {
        /* Do this now, after everyting else should be configured */
        u32 i = 100;
        while(i--){
            as3993PrintRfpLine();
        }
        for( i = 860000UL; i< 960000UL ;i+=1000UL)
        {
            LOG("%hx%hx ",(u16)(i>>16),(u16)(i&0xffff));
            as3993SetBaseFrequency(AS3993_REG_PLLMAIN1, i);
            as3993PrintRfpLine();
        }
    }
#endif
#endif
    
    return 0;
}

/*------------------------------------------------------------------------- */
/** External Interrupt Function
  * The AS3993 uses the external interrupt to signal the uC that
  * something happened. The interrupt function reads the AS3993
  * IRQ status registers.
  * The interrupt function sets the flags if an event occours.
  */
#if RUN_ON_AS3993 || RUN_ON_AS3980 || RUN_ON_AS3994
void INTERRUPT as3993Isr(void)
{
    u8 regs[2];
    static u8 addr = READ | AS3993_REG_IRQSTATUS1;
    if (gChipRevisionZero)
        delay_us(30);

    do{
        CLREXTIRQ();
    writeReadAS3993Isr(&addr, 1, regs, 2);
#ifdef RFDATARECEIVELED
    if (AS3993_IRQ1_RX == (regs[0] & (AS3993_IRQ1_RX | AS3993_IRQ1_RXERR))) RFDATARECEIVELED(LEDON); 
    else RFDATARECEIVELED(LEDOFF); 
#endif
    as3993Response |= (regs[0] | (regs[1] << 8));
    }while(AS3993_PORT_INT);

    //LOG("isr: %hx\n", as3993Response);
}
#endif

/*------------------------------------------------------------------------- */
u8 as3993ReadChipVersion(void)
{
    u8 version;
    version = as3993SingleRead(AS3993_REG_DEVICEVERSION);
    if (version > 0x60)
        gChipRevisionZero = 0;
    return version;
}

/*------------------------------------------------------------------------- */
void as3993SingleCommand(u8 command)
{
    DISEXTIRQ();
    writeReadAS3993( &command, 1, 0 , 0 , STOP_SGL, 1);
    ENEXTIRQ();
}
/*------------------------------------------------------------------------- */
void as3993SingleWriteNoStop(u8 address, u8 value)
{
    u8 buf[2];
    buf[0] = address;
    buf[1] = value;
    writeReadAS3993( buf, 2, 0 , 0 , STOP_NONE, 1);
}

/*------------------------------------------------------------------------- */
void as3993ContinuousRead(u8 address, s8 len, u8 *readbuf)
{
    DISEXTIRQ();
    address |= READ;
    writeReadAS3993( &address, 1, readbuf , len , STOP_CONT, 1);
    ENEXTIRQ();
}

/*------------------------------------------------------------------------- */
void as3993FifoRead(s8 len, u8 *readbuf)
{
    static u8 address = AS3993_REG_FIFO | READ ;
    DISEXTIRQ();
    writeReadAS3993( &address, 1, readbuf , len , STOP_CONT, 1);
    ENEXTIRQ();
}

/* Function is called from interrupt and normal level, therefore this 
   function must be forced to be reentrant on Keil compiler. */
u8 as3993SingleRead(u8 address)
{
    u8 readdata;

    DISEXTIRQ();
    if (address >= 0x23 && address <= 0x26) /* Test reg access */
    {
        u8 cmd = 0xb0;
        writeReadAS3993( &cmd, 1, 0 , 0 , STOP_NONE, 1);
    }
    address |= READ;
    writeReadAS3993( &address, 1, &readdata , 1 , STOP_SGL, 1);

    ENEXTIRQ();
    return(readdata);
}

void as3993ContinuousWrite(u8 address, u8 *buf, s8 len)
{
    DISEXTIRQ();
    writeReadAS3993( &address, 1, 0 , 0 , STOP_NONE, 1);
    writeReadAS3993( buf, len, 0 , 0 , STOP_CONT, 0);
    ENEXTIRQ();
}

void as3993SingleWrite(u8 address, u8 value)
{
    u8 buf[2];
    buf[0] = address;
    buf[1] = value;
    DISEXTIRQ();
    if (address >= 0x23 && address <= 0x26) /* Test reg access */
    {
        u8 cmd = 0xb0;
        writeReadAS3993( &cmd, 1, 0 , 0 , STOP_NONE, 1);
    }
    writeReadAS3993( buf, 2, 0 , 0 , STOP_SGL, 1);
    ENEXTIRQ();
}

/*------------------------------------------------------------------------- */
void as3993CommandContinuousAddress(u8 *command, u8 com_len,
                             u8 address, u8 *buf, u8 buf_len)
{
    DISEXTIRQ();
    writeReadAS3993( command, com_len, 0 , 0 , STOP_NONE, 1);
    writeReadAS3993( &address, 1, 0 , 0 , STOP_NONE, 0);
    writeReadAS3993( buf, buf_len, 0 , 0 , STOP_CONT, 0);
    ENEXTIRQ();
}

void as3993WaitForResponseTimed(u16 waitMask, u16 counter)
{
    while (((as3993Response & waitMask) == 0) && (counter))
    {
        delay_ms(1);
        counter--;
    }
    if (counter==0)
    {
#if !USE_UART_STREAM_DRIVER
        LOG("TI O T %x %x\n", as3993Response, waitMask);
#endif
        as3993Reset();
        as3993Response = RESP_NORESINTERRUPT;
    }

}

void as3993WaitForResponse(u16 waitMask)
{
    u16 counter;
    counter=0;
    while (((as3993Response & waitMask) == 0) && (counter < WAITFORRESPONSECOUNT))
    {
        counter++;
        delay_us(WAITFORRESPONSEDELAY);
    }
    if (counter >= WAITFORRESPONSECOUNT)
    {
#if !USE_UART_STREAM_DRIVER
        LOG("TI O response: %x, mask: %x\n", as3993Response, waitMask);
#endif
        as3993Reset();
        as3993Response = RESP_NORESINTERRUPT;
    }
//    else
//    	LOG("TI ok response: %x, mask: %x\n", as3993Response, waitMask);
}

void as3993EnterDirectMode()
{
    DISEXTIRQ();
    as3993ClrResponse();

    as3993SingleCommand(AS3993_CMD_DIRECT_MODE);

    setPortDirect();
}

void as3993ExitDirectMode()
{
    setPortNormal();
    
    as3993ClrResponse();
    ENEXTIRQ();
}

void as3993SelectLinkFrequency(u8 a)
{
    u8 reg;

    reg = as3993SingleRead(AS3993_REG_RXOPTIONS);

    reg &= ~0xf0;
    reg |= (a<<4);
    as3993SingleWrite(AS3993_REG_RXOPTIONS, reg);
}

static void as3993LockPLL(void)
{
    u8 buf;
    u8 var;
    u16 i;

    u8 vco_voltage;
    buf = as3993SingleRead(AS3993_REG_STATUSPAGE);
    buf &= ~0x30;
    buf |= 0x10; /* have vco_ri in aglstatus */
    as3993SingleWrite(AS3993_REG_STATUSPAGE, buf);

    buf = as3993SingleRead(AS3993_REG_VCOCONTROL);
    buf |= 0x80; /* set mvco bit */
    as3993SingleWrite(AS3993_REG_VCOCONTROL, buf);
    delay_ms(1); /* give PLL some settling time, should be around 500us */

    vco_voltage = as3993SingleRead(AS3993_REG_AGL) & 0x07;

    buf &= ~0x80; /* reset mvco bit */
    as3993SingleWrite(AS3993_REG_VCOCONTROL, buf);

    if ( vco_voltage <= 1 || vco_voltage >= 6 )
    {
        i=0;
        do
        {
            i++;
            as3993SingleCommand(AS3993_CMD_VCO_AUTO_RANGE);
            delay_ms(10);  /* Please keep in mind, that the Auto Bit procedure will take app. 6 ms whereby the locktime of PLL is just 400us */
            var=as3993SingleRead(AS3993_REG_AGCANDSTATUS);
        } while ( (var & 0x02)==0 && (i<3));/* wait for PLL to be locked and give a few attempts */
    }
}

/*------------------------------------------------------------------------- */
void as3993SetBaseFrequency(u8 regs, u32 frequency)
{
    u8 buf[3];
    u8 statusreg;
    u16 ref=0, i, j, x, reg_A,reg_B;
    u32 divisor;

    statusreg = as3993SingleRead(AS3993_REG_STATUSCTRL);
    as3993SingleWrite(AS3993_REG_STATUSCTRL, statusreg & 0xfe);
    if (regs == AS3993_REG_PLLMAIN1)
    {
        as3993ContinuousRead(AS3993_REG_PLLMAIN1, 3, buf);
        switch (buf[0]& 0x70)
        {

        case 0x40: {
            ref=125;
        } break;
        case 0x50: {
            ref=100;
        } break;
        case 0x60: {
            ref=50;
        } break;
        case 0x70: {
            ref=25;
        } break;
        default: {
            ref=0;
        }
        }
    }

    if (ref == 0)
    {
        return;
    }

    divisor=frequency/ref;

    i = 0x3FFF & (divisor >> 6); /* division by 64 */
    x = (i<<6)+ i;
    if (divisor > x)
    {
        x += 65;
        i++;
    }
    x -= divisor;
    j = i;
    do
    {
        if (x >= 33)
        {
            i--;
            x -= 33;
        }
        if (x >= 32)
        {
            j--;
            x -= 32;
        }
    } while (x >= 32);
    if (x > 16) 
    {            /* subtract 32 from x if it is larger than 16 */
        x -= 32; /* this yields more closely matched A and B values */
        j--;
    }

    reg_A = i - x;
    reg_B = j + x;
    if (regs==AS3993_REG_PLLMAIN1)
    {
        buf[0] = (buf[0] & 0xF0) | ((u8)((reg_B >> 6) & 0x0F));
        buf[1] = (u8)((reg_B << 2) & 0xFC) |  (u8)((reg_A >> 8) & 0x03);
        buf[2] = (u8)reg_A;

        as3993ContinuousWrite(AS3993_REG_PLLMAIN1, buf, 3);
    }
    as3993LockPLL();
    as3993SingleWrite(AS3993_REG_STATUSCTRL, statusreg);
}

void as3993EnterPowerDownMode()
{
    u8 i;
    int count;
    
    if (!ENABLE) return;

    DISEXTIRQ();
    CLREXTIRQ();
    /* Switch off antenna */
    as3993PowerDownRegs[0] = as3993SingleRead(AS3993_REG_STATUSCTRL);
    as3993SingleWrite(0, as3993PowerDownRegs[0] & (~0x03));
    for (i=1; i<AS3993_REG_ICD; i++)
    {
        if (i!=0x0F)
            as3993PowerDownRegs[i] = as3993SingleRead(i);
    }
    as3993PowerDownRegs[AS3993_REG_ICD+0] = as3993SingleRead(AS3993_REG_MIXOPTS);
    as3993PowerDownRegs[AS3993_REG_ICD+1] = as3993SingleRead(AS3993_REG_STATUSPAGE);
    as3993PowerDownRegs[AS3993_REG_ICD+2] = as3993SingleRead(AS3993_REG_IRQMASK1);
    as3993PowerDownRegs[AS3993_REG_ICD+3] = as3993SingleRead(AS3993_REG_IRQMASK2);
    as3993PowerDownRegs[AS3993_REG_ICD+4] = as3993SingleRead(AS3993_REG_TXSETTING);
    as3993PowerDownRegs[AS3993_REG_ICD+5] = as3993SingleRead(AS3993_REG_RXLENGTHUP);
    /* Wait for antenna being switched off */
    count = 500;
    while(count-- && (as3993SingleRead(AS3993_REG_AGCANDSTATUS) & 0x04))
    {
        delay_ms(1);
    }
    EN(LOW);
}

void as3993ExitPowerDownMode()
{
    u8 i;
    u8 buf[2];
    
    if (ENABLE) return;

    EN(HIGH);
    delay_us(10);
    as3993WaitForStartup();

    /* Do not switch on antenna before PLL is locked.*/
    as3993SingleWrite(0, as3993PowerDownRegs[0] & (~0x03));
    for (i=1; i<AS3993_REG_ICD; i++)
    {
        if (i!=0x0F)
            as3993SingleWrite(i, as3993PowerDownRegs[i]);
    }
    as3993SingleWrite(AS3993_REG_MIXOPTS,    as3993PowerDownRegs[AS3993_REG_ICD+0]);
    as3993SingleWrite(AS3993_REG_STATUSPAGE, as3993PowerDownRegs[AS3993_REG_ICD+1]);
    as3993SingleWrite(AS3993_REG_IRQMASK1,   as3993PowerDownRegs[AS3993_REG_ICD+2]);
    as3993SingleWrite(AS3993_REG_IRQMASK2,   as3993PowerDownRegs[AS3993_REG_ICD+3]);
    as3993SingleWrite(AS3993_REG_TXSETTING,  as3993PowerDownRegs[AS3993_REG_ICD+4]);
    as3993SingleWrite(AS3993_REG_RXLENGTHUP, as3993PowerDownRegs[AS3993_REG_ICD+5] & 0xF0);
    delay_us(300);
    as3993LockPLL();
    as3993SingleWrite(AS3993_REG_STATUSCTRL, as3993PowerDownRegs[0]);

    buf[0] = as3993SingleRead(AS3993_REG_AGCANDSTATUS);
    if (!(buf[0] & 0x03))
    {
        LOG("/******** Crystal not stable */\n");
    }
    if (!(buf[0] & 0x02))
    {
       LOG("/********* PLL not locked */\n");
    }
    WaitForAS3980();
    CLREXTIRQ();
    ENEXTIRQ();
}

void as3993Reset(void)
{
    as3993EnterPowerDownMode();
    delay_ms(1);
    as3993ExitPowerDownMode();
}

void as3993ResetDoNotPreserveRegisters(void)
{
    EN(LOW);
    delay_ms(1);
    EN(HIGH);
    delay_us(10);
    as3993WaitForStartup();
}

void as3993EnterPowerNormalMode()
{
    u8 stat;

    as3993ExitPowerDownMode();      //ensure that EN is high
    stat = as3993SingleRead(AS3993_REG_STATUSCTRL);
    stat &= 0x7F;
    as3993SingleWrite(AS3993_REG_STATUSCTRL, stat);
    as3993AntennaPower(0);
}

void as3993ExitPowerNormalMode()
{
}

void as3993EnterPowerNormalRfMode()
{
    u8 stat;

    as3993ExitPowerDownMode();      //ensure that EN is high
    stat = as3993SingleRead(AS3993_REG_STATUSCTRL);
    stat &= 0x7F;
    as3993SingleWrite(AS3993_REG_STATUSCTRL, stat);
    as3993AntennaPower(1);
}

void as3993ExitPowerNormalRfMode()
{
}

void as3993EnterPowerStandbyMode()
{
    u8 stat;

    as3993ExitPowerDownMode();      //ensure that EN is high
    as3993AntennaPower(0);
    stat = as3993SingleRead(AS3993_REG_STATUSCTRL);
    stat |= 0x80;
    as3993SingleWrite(AS3993_REG_STATUSCTRL, stat);
}

void as3993ExitPowerStandbyMode()
{
    u8 stat;

    as3993ExitPowerDownMode();      //ensure that EN is high
    stat = as3993SingleRead(AS3993_REG_STATUSCTRL);
    stat &= ~0x80;
    as3993SingleWrite(AS3993_REG_STATUSCTRL, stat);
}

void as3993WaitForStartup(void)
{
    u8 osc_ok, version;
    u8 myBuf[2];
    u16 count = 0;

    do
    {
        version = as3993ReadChipVersion() & 0x60;   //ignore revision
        osc_ok = as3993SingleRead(AS3993_REG_AGCANDSTATUS);
        count++;
    }
    while (!(((version == 0x60) && (osc_ok & 0x01)) || (count > 250)));    //wait for startup
    delay_us(500);
    as3993ContinuousRead(AS3993_REG_IRQSTATUS1, 2, &myBuf[0]);    // ensure that IRQ bits are reset
    as3993ClrResponse();
    delay_ms(2);            // give AS3993 some time to fully initialize
}

void as3993AntennaPower( u8 on)
{
    u8 val;
    unsigned count;
    val = as3993SingleRead(AS3993_REG_STATUSCTRL);

    if (on)
    {
        if ((val & 0x03) == 0x03)
            return;
        val |= 3;

#ifdef EXTPA
        EN_PA(PA_ON);
        delay_us(300); /* Give PA LDO time to get to maximum power */
#endif
        as3993SingleWrite(AS3993_REG_STATUSCTRL, val);
    }
    else
    {
        if ((val & 0x03) != 0x03)
            return;
        val &= ~3;
        as3993SingleWrite(AS3993_REG_STATUSCTRL, val);
        /* Wait for antenna being switched off */
        count = 500;
        while(count-- && (as3993SingleRead(AS3993_REG_AGCANDSTATUS) & 0x04))
        {
            delay_us(100);
        }
#ifdef EXTPA
        EN_PA(PA_OFF);
#endif
#ifdef RFDATARECEIVELED
        RFDATARECEIVELED(LEDOFF); 
#endif
    }

    if(on) delay_ms(6); /* according to standard we have to wait 1.5ms before issuing commands  */
}

void as3993SaveSensitivity( )
{
    as3993SavedSensRegs[0] = as3993SingleRead(AS3993_REG_RXMIXERGAIN);
}

void as3993RestoreSensitivity( )
{
    as3993SingleWrite(AS3993_REG_RXMIXERGAIN, as3993SavedSensRegs[0]);
}

s8 as3993GetSensitivity( )
{
    s8 sensitivity = 0;
    u8 reg0a, reg0d, gain;

    reg0d = as3993SingleRead(AS3993_REG_MISC1);
    reg0a = as3993SingleRead(AS3993_REG_RXMIXERGAIN);

    if ((reg0d & 0x04))
    { /* single ended input mixer*/
        sensitivity -= AS3993_NOM_SENSITIVITY;
        if ( (reg0a & 0x03) == 0x03 ) /* 6dB increase */
        {
            sensitivity -= 6;
        }
        else if ( (reg0a & 0x03) == 0x00 ) /* 6dB decrease */
        {
            sensitivity += 6;
        }
    }
    else
    { /* differential input mixer */
        sensitivity -= AS3993_NOM_SENSITIVITY;
        if ( reg0a & 0x01 )  /* mixer attenuation */
        {
            sensitivity += 8;
        }
        if ( reg0a & 0x02)  /* differential mixer gain increase */
        {
            sensitivity -= 10;
        }
    }
    gain = (reg0a >> 6) * 3;
    if ( reg0a & 0x20 )
    { /* RX Gain direction: increase */
        sensitivity -= gain;
    }
    else
    {
        sensitivity += gain;
    }
    return sensitivity;
}

s8 as3993SetSensitivity( s8 minimumSignal )
{
    u8 reg0d, reg0a, gain;

    reg0a = as3993SingleRead(AS3993_REG_RXMIXERGAIN);
    reg0d = as3993SingleRead(AS3993_REG_MISC1);

    reg0a &= 0x1C;
    if ((reg0d & 0x04))
    { /* single ended input mixer*/
        minimumSignal += AS3993_NOM_SENSITIVITY;
        if ( minimumSignal >= 6 )
        {
            minimumSignal -= 6; /* 6dB gain decrease */
            reg0a |= 0;
        }
        else if ( minimumSignal <= -6 )
        {
            minimumSignal += 6; /* 6dB gain increase */
            reg0a |= 3;
        }
        else
            reg0a |= 1;         /* nominal gain */
    }
    else
    { /* differential input mixer */
        minimumSignal += AS3993_NOM_SENSITIVITY;
        if ( minimumSignal >= 8 )   /* mixer attenuation */
        {
            minimumSignal -= 8;
            reg0a |= 1;
        }
        if ( minimumSignal <= -10 ) /* differential mixer gain increase */
        {
            minimumSignal += 10;
            reg0a |= 2;
        }
    }
    if ( minimumSignal > 0)
    {
        reg0a &= ~0x20;             /* RX Gain direction: decrease */
        gain = minimumSignal / 3;
        if ( gain>3 ) gain = 3;
        minimumSignal -= gain*3;
        
    }
    else
    {
        reg0a |= 0x20;              /* RX Gain direction: increase */
        gain = (-minimumSignal+2) / 3;
        if ( gain>3 ) gain = 3;
        minimumSignal += gain*3;
    }
    reg0a |= (gain<<6);
    as3993SingleWrite(AS3993_REG_RXMIXERGAIN, reg0a);
    return minimumSignal;
}

/* ADC Values are in sign magnitude representation -> convert */
#define CONVERT_ADC_TO_NAT(A) (((A)&0x80)?((A)&0x7f):(0 - ((A)&0x7f)))
s8 as3993GetADC( void )
{
    s8 val;
    as3993SingleCommand(AS3993_CMD_TRIGGERADCCON);
    delay_us(20); /* according to spec */
    val = as3993SingleRead(AS3993_REG_ADC);
    val = CONVERT_ADC_TO_NAT(val);
    return val;
}

u16 as3993GetReflectedPower( void )
{
    u16 value;
    s8 adcVal;
    u8 regMeas, regStatus;
    regStatus = as3993SingleRead(AS3993_REG_STATUSCTRL);
    regMeas = as3993SingleRead(AS3993_REG_MEASUREMENTCONTROL);
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, regMeas & ~0xC0);  /* disable the OAD pin outputs */

    as3993SingleWrite(AS3993_REG_STATUSCTRL, regStatus | 0x03 ); /* Receiver and transmitter are on */
    as3993SingleCommand(AS3993_CMD_BLOCKRX); /* Reset the receiver - otherwise the I values seem to oscillate */
    as3993SingleCommand(AS3993_CMD_ENABLERX);
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, 0x01); /* Mixer A DC */
    delay_us(300); /* settling time */
    value = as3993GetADC();
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, 0x02); /* Mixer B DC */
    delay_us(300); /* settling time */
    adcVal = as3993GetADC();
    as3993SingleCommand(AS3993_CMD_BLOCKRX); /* Disable the receiver since we enabled it before */

    /* mask out shifted in sign bits */
    value = (value & 0xff) | (adcVal << 8);
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, regMeas);
    as3993SingleWrite(AS3993_REG_STATUSCTRL, regStatus);
    return value;
}

u16 as3993GetReflectedPowerNoiseLevel( void )
{
    u16 value;
    s8 i_0, q_0;
    u8 regMeas, regStatus;
    regStatus = as3993SingleRead(AS3993_REG_STATUSCTRL);
    regMeas = as3993SingleRead(AS3993_REG_MEASUREMENTCONTROL);
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, regMeas & ~0xC0);  /* disable the OAD pin outputs */

    /* First measure the offset which might appear with disabled antenna */
    as3993SingleWrite(AS3993_REG_STATUSCTRL, (regStatus | 2) & (~1) ); /* Field off, receiver on */
    as3993SingleCommand(AS3993_CMD_BLOCKRX); /* Reset the receiver - otherwise the I values seem to oscillate */
    as3993SingleCommand(AS3993_CMD_ENABLERX);
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, 0x01); /* Mixer A DC */
    delay_us(300); /* settling time */
    i_0 = as3993GetADC();
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, 0x02); /* Mixer B DC */
    delay_us(300); /* settling time */
    q_0 = as3993GetADC();

    value = (i_0 & 0xff) | (q_0 << 8);
    as3993SingleWrite(AS3993_REG_MEASUREMENTCONTROL, regMeas);
    as3993SingleWrite(AS3993_REG_STATUSCTRL, regStatus);
    return value;

}

s8 as3993TxRxGen2Bytes(u8 cmd, u8 *txbuf, u16 txbits, 
                               u8 *rxbuf, u16 *rxbits,
                               u8 norestime, u8 followCmd,
                               u8 waitTxIrq)
{
    static u8 currCmd;
    u8 buf[2];
    u8 rxbytes = (*rxbits + 7) / 8;
    u8 checkRxLength = 0;
    u16 rxs = 0;
    u8 rxed = 0;
    if (rxbits)
    {
        rxs = *rxbits;
        *rxbits = 0;
        checkRxLength = 1;
    }
    if(rxbuf || rxs)
    {
        as3993SingleWrite(AS3993_REG_RXNORESPONSEWAITTIME, norestime);
    }
    if (rxs)
    {
        buf[1] = rxs & 0xff;
        buf[0] = ((rxs>>8) & 0x0F) | 0x10;    // set auto_errcode_rxl
        as3993ContinuousWrite(AS3993_REG_RXLENGTHUP,buf,2);
    }
    if (txbits)
    {
        u8 txbytes = (txbits + 7)/8;
        u8 totx = txbytes;
        if (totx > 24) totx = 24;

        /* set up two bytes tx length register */
        buf[1] = (txbits % 8) << 1;
        buf[1] |= (txbits / 8) << 4;
        buf[0] = txbits / 128;
        as3993ContinuousWrite(AS3993_REG_TXLENGTHUP,buf,2);

        as3993CommandContinuousAddress(&cmd, 1, AS3993_REG_FIFO, txbuf, totx);
        txbytes -= totx;
        txbuf += totx;
        while (txbytes)
        {
            totx = txbytes;
            if (totx > 16) totx = 18;

            as3993WaitForResponse(RESP_FIFO);
            as3993ClrResponseMask(RESP_FIFO);
            as3993ContinuousWrite(AS3993_REG_FIFO,txbuf,totx);
            txbytes -= totx;
            txbuf += totx;
        }
        currCmd = cmd;
    }
    else if (cmd)
    {
        as3993SingleCommand(cmd);
        currCmd = cmd;
    }
    if (currCmd && waitTxIrq)
    {
        as3993WaitForResponse(RESP_TXIRQ);
        as3993ClrResponseMask(RESP_TXIRQ | RESP_FIFO);
    }
    if (rxbits && !rxs && !rxbuf)
    {   /* we do not want to receive any data */
        as3993SingleCommand(AS3993_CMD_BLOCKRX);
        buf[1] = 0;
        buf[0] = 0;
        as3993ContinuousWrite(AS3993_REG_RXLENGTHUP,buf,2);
    }
    if (rxbuf)
    {
        u8 count;
        u16 resp;
        if (0xff == norestime)
        {
            u16 responseTimout = 30;
            switch (getGen2IntConfig()->linkFreq)
            {
                case GEN2_LF_40 : responseTimout = 120; break;
                case GEN2_LF_160: responseTimout = 50; break;
                case GEN2_LF_213: responseTimout = 40; break;
             }
            as3993WaitForResponseTimed(RESP_RXDONE_OR_ERROR | RESP_FIFO, responseTimout);
        }
        else
        {
            as3993WaitForResponse(RESP_RXDONE_OR_ERROR | RESP_FIFO);
        }
        while (as3993GetResponse() & RESP_FIFO &&
            !(as3993GetResponse() & RESP_RXIRQ))
        {
            count = 18;
            if (checkRxLength && count > rxbytes)
            {
#if AS3993DEBUG
                LOG("limiting1 %hhx %hhx resp %hx\n",count,rxbytes,as3993GetResponse());
#endif
                count = rxbytes;
            }
            as3993FifoRead(count, rxbuf);
            rxbuf += count;
            rxbytes -= count;
            rxed += count;
            as3993ClrResponseMask(RESP_FIFO);
            if (checkRxLength && rxbytes == 0)   //we do not want to receive more data
                return AS3993_ERR_RXCOUNT;
            as3993WaitForResponse(RESP_RXDONE_OR_ERROR | RESP_FIFO);
        }
        as3993WaitForResponse(RESP_RXDONE_OR_ERROR);
        resp = as3993GetResponse();
        as3993ClrResponse();
        if (followCmd && !(resp & (RESP_NORESINTERRUPT | RESP_RXERR)))
        {
            switch (getGen2IntConfig()->linkFreq)// prevent violate T2 min
            {
                case GEN2_LF_40 : delay_us(200); break;
                case GEN2_LF_160: delay_us(30); break;
                case GEN2_LF_213: delay_us(15); break;
                case GEN2_LF_256: delay_us(6); break;
            }
            as3993SingleCommand(followCmd);
            currCmd = followCmd;
        }
        count = as3993SingleRead(AS3993_REG_FIFOSTATUS) & 0x1F; /*get the number of bytes */
        if (checkRxLength && count > rxbytes)
        {
#if AS3993DEBUG
            LOG("limiting %hhx %hhx\n",count,rxbytes);
#endif
            count = rxbytes;
            resp |= RESP_RXCOUNTERROR;
        }
        if (count)
            as3993FifoRead(count, rxbuf);
        rxbytes -= count;
        rxed += count;
        if (rxbits)
            *rxbits = 8 * rxed;

        if (getGen2IntConfig()->linkFreq == GEN2_LF_40)
        {
            delay_us(100);
        }
#if RUN_ON_AS3980   /* on AS3980 header IRQ is actually 2nd-byte IRQ -> ignore if no
                                        command which expects header bit was sent */
        if (currCmd != AS3993_CMD_TRANSMCRCEHEAD)
            resp &= ~RESP_HEADERBIT;
#endif
        if(resp & (RESP_NORESINTERRUPT | RESP_RXERR | RESP_HEADERBIT | RESP_RXCOUNTERROR))
        {
#if AS3993DEBUG
            if(resp & RESP_RXERR)
            {   //log errors (except no response)
                //LOG("rx error=%hx , rxed=%hhx , rxlen=%hx\n",resp, rxed);
                //LOGDUMP(rxbuf, rxed);
            }
#endif
            if (resp & RESP_NORESINTERRUPT)
                return AS3993_ERR_NORES;
            if (resp & RESP_HEADERBIT && currCmd == AS3993_CMD_TRANSMCRCEHEAD)
                return AS3993_ERR_HEADER;
            //if (resp & RESP_FIFOOVERFLOW)   return AS3993_ERR_FIFO_OVER;
            if (resp & RESP_RXCOUNTERROR)
            {
                if (!(resp & RESP_RXERR))   //as3980 produces irq_err2 (without irq_rxerr) if new epc is read 500ms after last one.
                {
                    WaitForAS3980();        // Wait for tx to be re-enabled.
                }
                return AS3993_ERR_RXCOUNT;
            }
            if (resp & RESP_PREAMBLEERROR)  return AS3993_ERR_PREAMBLE;
            if (resp & RESP_CRCERROR)       return AS3993_ERR_CRC;
            return -1;
        }
    }
    return ERR_NONE;
}
