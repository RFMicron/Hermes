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
  * @brief This file is the include file for the timer.c file.
  *
  * @author Ulrich Herrmann
  * @author Bernhard Breinbauer
  */

#ifndef __TIMER_H__
#define __TIMER_H__
#include "global.h"
#include <p24Fxxxx.h>
#include <libpic30.h>

/* Porting note: replace delay functions which with functions provided with your controller or use timer */
#define delay_ms(ms)    { __delay_ms(ms); }
#define delay_us(us)    { __delay_us(us); }

/*! 
  Start measurement with the slow timer. It has a period of 10ms.
  */
void slowTimerStart( );

/*! 
 * Measure time passed since last call of slowTimerStart(). The resolution is
 * ~10ms. The return value is in ms.
 */
u16 slowTimerValue( );

/*!
 * Stops the slow timer. This disables the interrupt and therefore reduces the
 * load of the CPU.
 */
void slowTimerStop( );

/*!
 * Set up timers. The timers are used for:
 * T1: 
 * T2:
 * T3: slow timer for measuring bigger intervals, resolution is ~10ms
 * T4:
 * T5:
 */
void timerInit();
#endif
