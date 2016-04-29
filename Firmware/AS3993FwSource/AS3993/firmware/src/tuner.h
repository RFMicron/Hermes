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
  * @brief This file provides declarations for tuner related functions.
  *
  * @author Ulrich Herrmann
  * @author Bernhard Breinbauer
  */

#ifndef __TUNER_H__
#define __TUNER_H__

#include "global.h"
#include "as3993_config.h"

/*------------------------------------------------------------------------- */
#define TUNER_CONFIG_CIN (TUNER_CONFIG & TUNER_CIN)
#define TUNER_CONFIG_CLEN (TUNER_CONFIG & TUNER_CLEN)
#define TUNER_CONFIG_COUT (TUNER_CONFIG & TUNER_COUT)


/**
  * This struct stores a set of tuner settings and the associated measured
  * reflected power.
  */
typedef struct {
    u8 cin;
    u8 clen;
    u8 cout;
    u16 reflectedPower;
} TunerParameters;

/**
  * This struct stores a set of tuner settings and the associated measured
  * reflected power.
  */
typedef struct {
    void (*cinSenFn)(int);
    void (*clenSenFn)(int);
    void (*coutSenFn)(int);
} TunerConfiguration;

/**
 * Initializes the tuner caps to value 0. This function requires an already set-up
 * SPI communication (see initInterface() in serialinterface.c).
 */
extern void tunerInit(const TunerConfiguration * config);

/**
 * Sets the tuner cap associated with component to the value val. The value val is the register
 * value of the tuner cap (not a real Farad value). The maximum value of val depends on the
 * used component on the board. For the PE64904 the maximum value is 31.
 * @param component The tuner cap which should be changed. 
 *      Allowed values are: #TUNER_CIN, #TUNER_CLEN, #TUNER_COUT.
 * @param val The register value which will be written to the selected tuner cap.
 * @param config The tuner's configuration to be used
 */
extern void tunerSetCap(const TunerConfiguration *config, u8 component, u8 val);

/**
 * Simple automatic tuning function. This function tries to find an optimized tuner setting (minimal reflected power).
 * The function starts at the current tuner setting and modifies the tuner caps until a
 * setting with a minimum of reflected power is found. When changing the tuner further leads to
 * an increase of reflected power the algorithm stops.
 * Note that, although the algorithm has been optimized to not immediately stop at local minima
 * of reflected power, it still might not find the tuner setting with the lowest reflected
 * power. The algorithm of tunerOneHillClimb() is probably producing better results, but it
 * is slower.
 * @param p Struct which contains the found tuning optimum.
 * @param maxSteps Number of maximum steps which should be performed when searching for tuning optimum.
 * @param config The tuner's configuration to be used
 */
extern void tunerOneHillClimb(const TunerConfiguration *config, TunerParameters *p, u16 maxSteps);

/**
 * Sophisticated automatic tuning function. This function tries to find an optimized tuner setting (minimal reflected power).
 * The function splits the 3-dimensional tuner-setting-space (axis are Cin, Clen and Cout) into segments
 * and searches in each segment (by using tunerOneHillClimb()) for its local minimum of reflected power.
 * The tuner setting (point in tuner-setting-space) which has the lowest reflected power is
 * returned in parameter res.
 * This function has a much higher probability to find the tuner setting with the lowest reflected
 * power than tunerOneHillClimb() but on the other hand takes much longer.
 * @param *config is a structure with tuner settings.
 * @param res Struct which contains the found tuning optimum.
 */
extern void tunerMultiHillClimb(const TunerConfiguration *config, TunerParameters *res );

/**
 * Exhaustive automatic tuning function. This function tries to find an optimized tuner setting (minimal reflected power).
 * The function applies each and every possible tuner setting and returns the setting with the lowest
 * value of reflected power in parameter res. This algorithm ensures that the optimum tuner setting
 * is found but takes a very long time.
 * @param res Struct which contains the found tuning optimum.
 * @param config The tuner's configuration to be used
 */
extern void tunerTraversal(const TunerConfiguration *config, TunerParameters *res );

/**
 * Applies the values cin, clen and cout to the tuner network. The parameter values are
 * register values of the associated components not values in Farad.
 * The maximum value of val depends on the used component on the board. For the PE64904 the maximum value is 31.
 * @param cin Value which is applied to cap Cin of the tuner network.
 * @param clen Value which is applied to cap Clen of the tuner network.
 * @param cout Value which is applied to cap Cout of the tuner network.
 * @param config The tuner's configuration to be used
 */
extern void tunerSetTuning(const TunerConfiguration *config, u8 cin, u8 clen, u8 cout );

/**
 * Measures the reflected power by utilizing as3993GetReflectedPower() and
 * as3993GetReflectedPowerNoiseLevel(). This function is used by the automatic tuning algorithms
 * to find the tuner setting with the lowest reflected power.
 * @return Reflected power measured with the current tuner settings.
 */
extern u16 tunerGetReflected(void);

#endif
