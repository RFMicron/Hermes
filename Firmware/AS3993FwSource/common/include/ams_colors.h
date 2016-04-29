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

/*! \file ams_colors.h
 *
 *  \author F. Lobmaier
 *
 *  \brief Constants shared between GUI and firmware concerning the ams
 *         corporate colors.
 */

#ifndef AMS_COLORS_H
#define AMS_COLORS_H

// AMS color definitions
// defined as strings to be easily used with Qt stylesheet definitions
// (e.g. for USB indicator in statusbar)
// format for firmware usage to be defined
#define AMSCOLOR_GRAY_95	"#2c292d"   // R:  44 G:  41 B:  45
#define AMSCOLOR_GRAY 		"#46555f"   // R:  70 G:  85 B:  95
#define AMSCOLOR_GRAY_70 	"#7d888f"   // R: 125 G: 136 B: 143
#define AMSCOLOR_GRAY_45	"#acb2b7"   // R: 172 G: 178 B: 183

#define AMSCOLOR_YELLOW		"#f7e600"   // R: 247 G: 230 B:   0

#define AMSCOLOR_BLUE		"#0075b0"   // R:   0 G: 117 B: 176
#define AMSCOLOR_BLUE_70	"#4c9ec8"   // R:  76 G: 158 B: 200
#define AMSCOLOR_BLUE_45	"#8cc1db"   // R: 140 G: 193 B: 219

#define AMSCOLOR_GREEN_95   "#41ab02"   // R:  65 G: 171 B:   2
#define AMSCOLOR_GREEN		"#009f69"   // R:   0 G: 159 B: 105
#define AMSCOLOR_LIGHTGREEN "#78f078"   // R: 120 G: 240 B: 120

#define AMSCOLOR_ORANGE     "#fd5613"   // R: 253 G:  86 B:  19
#define AMSCOLOR_ORANGE_70  "#fe8e12"   // R: 254 G: 142 B:  18
#define AMSCOLOR_ORANGE_45  "#fdba41"   // R: 253 G: 186 B:  65

#define AMSCOLOR_RED		"#df5353"   // R: 223 G:  83 B:  83

#define AMSCOLOR_VIOLA		"#8b2dbd"   // R: 139 G:  45 B: 189

#define AMSCOLOR_BLACKBERRY "#654447"   // R: 101 G:  68 B:  71
#define AMSCOLOR_SKIN		"#ffb690"   // R: 255 G: 182 B: 144
#define AMSCOLOR_WOOD		"#8c6239"   // R: 140 G:  98 B:  57

// AMS color macros
#define AMSCOLOR_CSS_COLOR(value) QString("color: ") + QString(value) + QString(";")
#define AMSCOLOR_CSS_BGCOLOR(value) QString("background-color: ") + QString(value) + QString(";")


#endif //AMS_COLORS_H
