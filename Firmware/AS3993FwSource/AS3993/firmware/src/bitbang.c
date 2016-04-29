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
 *  \brief Bitbang engine
 *
 *  The bitbang module is needed to support serial protocols via bitbang technique
 */

/*
******************************************************************************
* INCLUDES
******************************************************************************
*/

#include "as3993_config.h"
#include "bitbang.h"
#include "logger.h"
#include "timer.h"

#if ISO6B
/*
******************************************************************************
* LOCAL VARIABLES
******************************************************************************
*/
static volatile u8 bbLength_;
static volatile t_bbData *bbData_;
static volatile u8 bbFin;
/*
******************************************************************************
* GLOBAL FUNCTIONS
******************************************************************************
*/
void bbSetup(s16 microsec)
{
    u32 systemClock = SYSCLK;
    s16 prVal = systemClock/1000000 * microsec;
    
    T4CONbits.TON = 0;
    T4CON = 0x00;
    TMR4 = 0;

    PR4 = prVal;

    _T4IP = 3;  // Timer4 interrupt priority 2 (low)
    _T4IF = 0;
    _T4IE = 0;

    bbLength_ = 0;
    bbFin = 0;

}

void bbRun(t_bbData *bbdata, s16 length)
{
    bbFin = 0;
    bbData_ = bbdata;
    bbLength_ = length;

    TMR4 = 0;
    _T4IE = 1;
    T4CONbits.TON = 1; // Start Timer
    IEC5bits.USB1IE = 0;


    while(bbLength_);  // wait until bits are bang't
    while(!bbFin);     //wait for one extra clock

    _T4IE = 0;
    IEC5bits.USB1IE = 1;
    T4CONbits.TON = 0; // Stop Timer
}

/*
******************************************************************************
* LOCAL FUNCTIONS
******************************************************************************
*/
void INTERRUPT timer4Isr(void)	// interrupt handler for Timer4
{
   s8 bangbit = 0;

    if (bbLength_)
    {
        _T4IF = 0;
        (*bbData_).length --;
        bangbit = (*bbData_).bbdata >> (*bbData_).length;
           
        if ((*bbData_).length == 0)
        {
            bbData_ ++;
            bbLength_ --;
        }
     
        BB_PIN = bangbit &1;

    }
    else
    {
        BB_PIN = 1;
        _T4IF = 0;
        bbFin = 1;
    }

}

#endif
